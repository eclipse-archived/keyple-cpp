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

#pragma once

#include <memory>

/* Calypso */
#include "CalypsoSam.h"
#include "SamSelector.h"

/* Core */
#include "AbstractApduCommandBuilder.h"
#include "AbstractSeSelectionRequest.h"
#include "KeypleCalypsoExport.h"
#include "SeProtocol.h"
#include "SeResponse.h"

namespace keyple {
namespace calypso {
namespace transaction {

using namespace keyple::calypso::command::sam;
using namespace keyple::core::command;
using namespace keyple::core::selection;
using namespace keyple::core::seproxy;
using namespace keyple::core::seproxy::protocol;
using namespace keyple::core::seproxy::message;

/**
 * Specialized selection request to manage the specific characteristics of
 * Calypso SAMs
 */
class KEYPLECALYPSO_API SamSelectionRequest
: public AbstractSeSelectionRequest<AbstractApduCommandBuilder> {
public:
    /**
     * Create a {@link SamSelectionRequest}
     *
     * @param samSelector the SAM selector
     * @param channelState the action to take after (close or keep open)
     */
    SamSelectionRequest(std::shared_ptr<SamSelector> samSelector);

protected:
    /**
     * Create a CalypsoSam object containing the selection data received from
     * the plugin
     *
     * @param seResponse the SE response received
     * @return a {@link CalypsoSam}
     */
    const std::shared_ptr<CalypsoSam> parse(
        std::shared_ptr<SeResponse> seResponse) override;
};

}
}
}
