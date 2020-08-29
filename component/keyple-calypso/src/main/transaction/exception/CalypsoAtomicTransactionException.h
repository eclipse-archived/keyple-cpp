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

#include "CalypsoPoTransactionException.h"

namespace keyple {
namespace calypso {
namespace transaction {
namespace exception {

/**
 * The exception {@code CalypsoAtomicTransactionException} indicates that the
 * transaction cannot be done in an atomic way because the capacity of the
 * session buffer is not sufficient.
 */
class CalypsoAtomicTransactionException final
: public CalypsoPoTransactionException {
public:
    /**
     * @param message the message to identify the exception context
     */
    CalypsoAtomicTransactionException(const std::string& message)
    : CalypsoPoTransactionException(message) {}
};

}
}
}
}
