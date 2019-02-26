#include "Logger.h"
#include "LoggerFactory.h"

namespace org {
    namespace eclipse {
        namespace keyple {
            namespace common {

                class LoggerFactory;

                /**
	             * Constructor
	             */
                Logger::Logger(const std::string &className) : className(className)
                {
                    //std::cout
                    //    << "[DEBUG] [class org::eclipse::keyple::common::Logger] logger constructor for "
                    //    << className << std::endl;

                    traceEnabled = 1;
                    debugEnabled = 1;
                    warnEnabled  = 1;
                    infoEnabled  = 1;
                }

                /**
                 * Destructor
                 */
                Logger::~Logger()
                {
                    //std::cout << "[DEBUG] [class org::eclipse::keyple::common::Logger] logger destructor of "
                    //          << className << std::endl;

                    /* Remove itself from factory list */
                    LoggerFactory::removeLogger(this);
                }

                bool Logger::isTraceEnabled()
                {
                    return traceEnabled;
                }

                bool Logger::isDebugEnabled()
                {
                    return debugEnabled;
                }

                bool Logger::isWarnEnabled()
                {
                    return warnEnabled;
                }

                bool Logger::isInfoEnabled()
                {
                    return infoEnabled;
                }

                std::string Logger::getClassName()
                {
                    return className;
                }

                void Logger::setTraceEnabled(bool enabled)
                {
                    traceEnabled = enabled;
                }

                void Logger::setDebugEnabled(bool enabled)
                {
                    debugEnabled = enabled;
                }

                void Logger::setWarnEnabled(bool enabled)
                {
                    warnEnabled = enabled;
                }

                void Logger::setInfoEnabled(bool enabled)
                {
                    infoEnabled = enabled;
                }

            } // namespace common
        }     // namespace keyple
    }         // namespace eclipse
} // namespace org