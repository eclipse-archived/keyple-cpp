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

#include <string>
#include <vector>
#include <memory>

#include "StubSecureElement.h"

namespace keyple {
namespace example {
namespace commono {
namespace calypso {
namespace stub {

using namespace keyple::plugin::stub;

/**
 * Simple contact stub SE (no command)
 */
class StubMemoryST25 : public StubSecureElement {
public:
    /**
     *
     */
    static const std::string seProtocol;

    /**
     *
     */
    const std::string ATR_HEX = "3B8F8001804F0CA000000306070007D0020C00B6";

    /**
     *
     */
    StubMemoryST25();

    /**
     *
     */
    const std::vector<uint8_t>& getATR() override;

    /**
     *
     */
    std::string getSeProcotol() override;
};

}
}
}
}
}
