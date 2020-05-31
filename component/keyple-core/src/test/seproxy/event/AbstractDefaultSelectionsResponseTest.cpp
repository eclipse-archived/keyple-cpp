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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "AbstractDefaultSelectionsResponse.h"

using namespace testing;

using namespace keyple::core::seproxy::event;

class AbstractDefaultSelectionsResponseMock :
public AbstractDefaultSelectionsResponse {
protected:
    const std::list<std::shared_ptr<SeResponse>>&
        getSelectionSeResponseSet() const override
    {
        return responses;
    }

private:
    std::list<std::shared_ptr<SeResponse>> responses;
};

TEST(AbstractDefaultSelectionsResponseTest, AbstractDefaultSelectionsResponse)
{
    AbstractDefaultSelectionsResponseMock response;
}
