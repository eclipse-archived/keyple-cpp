#include "PoSelector.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/seproxy/protocol/SeProtocol.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/command/AbstractApduResponseParser.h"
#include "CalypsoPo.h"
#include "../command/po/parser/ReadRecordsRespPars.h"
#include "../command/po/builder/ReadRecordsCmdBuild.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/seproxy/protocol/ContactsProtocols.h"
#include "../command/po/parser/SelectFileRespPars.h"
#include "../command/po/builder/SelectFileCmdBuild.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/util/ByteArrayUtils.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/seproxy/message/ApduRequest.h"
#include "../command/po/PoCustomReadCommandBuilder.h"
#include "../command/po/PoCustomModificationCommandBuilder.h"
#include "../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/seproxy/message/SeResponse.h"

namespace org {
    namespace eclipse {
        namespace keyple {
            namespace calypso {
                namespace transaction {
                    using PoClass = org::eclipse::keyple::calypso::command::PoClass;
                    using PoCustomModificationCommandBuilder = org::eclipse::keyple::calypso::command::po::PoCustomModificationCommandBuilder;
                    using PoCustomReadCommandBuilder = org::eclipse::keyple::calypso::command::po::PoCustomReadCommandBuilder;
                    using ReadRecordsCmdBuild = org::eclipse::keyple::calypso::command::po::builder::ReadRecordsCmdBuild;
                    using SelectFileCmdBuild = org::eclipse::keyple::calypso::command::po::builder::SelectFileCmdBuild;
                    using ReadDataStructure = org::eclipse::keyple::calypso::command::po::parser::ReadDataStructure;
                    using ReadRecordsRespPars = org::eclipse::keyple::calypso::command::po::parser::ReadRecordsRespPars;
                    using SelectFileRespPars = org::eclipse::keyple::calypso::command::po::parser::SelectFileRespPars;
                    using AbstractApduResponseParser = org::eclipse::keyple::command::AbstractApduResponseParser;
                    using ChannelState = org::eclipse::keyple::seproxy::ChannelState;
                    using ApduRequest = org::eclipse::keyple::seproxy::message::ApduRequest;
                    using ApduResponse = org::eclipse::keyple::seproxy::message::ApduResponse;
                    using SeResponse = org::eclipse::keyple::seproxy::message::SeResponse;
                    using ContactsProtocols = org::eclipse::keyple::seproxy::protocol::ContactsProtocols;
                    using SeProtocol = org::eclipse::keyple::seproxy::protocol::SeProtocol;
                    using SeSelector = org::eclipse::keyple::transaction::SeSelector;
                    using ByteArrayUtils = org::eclipse::keyple::util::ByteArrayUtils;
                    using org::slf4j::Logger;
                    using org::slf4j::LoggerFactory;
const std::shared_ptr<org::slf4j::Logger> PoSelector::logger = org::slf4j::LoggerFactory::getLogger(PoSelector::typeid);

                    PoSelector::PoSelector(const std::string &atrRegex, ChannelState channelState, std::shared_ptr<SeProtocol> protocolFlag, const std::string &extraInfo) : org::eclipse::keyple::transaction::SeSelector(atrRegex, channelState, protocolFlag, extraInfo), poClass(PoClass::LEGACY), protocolFlag(protocolFlag) {
                        setMatchingClass(CalypsoPo::typeid);
                        setSelectorClass(PoSelector::typeid);
                        if (logger->isTraceEnabled()) {
                            logger->trace("Calypso {} selector", poClass);
                        }
                    }

                    PoSelector::PoSelector(std::vector<char> &aid, SelectMode selectMode, ChannelState channelState, std::shared_ptr<SeProtocol> protocolFlag, const std::string &extraInfo) : org::eclipse::keyple::transaction::SeSelector(aid, selectMode, channelState, protocolFlag, extraInfo), poClass(PoClass::ISO), protocolFlag(protocolFlag) {
                        setMatchingClass(CalypsoPo::typeid);
                        setSelectorClass(PoSelector::typeid);
                        if (logger->isTraceEnabled()) {
                            logger->trace("Calypso {} selector", poClass);
                        }
                    }

