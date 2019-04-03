#include "VirtualReaderBaseTest.h"
#include "../../../../../../../../main/java/org/eclipse/keyple/plugin/remotese/transport/factory/TransportFactory.h"
#include "../../../../../../../../main/java/org/eclipse/keyple/plugin/remotese/nativese/NativeReaderServiceImpl.h"
#include "../../../../../../../../../../stub/src/main/java/org/eclipse/keyple/plugin/stub/StubReader.h"
#include "../../../../../../../../main/java/org/eclipse/keyple/plugin/remotese/pluginse/VirtualReader.h"
#include "../../../../../../../../main/java/org/eclipse/keyple/plugin/remotese/pluginse/VirtualReaderService.h"
#include "../../../../../../../../../../stub/src/main/java/org/eclipse/keyple/plugin/stub/StubPlugin.h"
#include "../../../../../../../../main/java/org/eclipse/keyple/plugin/remotese/transport/impl/java/LocalTransportFactory.h"
#include "Integration.h"
#include "../../../../../../../../../../../keyple-core/src/main/java/org/eclipse/keyple/seproxy/protocol/SeProtocolSetting.h"
#include "../../../../../../../../../../stub/src/main/java/org/eclipse/keyple/plugin/stub/StubProtocolSetting.h"

namespace org {
    namespace eclipse {
        namespace keyple {
            namespace plugin {
                namespace remotese {
                    namespace integration {
                        using NativeReaderServiceImpl = org::eclipse::keyple::plugin::remotese::nativese::NativeReaderServiceImpl;
                        using VirtualReader = org::eclipse::keyple::plugin::remotese::pluginse::VirtualReader;
                        using VirtualReaderService = org::eclipse::keyple::plugin::remotese::pluginse::VirtualReaderService;
                        using TransportFactory = org::eclipse::keyple::plugin::remotese::transport::factory::TransportFactory;
                        using LocalTransportFactory = org::eclipse::keyple::plugin::remotese::transport::impl::java::LocalTransportFactory;
                        using StubPlugin = org::eclipse::keyple::plugin::stub::StubPlugin;
                        using StubProtocolSetting = org::eclipse::keyple::plugin::stub::StubProtocolSetting;
                        using StubReader = org::eclipse::keyple::plugin::stub::StubReader;
                        using SeProtocolSetting = org::eclipse::keyple::seproxy::protocol::SeProtocolSetting;
                        using namespace org::junit;
                        using org::junit::rules::TestName;
                        using org::slf4j::Logger;
                        using org::slf4j::LoggerFactory;
const std::shared_ptr<org::slf4j::Logger> VirtualReaderBaseTest::logger = org::slf4j::LoggerFactory::getLogger(VirtualReaderBaseTest::typeid);

                        void VirtualReaderBaseTest::initKeypleServices() throw(std::runtime_error) {
                            logger->info("------------------------------");
                            logger->info("Test {}", name->getMethodName());
                            logger->info("------------------------------");

                            // assert that there is no stub readers plugged already
                            Assert::assertEquals(0, StubPlugin::getInstance()->getReaders()->size());

                            logger->info("*** Init LocalTransportFactory");
                            // use a local transport factory for testing purposes (only java calls between client and
                            // server). Only one client and one server bound together.
                            factory = std::make_shared<LocalTransportFactory>();

                            logger->info("*** Bind Master Services");
                            // bind Master services to server
                            virtualReaderService = Integration::bindMaster(factory->getServer());

                            logger->info("*** Bind Slave Services");
                            // bind Slave services to client
                            nativeReaderService = Integration::bindSlave(factory->getClient());



                        }

                        void VirtualReaderBaseTest::clearStubpluginReaders() throw(std::runtime_error) {

                            logger->info("Cleaning of the stub plugin");

                            std::shared_ptr<StubPlugin> stubPlugin = StubPlugin::getInstance();

                            // if nativeReader was initialized during test, unplug it
                            if (nativeReader != nullptr) {
                                stubPlugin->unplugStubReader(nativeReader->getName(), true);
                                nativeReader->clearObservers();
                            }



                            // stubPlugin.removeObserver(stubPluginObserver);

                            // Thread.sleep(500);

                            logger->info("End of cleaning of the stub plugin");
                        }

                        std::shared_ptr<StubReader> VirtualReaderBaseTest::connectStubReader(const std::string &readerName, const std::string &nodeId) throw(std::runtime_error) {
                            // configure native reader
                            std::shared_ptr<StubReader> nativeReader = std::static_pointer_cast<StubReader>(Integration::createStubReader(readerName));
                            nativeReader->addSeProtocolSetting(std::make_shared<SeProtocolSetting>(StubProtocolSetting::SETTING_PROTOCOL_ISO14443_4));
                            this->nativeReaderService->connectReader(nativeReader, nodeId);
                            return nativeReader;
                        }

                        void VirtualReaderBaseTest::disconnectStubReader(const std::string &sessionId, const std::string &nativeReaderName, const std::string &nodeId) throw(std::runtime_error) {
                            this->nativeReaderService->disconnectReader(sessionId, nativeReaderName, nodeId);
                        }

                        std::shared_ptr<VirtualReader> VirtualReaderBaseTest::getVirtualReader() throw(std::runtime_error) {
                            Assert::assertEquals(1, this->virtualReaderService->getPlugin()->getReaders()->size());
                            return std::static_pointer_cast<VirtualReader>(this->virtualReaderService->getPlugin()->getReaders()->first());
                        }
                    }
                }
            }
        }
    }
}
