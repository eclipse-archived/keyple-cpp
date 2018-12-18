#pragma once

#include <string>
#include <unordered_map>
#include "exceptionhelper.h"
#include <memory>

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace org { namespace eclipse { namespace keyple { namespace seproxy { namespace message { class ApduResponse; } } } } }
namespace org { namespace eclipse { namespace keyple { namespace command { class StatusProperties; } } } }

/********************************************************************************
 * Copyright (c) 2018 Calypso Networks Association https://www.calypsonet-asso.org/
 *
 * See the NOTICE file(s) distributed with this work for additional information regarding copyright
 * ownership.
 *
 * This program and the accompanying materials are made available under the terms of the Eclipse
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
namespace org {
    namespace eclipse {
        namespace keyple {
            namespace command {

                using ApduResponse = org::eclipse::keyple::seproxy::message::ApduResponse;

                /**
                 * Base class for parsing APDU
                 */
                class AbstractApduResponseParser : public std::enable_shared_from_this<AbstractApduResponseParser> {

                    /** the byte array APDU response. */
                protected:
                    std::shared_ptr<ApduResponse> response;

                    static const std::unordered_map<Integer, std::shared_ptr<StatusProperties>> STATUS_TABLE;
                                private:
                                    class StaticConstructor : public std::enable_shared_from_this<StaticConstructor> {
                                    public:
                                        StaticConstructor();
                                    };

                                private:
                                    static AbstractApduResponseParser::StaticConstructor staticConstructor;


                    /** Indicates whether the ApduResponse has been provided or not */
                    bool initialized = false;

                    // Note: The conversion of all commands was done with:
                    // Input regex: new byte\[\] \{\(byte\) 0x([0-9A-Za-z]{2})\, \(byte\) 0x([0-9A-Za-z]{2})\}
                    // Output regex: 0x$1$2

                    /**
                     * Get the internal status table
                     * 
                     * @return Status table
                     */
                protected:
                    virtual std::unordered_map<Integer, std::shared_ptr<StatusProperties>> getStatusTable();

                    /**
                     * the generic abstract constructor to build a parser of the APDU response.
                     *
                     * @param response response to parse
                     */
                public:
//JAVA TO C++ CONVERTER TODO TASK: Most Java annotations will not have direct C++ equivalents:
//ORIGINAL LINE: @Deprecated public AbstractApduResponseParser(org.eclipse.keyple.seproxy.message.ApduResponse response)
                    AbstractApduResponseParser(std::shared_ptr<ApduResponse> response);

                    /**
                     * Default constructor
                     */
                    AbstractApduResponseParser();

                    /**
                     * Sets the Apdu response to parse
                     * 
                     * @param response the apdu response
                     */
                    void setApduResponse(std::shared_ptr<ApduResponse> response);

                    bool isInitialized();

                    /**
                     * Gets the apdu response.
                     *
                     * @return the ApduResponse instance.
                     */
                    std::shared_ptr<ApduResponse> getApduResponse();

                private:
                    int getStatusCode();

                    std::shared_ptr<StatusProperties> getPropertiesForStatusCode();

                    /**
                     * Checks if is successful.
                     *
                     * @return if the status is successful from the statusTable according to the current status
                     *         code.
                     */
                public:
                    virtual bool isSuccessful();

                    /**
                     * Gets the status information.
                     *
                     * @return the ASCII message from the statusTable for the current status code.
                     */
                    std::string getStatusInformation();


                    /**
                     * Status code properties
                     */
                protected:
                    class StatusProperties : public std::enable_shared_from_this<StatusProperties> {

                        /** The successful. */
                    private:
                        const bool successful;

                        /** The information. */
                        const std::string information;

                        /**
                         * A map with the double byte of a status as key, and the successful property and ASCII text
                         * information as data.
                         *
                         * @param successful set successful status
                         * @param information additional information
                         */
                    public:
                        StatusProperties(bool successful, const std::string &information);

                        /**
                         * Gets the successful.
                         *
                         * @return the successful
                         */
                        virtual bool isSuccessful();

                        /**
                         * Gets the information.
                         *
                         * @return the information
                         */
                        virtual std::string getInformation();

                    };
                };

            }
        }
    }
}
