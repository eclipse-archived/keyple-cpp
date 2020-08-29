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

#include "CalypsoSamCommandException.h"

namespace keyple {
namespace calypso {
namespace command {
namespace sam {
namespace builder {
namespace exception {

using namespace keyple::calypso::command::sam;

/**
 * The exception {@code CalypsoSamIllegalParameterException} indicates that some
 * input parameter is not accepted by the SAM.
 */
class CalypsoSamIllegalParameterException final
: public CalypsoSamCommandException {
public:
    /**
     * @param message the message to identify the exception context
     * @param command the Calypso SAM command
     * @param statusCode the status code
     */
    CalypsoSamIllegalParameterException(
      const std::string& message,
      const std::shared_ptr<CalypsoSamCommand> command,
      const std::shared_ptr<int> statusCode)
    : CalypsoSamCommandException(message, command, statusCode) {}
    }
};

}
}
}
}
}
}
