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

#pragma once

#include <map>
#include <vector>
#include <memory>

/* Calypso */
#include "AbstractSamResponseParser.h"
#include "CardCipherPinCmdBuild.h"
#include "KeypleCalypsoExport.h"

namespace keyple {
namespace calypso {
namespace command {
namespace sam {
namespace parser {
namespace security {

using namespace keyple::calypso::command::sam;
using namespace keyple::calypso::command::sam::builder::security;
using namespace keyple::core::seproxy::message;

using StatusProperties = AbstractApduResponseParser::StatusProperties;

class KEYPLECALYPSO_API CardCipherPinRespPars : public AbstractSamResponseParser {
public:
    /**
     * Instantiates a new CardCipherPinRespPars.
     *
     * @param response from the SAM
     * @param builder the reference to the builder that created this parser
     */
    CardCipherPinRespPars(const std::shared_ptr<ApduResponse> response,
                          CardCipherPinCmdBuild* builder);

    /**
     * Gets the 8 bytes of ciphered data.
     *
     * @return the ciphered data byte array
     */
    const std::vector<uint8_t> getCipheredData() const;

protected:
    /**
     *
     */
    const std::map<int, std::shared_ptr<StatusProperties>>&
        getStatusTable() const override;

private:
    /**
     *
     */
    static const std::map<int, std::shared_ptr<StatusProperties>> STATUS_TABLE;
};

}
}
}
}
}
}
