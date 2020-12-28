/******************************************************************************
 * Copyright (c) 2018 Calypso Networks Association                            *
 * https://www.calypsonet-asso.org/                                           *
 *                                                                            *
 * See the NOTICE file(s) distributed with this work for additional           *
 * information regarding copyright ownership.                                 *
 *                                                                            *
 * This program and the accompanying materials are made available under the   *
 * terms of the Eclipse Public License 2.0 which is available at              *
 * http://www.eclipse.org/legal/epl-2.0                                       *
 *                                                                            *
 * SPDX-License-Identifier: EPL-2.0                                           *
 ******************************************************************************/

#include "PcscReaderImpl.h"

/* Core */
#include "ByteArrayUtil.h"
#include "KeypleBaseException.h"
#include "KeypleChannelControlException.h"
#include "KeypleIOReaderException.h"
#include "KeypleReaderException.h"
#include "ObservableReaderStateService.h"
#include "SeProtocol.h"
#include "SmartInsertionMonitoringJob.h"
#include "SmartRemovalMonitoringJob.h"
#include "WaitForStartDetect.h"
#include "WaitForSeInsertion.h"
#include "WaitForSeProcessing.h"
#include "WaitForSeRemoval.h"

/* PC/SC Plugin */
#include "PcscTerminalException.h"

