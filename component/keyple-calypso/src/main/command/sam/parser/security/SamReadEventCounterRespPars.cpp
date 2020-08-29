/******************************************************************************
 * Copyright (c) 2020 Calypso Networks Association                            *
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

#include "SamReadEventCounterRespPars.h"

/* Common */
#include "ClassNotFoundException.h"

/* Core */
#include "ApduResponse.h"

/* Calypso */
#include "CalypsoSamCounterOverflowException.h"
#include "CalypsoSamIllegalParameterException.h"

namespace keyple {
namespace calypso {
namespace command {
namespace sam {
namespace parser {
namespace security {

using namespace keyple::calypso::command::sam;
using namespace keyple::calypso::command::sam::exception;
using namespace keyple::core::seproxy::message;

const std::map<int, std::shared_ptr<StatusProperties>>
    SamReadEventCounterRespPars::STATUS_TABLE = {
    {
        0x6900,
        std::make_shared<StatusProperties>(
            "An event counter cannot be incremented.",
            typeid(CalypsoSamCounterOverflowException))
    }, {
        0x6A00,
        std::make_shared<StatusProperties>(
            "Incorrect P1 or P2.",
            typeid(CalypsoSamIllegalParameterException))
    }, {
        0x6200,
        std::make_shared<StatusProperties>(
            "Correct execution with warning: data not signed.",
            typeid(ClassNotFouncException))
    }
};

SamReadEventCounterRespPars::SamReadEventCounterRespPars(
  const std::shared_ptr<ApduResponse> response,
  SamReadEventCounterCmdBuild* builder)
: AbstractSamResponseParser(response) {}

const std::map<int, std::shared_ptr<StatusProperties>>&
    SamReadEventCounterRespPars::getStatusTable() const
{
    return STATUS_TABLE;
}

std::vector<uint8_t> SamReadEventCounterRespPars::getCounterData() const
{
    return isSuccessful() ? mResponse->getDataOut() : std::vector<uint8_t>();
}

}
}
}
}
}
}
