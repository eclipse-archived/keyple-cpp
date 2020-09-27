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

#include "PoGetChallengeCmdBuild.h"

/* Calypso */
#include "CalypsoPoCommand.h"
#include "PoGetChallengeRespPars.h"

/* Core */
#include "ApduResponse.h"

namespace keyple {
namespace calypso {
namespace command {
namespace po {
namespace builder {
namespace security {

using namespace keyple::calypso::command;
using namespace keyple::calypso::command::po;
using namespace keyple::calypso::command::po::parser::security;
using namespace keyple::core::seproxy::message;

PoGetChallengeCmdBuild::PoGetChallengeCmdBuild(PoClass poClass)
: AbstractPoCommandBuilder<PoGetChallengeRespPars>(
      std::make_shared<CalypsoPoCommand>(CalypsoPoCommand::GET_CHALLENGE),
      nullptr)
{
    mRequest = setApduRequest(poClass.getValue(), command, 0x01, 0x10, 0x08);
}

std::shared_ptr<PoGetChallengeRespPars>
    PoGetChallengeCmdBuild::createResponseParser(
        std::shared_ptr<ApduResponse> apduResponse)
{
    return std::make_shared<PoGetChallengeRespPars>(apduResponse, this);
}

bool PoGetChallengeCmdBuild::isSessionBufferUsed() const
{
    return false;
}

}
}
}
}
}
}
