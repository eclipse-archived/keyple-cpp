/* Core */
#include "AbstractThreadedObservablePlugin.h"
#include "PluginEvent.h"
#include "AbstractObservableReader.h"
#include "KeypleReaderException.h"
#include "InterruptedException.h"

namespace org {
    namespace eclipse {
        namespace keyple {
            namespace seproxy {
                namespace plugin {

                    using ObservablePlugin = org::eclipse::keyple::seproxy::event::ObservablePlugin;
                    using PluginEvent      = org::eclipse::keyple::seproxy::event::PluginEvent;
                    using KeypleReaderException =
                        org::eclipse::keyple::seproxy::exception::KeypleReaderException;

                    std::set<std::string> _set;
                    std::shared_ptr<std::set<std::string>> nativeReadersNames =
                        std::make_shared<std::set<std::string>>(_set);

                    AbstractThreadedObservablePlugin::AbstractThreadedObservablePlugin(
                        const std::string &name)
                        : AbstractObservablePlugin(name)
                    {
                        logger->debug("constructor (name: %s)\n", name);
                    }

                    AbstractThreadedObservablePlugin::~AbstractThreadedObservablePlugin()
                    {
                        logger->debug("destructor (name: %s)\n", name);
                    }

                    void AbstractThreadedObservablePlugin::startObservation() {
                        logger->debug("\n");
                        thread = std::make_shared<AbstractThreadedObservablePlugin::EventThread>(shared_from_this(), this->getName());
                        thread->start();
                    }

                    void AbstractThreadedObservablePlugin::stopObservation() {
                        logger->debug("\n");
                        if (thread != nullptr) {
                            thread->end();
                        }
                    }

                    AbstractThreadedObservablePlugin::EventThread::EventThread(
                        std::shared_ptr<AbstractThreadedObservablePlugin> outerInstance,
                        const std::string &pluginName)
                        : pluginName(pluginName), outerInstance(outerInstance)
                    {
                        outerInstance->logger->debug("constructor with outerInstance: %p, pluginName: %s\n",
                                                     outerInstance, pluginName);
                    }

                    void AbstractThreadedObservablePlugin::EventThread::end()
                    {
                        outerInstance->logger->debug("\n");

                        running = false;
                        this->interrupt();
                    }

                    void *AbstractThreadedObservablePlugin::EventThread::run()
                    {
                        std::shared_ptr<std::set<std::string>> changedReaderNames = std::make_shared<std::set<std::string>>();
                        try
                        {
                            while (running)
                            {
                                /* retrieves the current readers names list */
                                std::shared_ptr<std::set<std::string>> actualNativeReadersNames =
                                    outerInstance->fetchNativeReadersNames();

                                /* Checks if it has changed this algorithm favors cases where nothing change */
                                if (outerInstance->nativeReadersNames != actualNativeReadersNames)
                                {
                                    /*
                                     * parse the current readers list, notify for disappeared readers, update
                                     * readers list
                                     */
                                    /* build changed reader names list */
                                    changedReaderNames->clear();
                                    for (auto _it : (*outerInstance->readers)) {
                                            std::shared_ptr<AbstractObservableReader> it = std::dynamic_pointer_cast<AbstractObservableReader>(_it);
                                            if (actualNativeReadersNames->find(it->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName()) == actualNativeReadersNames->end()) {
                                            changedReaderNames->insert(it->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName());
                                        }
                                    }
                                    /* notify disconnections if any and update the reader list */
                                    if (changedReaderNames->size() > 0) {
                                        /* grouped notification */
                                        outerInstance->ObservablePlugin::notifyObservers(std::make_shared<PluginEvent>(this->pluginName, changedReaderNames, PluginEvent::EventType::READER_DISCONNECTED));
                                        /* list update */
                                        for (auto _it : (*outerInstance->readers)) {
                                            std::shared_ptr<AbstractObservableReader> it = std::dynamic_pointer_cast<AbstractObservableReader>(_it);
                                            if (actualNativeReadersNames->find(it->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName()) != actualNativeReadersNames->end()) {
                                                outerInstance->readers->erase(_it);
                                                outerInstance->logger->trace("[%s][%s] Plugin thread => Remove unplugged reader from readers list.", this->pluginName,
                                                              it->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName());
                                                /* remove reader name from the current list */
                                                outerInstance->nativeReadersNames->erase(it->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName());
                                        }
                                    }
                                        /* clean the list for a possible connection notification */
                                        changedReaderNames->clear();
                                    }
                                    /*
                                     * parse the new readers list, notify for readers appearance, update readers
                                     * list
                                     */
                                    for (auto readerName : *actualNativeReadersNames) {
                                        if (outerInstance->nativeReadersNames->find(readerName) != outerInstance->nativeReadersNames->end()) {
                                            std::shared_ptr<AbstractObservableReader> reader = outerInstance->fetchNativeReader(readerName);
                                            outerInstance->readers->insert(reader);
                                            /* add to the notification list */
                                            changedReaderNames->insert(readerName);
                                            outerInstance->logger->trace("[%s][%s] Plugin thread => Add plugged reader to readers list.", this->pluginName,
                                                                         reader->AbstractLoggedObservable<std::shared_ptr<ReaderEvent>>::getName());
                                            /* add reader name to the current list */
                                            outerInstance->nativeReadersNames->insert(readerName);
                                        }
                                    }
                                    /* notify connections if any */
                                    if (changedReaderNames->size() > 0) {
                                        outerInstance->ObservablePlugin::notifyObservers(std::make_shared<PluginEvent>(this->pluginName, changedReaderNames, PluginEvent::EventType::READER_CONNECTED));
                                    }
                                }
                                /* sleep for a while. */
                                Thread::sleep(outerInstance->threadWaitTimeout);
                            }
                        }
                        catch (const InterruptedException &e) {
                            e.printStackTrace();
                            outerInstance->logger->warn("[%s] An exception occurred while monitoring plugin: %s, cause %s", this->pluginName, e.getMessage(), e.getCause());
                        }
                        catch (const KeypleReaderException &e) {
                            e.printStackTrace();
                            outerInstance->logger->warn("[%s] An exception occurred while monitoring plugin: %s, cause %s", this->pluginName, e.what(), e.getCause());
                        }

                        return nullptr;
                    }

                    void AbstractThreadedObservablePlugin::finalize() throw(std::runtime_error) {
                        thread->end();
                        thread.reset();
                        logger->trace("[%s] observable Plugin thread ended.", this->getName());
                        //AbstractObservablePlugin::finalize();
                    }

                } // namespace plugin
            }     // namespace seproxy
        }         // namespace keyple
    }             // namespace eclipse
} // namespace org
