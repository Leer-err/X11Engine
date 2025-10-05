#ifndef LOGGER_FACTORY_H
#define LOGGER_FACTORY_H

#include "Logger.h"

class LoggerFactory {
   public:
    static Logger getLogger(const char* name, LogLevel level = LogLevel::Info);
};

#endif  // LOGGER_FACTORY_H