                    std::shared_ptr<ReadRecordsRespPars> PoSelector::prepareReadRecordsCmdInternal(char sfi, ReadDataStructure readDataStructureEnum, char firstRecordNumber, int expectedLength, const std::string &extraInfo) {

                        /*
                         * the readJustOneRecord flag is set to false only in case of multiple read records, in all
                         * other cases it is set to true
                         */
                        bool readJustOneRecord = !(readDataStructureEnum == ReadDataStructure::MULTIPLE_RECORD_DATA);

                        seSelectionApduRequestList.push_back((std::make_shared<ReadRecordsCmdBuild>(poClass, sfi, firstRecordNumber, readJustOneRecord, static_cast<char>(expectedLength), extraInfo))->getApduRequest());

                        if (logger->isTraceEnabled()) {
                            logger->trace("ReadRecords: SFI = {}, RECNUMBER = {}, JUSTONE = {}, EXPECTEDLENGTH = {}", sfi, firstRecordNumber, readJustOneRecord, expectedLength);
                        }

                        /* create a parser to be returned to the caller */
                        std::shared_ptr<ReadRecordsRespPars> poResponseParser = std::make_shared<ReadRecordsRespPars>(firstRecordNumber, readDataStructureEnum);

                        /*
                         * keep the parser in the CommandParser list
                         */
                        poResponseParserList.push_back(poResponseParser);

                        return poResponseParser;
                    }

                    std::shared_ptr<ReadRecordsRespPars> PoSelector::prepareReadRecordsCmd(char sfi, ReadDataStructure readDataStructureEnum, char firstRecordNumber, int expectedLength, const std::string &extraInfo) {
                        if (expectedLength < 1 || expectedLength > 250) {
                            throw std::invalid_argument("Bad length.");
                        }
                        return prepareReadRecordsCmdInternal(sfi, readDataStructureEnum, firstRecordNumber, expectedLength, extraInfo);
                    }

                    std::shared_ptr<ReadRecordsRespPars> PoSelector::prepareReadRecordsCmd(char sfi, ReadDataStructure readDataStructureEnum, char firstRecordNumber, const std::string &extraInfo) {
                        if (protocolFlag == ContactsProtocols::PROTOCOL_ISO7816_3) {
                            throw std::invalid_argument("In contacts mode, the expected length must be specified.");
                        }
                        return prepareReadRecordsCmdInternal(sfi, readDataStructureEnum, firstRecordNumber, 0, extraInfo);
                    }

                    std::shared_ptr<SelectFileRespPars> PoSelector::prepareSelectFileDfCmd(std::vector<char> &path, const std::string &extraInfo) {
                        seSelectionApduRequestList.push_back((std::make_shared<SelectFileCmdBuild>(poClass, SelectFileCmdBuild::SelectControl::PATH_FROM_MF, SelectFileCmdBuild::SelectOptions::FCI, path))->getApduRequest());
                        if (logger->isTraceEnabled()) {
                            logger->trace("Select File: PATH = {}", ByteArrayUtils::toHex(path));
                        }

                        /* create a parser to be returned to the caller */
                        std::shared_ptr<SelectFileRespPars> poResponseParser = std::make_shared<SelectFileRespPars>();

                        /*
                         * keep the parser in a CommandParser list with the number of apduRequest associated with it
                         */
                        poResponseParserList.push_back(poResponseParser);

                        return poResponseParser;
                    }

                    void PoSelector::preparePoCustomReadCmd(const std::string &name, std::shared_ptr<ApduRequest> apduRequest) {
                        seSelectionApduRequestList.push_back((std::make_shared<PoCustomReadCommandBuilder>(name, apduRequest))->getApduRequest());
                        if (logger->isTraceEnabled()) {
                            logger->trace("CustomReadCommand: APDUREQUEST = {}", apduRequest);
                        }
                    }

                    void PoSelector::preparePoCustomModificationCmd(const std::string &name, std::shared_ptr<ApduRequest> apduRequest) {
                        seSelectionApduRequestList.push_back((std::make_shared<PoCustomModificationCommandBuilder>(name, apduRequest))->getApduRequest());
                        if (logger->isTraceEnabled()) {
                            logger->trace("CustomModificationCommand: APDUREQUEST = {}", apduRequest);
                        }
                    }

                    void PoSelector::updateParsersWithResponses(std::shared_ptr<SeResponse> seResponse) {
                        /* attempt to update the parsers only if the list is not empty! */
                        if (poResponseParserList.size() != 0) {
                            std::vector<std::shared_ptr<AbstractApduResponseParser>>::const_iterator parserIterator = poResponseParserList.begin();
                            /* double loop to set apdu responses to corresponding parsers */
                            for (auto apduResponse : seResponse->getApduResponses()) {
//JAVA TO C++ CONVERTER TODO TASK: Java iterators are only converted within the context of 'while' and 'for' loops:
                                if (!parserIterator.hasNext()) {
                                    throw std::make_shared<IllegalStateException>("Parsers list and responses list mismatch! ");
                                }
//JAVA TO C++ CONVERTER TODO TASK: Java iterators are only converted within the context of 'while' and 'for' loops:
                                parserIterator.next().setApduResponse(apduResponse);
                                if (!apduResponse->isSuccessful()) {
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