namespace keyple {
namespace plugin {
namespace pcsc {

using namespace keyple::core::seproxy::exception;
using namespace keyple::core::seproxy::plugin;
using namespace keyple::core::seproxy::plugin::local;
using namespace keyple::core::seproxy::plugin::local::monitoring;
using namespace keyple::core::seproxy::plugin::local::state;
using namespace keyple::core::seproxy::protocol;
using namespace keyple::core::util;

const std::string PcscReaderImpl::PROTOCOL_T0   = "T=0";
const std::string PcscReaderImpl::PROTOCOL_T1   = "T=1";
const std::string PcscReaderImpl::PROTOCOL_T_CL = "T=CL";
const std::string PcscReaderImpl::PROTOCOL_ANY  = "T=0";

PcscReaderImpl::PcscReaderImpl(const std::string& pluginName,
                               PcscTerminal& terminal)
: AbstractObservableLocalReader(pluginName, terminal.getName()),
  loopWaitSeRemoval(true), terminal(terminal),
  transmissionMode(TransmissionMode::NONE)
{
    this->executorService = std::make_shared<MonitoringPool>();
    this->stateService    = initStateService();

    logger->debug("[PcscReaderImpl] constructor => using terminal %\n",
                  terminal.getName());

    /*
     * Using null values to use the standard method for defining default values
     *
     * /!\ Must use hardcoded strings here as
     *     PcscReader::SETTING_KEY_TRANSMISSION_MODE and the three other strings
     *     are not yet instantiated.
     */
    try {
        setParameter("transmission_mode", "");
        setParameter("protocol", "");
        setParameter("mode", "");
        setParameter("disconnect", "");
    } catch (KeypleBaseException& e) {
        (void)e;
        /* Can not fail with null value */
    }
}

std::shared_ptr<ObservableReaderStateService> PcscReaderImpl::initStateService()
{
    std::map<MonitoringState, std::shared_ptr<AbstractObservableState>> states;

    states.insert(
        std::pair<MonitoringState, std::shared_ptr<AbstractObservableState>>(
            MonitoringState::WAIT_FOR_START_DETECTION,
            std::make_shared<WaitForStartDetect>(this)));

    states.insert(
        std::pair<MonitoringState, std::shared_ptr<AbstractObservableState>>(
            MonitoringState::WAIT_FOR_SE_INSERTION,
            std::make_shared<WaitForSeInsertion>(
                this, std::make_shared<SmartInsertionMonitoringJob>(this),
                executorService)));

    states.insert(
        std::pair<MonitoringState, std::shared_ptr<AbstractObservableState>>(
            MonitoringState::WAIT_FOR_SE_PROCESSING,
            std::make_shared<WaitForSeProcessing>(
                this, std::make_shared<SmartRemovalMonitoringJob>(this),
                executorService)));

    states.insert(
        std::pair<MonitoringState, std::shared_ptr<AbstractObservableState>>(
            MonitoringState::WAIT_FOR_SE_REMOVAL,
            std::make_shared<WaitForSeRemoval>(
                this, std::make_shared<SmartRemovalMonitoringJob>(this),
                executorService)));

    return std::make_shared<ObservableReaderStateService>(
        this, states, MonitoringState::WAIT_FOR_START_DETECTION);
}

void PcscReaderImpl::closePhysicalChannel()
{
    try {
        logger->debug("[%] closePhysicalChannel\n", getName());
        terminal.closeAndDisconnect(cardReset);

    } catch (PcscTerminalException& e) {
        throw KeypleChannelControlException("Error closing physical channel",
                                            e);
    }

    this->channelOpen = false;
}

bool PcscReaderImpl::checkSePresence()
{
    try {
        logger->debug("checkSePresence - calling isCardPresent\n");
        return terminal.isCardPresent(true);

    } catch (PcscTerminalException& e) {
        logger->debug("[%] checkSePresence - PcscTerminalException %\n", e);
    }

    return false;
}

bool PcscReaderImpl::waitForCardPresent()
{
    logger->debug("[%] waitForCardPresent => loop with latency of % ms\n",
                  getName(), insertLatency);

    /* Activate loop */
    loopWaitSe = true;

    /* Rearm flag for removal process as well */
    loopWaitSeRemoval = true;

    try {
        logger->trace("[%] waitForCardPresent => looping...\n", getName());
        while (loopWaitSe) {
            if (terminal.waitForCardPresent(insertLatency)) {
                /* Card inserted */
                 logger->debug("[%] waitForCardPresent => card inserted\n",
                               getName());
                return true;
            } else {
                /* Check if task has been cancelled ? */
            }
        }

        /* If loop was stopped */
        return false;

    } catch (PcscTerminalException& e) {
        throw KeypleIOReaderException(StringHelper::formatSimple(
            "[%s] Exception occurred in waitForCardPresent. "
            "Message: %s",
            this->getName(), e.getMessage()));
    }
}

void PcscReaderImpl::stopWaitForCard()
{
    loopWaitSe = false;
}

bool PcscReaderImpl::waitForCardAbsentNative()
{
    logger->debug("[%] waitForCardAbsentNative => loop with latency of "
                  "% ms\n", getName(), removalLatency);

    while(!loopWaitSeRemoval)
        /* A stopWaitForCardRemoval() has been triggered. Wait for finish. */
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    try {
        while (loopWaitSeRemoval) {
            logger->trace("[%] waitForCardAbsentNative => looping\n",getName());
            if (terminal.waitForCardAbsent(removalLatency)) {
                /* notify removal only if expected */
                if(loopWaitSeRemoval) {
                    /* Card removed */
                    return true;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        loopWaitSeRemoval = true;
        return false;

    } catch (PcscTerminalException& e) {
        throw KeypleIOReaderException(StringHelper::formatSimple(
            "[%s] Exception occurred in waitForCardAbsentNative. "
            "Message: %s", getName(), e.getMessage()));
    }
}

void PcscReaderImpl::stopWaitForCardRemoval()
{
    loopWaitSeRemoval = false;
}

std::vector<uint8_t> PcscReaderImpl::transmitApdu(
    const std::vector<uint8_t>& apduIn)
{
    logger->debug("transmitApdu\n");

    std::vector<uint8_t> response;

    try {
        response = terminal.transmitApdu(apduIn);
    } catch (PcscTerminalException& e) {
        logger->error("transmitApdu - PcscTerminalException %\n", e);
        throw KeypleIOReaderException("transmitApdu failed", e);
    } catch (std::invalid_argument& e) {
        /* Card could have been removed prematurely */
        logger->error("transmitApdu - std::invalid_argument %\n", e.what());
        throw KeypleIOReaderException("transmitApdu failed", e);
    }

    return response;
}

bool PcscReaderImpl::protocolFlagMatches(
    const std::shared_ptr<SeProtocol> protocolFlag)
{
    bool result;

    /* Test protocolFlag to check if ATR based protocol filtering is required */
    if (protocolFlag) {
        if (!isPhysicalChannelOpen()) {
            logger->debug("protocolFlagMatches - physical channel not open, "
                        "opening it\n");
            openPhysicalChannel();
        }
    }

    /*
     * The requestSet will be executed only if the protocol match the
     * requestElement.
     */
    std::string selectionMask = protocolsMap[protocolFlag];
    logger->debug("protocolFlagMatches - selectionMask: %\n", selectionMask);
    if (selectionMask == "") {
        throw KeypleReaderException(
            "Target selector mask not found!"); // nullptr));
    }

    Pattern* p      = Pattern::compile(selectionMask);
    std::string atr = ByteArrayUtil::toHex(this->terminal.getATR());
    if (!p->matcher(atr)->matches()) {
        logger->debug("[%] protocolFlagMatches => unmatching SE. "
                      "PROTOCOLFLAG = %\n", getName(), protocolFlag);
        result = false;
    } else {
        logger->debug("[%] protocolFlagMatches => matching SE. "
                      "PROTOCOLFLAG = %\n", getName(), protocolFlag);
        result = true;
    }
    //} else {
    //    // no protocol defined returns true
    //    result = true;
    //}

    return result;
}

void PcscReaderImpl::setParameter(const std::string& name,
                                  const std::string& value)
{    logger->debug("[%] setParameter => PCSC: Set a parameter. NAME = %,"
                  " VALUE = %\n", getName(), name, value);

    if (name == "") {
        throw std::invalid_argument("Parameter shouldn't be null\n");
    }

    /*
     * /!\ Must use hardcoded strings here as
     *     PcscReader::SETTING_KEY_TRANSMISSION_MODE and the three other strings
     *     are not yet instantiated.
     */
    if (name == "transmission_mode") {
        if (value == "")
            transmissionMode = TransmissionMode::NONE;
        else if (value == "contacts")
            transmissionMode = TransmissionMode::CONTACTS;
        else if (value == "contactless")
            transmissionMode = TransmissionMode::CONTACTLESS;
        else
            throw std::invalid_argument("Bad tranmission mode " + name + " : " +
                                        value);

    } else if (name == "protocol") {
        if (value == "" || value == "Tx")
            parameterCardProtocol = "*";
        else if (value == "T0")
            parameterCardProtocol = "T=0";
        else if (value == "T1")
            parameterCardProtocol = "T=1";
        else if (value == "TCL")
            parameterCardProtocol = "T=CL";
        else
            throw std::invalid_argument("Bad protocol " + name + " : " + value);

        /*
         * /!\ Java diff
         *
         * Actualize 'transmissionMode' according to 'parameterCardProtocol'.
         */
        if (parameterCardProtocol == PROTOCOL_T1 ||
            parameterCardProtocol == PROTOCOL_T_CL) {
            transmissionMode = TransmissionMode::CONTACTLESS;
        } else {
            transmissionMode = TransmissionMode::CONTACTS;
        }

    } else if (name == "mode") {
        if (value == "" || value == "shared") {
            if (cardExclusiveMode) {
                try {
                    terminal.endExclusive();
                } catch (PcscTerminalException& e) {
                    throw KeypleReaderException("Couldn't disable exclusive "
                                                "mode", e);
                }
            }
            cardExclusiveMode = false;
        } else if (value == "exclusive") {
            cardExclusiveMode = true;
        } else {
            throw std::invalid_argument("Parameter value not supported " +
                                        name + " : " + value);
        }
    } else if (name == "disconnect") {
        if (value == "" || value == "reset") {
            cardReset = true;
        } else if (value == "unpower") {
            cardReset = false;
        } else if (value == "eject" ||
                   value == "leave") {
            throw std::invalid_argument("This disconnection parameter is not "
                                        "supported by this plugin" +
                                        name + " : " + value);
        } else {
            throw std::invalid_argument("Parameters not supported : " + name +
                                        " : " + value);
        }
    } else {
        throw std::invalid_argument("This parameter is unknown! " + name +
                                    " : " + value);
    }
}

const std::map<const std::string, const std::string>
    PcscReaderImpl::getParameters() const
{
    std::map<const std::string, const std::string> parameters;

    /* Returning the protocol */
    std::string protocol = parameterCardProtocol;
    if (protocol == "*") {
        protocol = SETTING_PROTOCOL_TX;
    } else if (protocol == "T=0") {
        protocol = SETTING_PROTOCOL_T0;
    } else if (protocol == "T=1") {
        protocol = SETTING_PROTOCOL_T1;
    } else {
        throw IllegalStateException("Illegal protocol: " + protocol);
    }

    parameters.emplace(SETTING_KEY_PROTOCOL, protocol);

    /* The mode ? */
    if (!cardExclusiveMode) {
        parameters.emplace(SETTING_KEY_MODE, SETTING_MODE_SHARED);
    }

    return parameters;
}

const std::vector<uint8_t>& PcscReaderImpl::getATR()
{
    return terminal.getATR();
}

bool PcscReaderImpl::isPhysicalChannelOpen()
{
    return this->channelOpen;
}

void PcscReaderImpl::openPhysicalChannel()
{
    /*
     *  Init of the physical SE channel: if not yet established, opening of a
     * new physical channel
     */
    try {
        this->terminal.openAndConnect(parameterCardProtocol);
        if (cardExclusiveMode) {
            this->terminal.beginExclusive();
            logger->debug("[%] Opening of a physical SE channel in "
                          "exclusive mode\n", getName());
        } else {
            logger->debug("[%] Opening of a physical SE channel in "
                          "shared mode\n", getName());
        }
    } catch (PcscTerminalException& e) {
        throw KeypleChannelControlException(
            "Error while opening Physical Channel", e);
    }

    this->channelOpen = true;
}

const TransmissionMode& PcscReaderImpl::getTransmissionMode() const
{
    /*
     * /!\ Java diff
     *
     * getTransmissionMode() is Java can either return the actual value of
     * 'transmissionMode' or a value deducted from 'parameterCardProtocol' if
     * not set.
     *
     * This is a bit deceiving and 'transmissionMode' could never be set
     * correctly. Instead, I made sure 'transmissionMode' is set correctly
     * whenever 'parameterCardProtocol' changes (e.g. in setParameter())
     *
     * This allows getTranmissionMode() to be 'const', which seems right for a
     * 'getter'.
     */

    return transmissionMode;
}

}
}
}
