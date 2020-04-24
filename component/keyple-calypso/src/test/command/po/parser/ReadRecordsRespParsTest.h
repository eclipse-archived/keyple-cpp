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

#pragma once

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace keyple {
namespace calypso {
namespace command {
namespace po {
namespace parser {

class ReadRecordsRespParsTest
: public std::enable_shared_from_this<ReadRecordsRespParsTest> {
public:
    virtual void readRecordRespPars_one_record();
    virtual void readRecordRespPars_records();
    virtual void sampleMultipleRecordsParsing();
    virtual void readRecordRespPars_one_record_sfi();
    virtual void readRecordRespPars_records_sfi();
};

}
}
}
}
}
