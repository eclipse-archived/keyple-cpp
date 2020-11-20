/**************************************************************************************************
 * Copyright (c) 2020 Calypso Networks Association                                                *
 * https://www.calypsonet-asso.org/                                                               *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "SvDebitRespPars.h"

/* Calypso */
#include "CalypsoPoSessionBufferOverflowException.h"
#include "CalypsoSamCounterOverflowException.h"
#include "CalypsoSamAccessForbiddenException.h"
#include "CalypsoPoIllegalParameterException.h"
#include "CalypsoPoSecurityDataException.h"

/* Common */
#include "ClassNotFoundException.h"
#include "IllegalStateException.h"

namespace keyple {
namespace calypso {
namespace command {
namespace po {
namespace parser {
namespace storedvalue {

using namespace keyple::calypso::command::po::exception;
using namespace keyple::calypso::command::sam::exception;
using namespace keyple::common::exception;

const std::map<int, std::shared_ptr<StatusProperties>> SvDebitRespPars::STATUS_TABLE = {
    {
        0x6400,
        std::make_shared<StatusProperties>(
            "Too many modifications in session.",
            typeid(CalypsoPoSessionBufferOverflowException)),
    }, {
        0x6700,
        std::make_shared<StatusProperties>(
            "Lc value not supported.",
            typeid(CalypsoPoIllegalParameterException)),
    }, {
        0x6900,
        std::make_shared<StatusProperties>(
            "Transaction counter is 0 or SV TNum is FFFEh or FFFFh.",
            typeid(CalypsoSamCounterOverflowException)),
    }, {
        0x6985,
        std::make_shared<StatusProperties>(
            "Preconditions not satisfied.",
            typeid(CalypsoSamAccessForbiddenException)),
    }, {
        0x6988,
        std::make_shared<StatusProperties>(
            "Incorrect signatureHi.",
            typeid(CalypsoPoSecurityDataException)),
    }, {
        0x6200,
        std::make_shared<StatusProperties>(
            "Successful execution, response data postponed until session closing.")
    }, {
        0x9000,
        std::make_shared<StatusProperties>("Success")
    }
};

SvDebitRespPars::SvDebitRespPars(std::shared_ptr<ApduResponse> response, SvDebitCmdBuild* builder)
: AbstractPoResponseParser(
      response,
      reinterpret_cast<AbstractPoCommandBuilder<AbstractPoResponseParser>*>(builder))
{
    /* The permitted lengths are 0 (in session), 3 (not 3.2) or 6 (3.2) */
    if (response->getDataOut().size() != 0 &&
        response->getDataOut().size() != 3 &&
        response->getDataOut().size() != 6)
        throw IllegalStateException("Bad length in response to SV Debit command.");
}

const std::vector<uint8_t> SvDebitRespPars::getSignatureLo() const
{
    return getApduResponse()->getDataOut();
}

const std::map<int, std::shared_ptr<StatusProperties>>& SvDebitRespPars::getStatusTable() const
{
    return STATUS_TABLE;
}

const std::string SvDebitRespPars::toString() const
{
    return "SV Debit";
}

}
}
}
}
}
}
