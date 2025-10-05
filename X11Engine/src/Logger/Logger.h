#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/logger.h>

#include <memory>
#include <vector>

#include "spdlog/common.h"

enum class LogLevel { Debug, Info, Warn, Error };

class Logger {
   public:
    Logger(const char* name, std::vector<spdlog::sink_ptr> sinks);
    ~Logger();

    void setLevel(LogLevel level);

    template <typename... ARGS>
    void info(std::format_string<ARGS...> line, ARGS&&... args) {
        logger->info(std::format(line, std::forward<ARGS>(args)...));
    }

    template <typename... ARGS>
    void warning(std::format_string<ARGS...> line, ARGS&&... args) {
        logger->warn(std::format(line, std::forward<ARGS>(args)...));
    }

    template <typename... ARGS>
    void error(std::format_string<ARGS...> line, ARGS&&... args) {
        logger->error(std::format(line, std::forward<ARGS>(args)...));
    }

    template <typename... ARGS>
    void debug(std::format_string<ARGS...> line, ARGS&&... args) {
        logger->debug(std::format(line, std::forward<ARGS>(args)...));
    }

   private:
    std::shared_ptr<spdlog::logger> logger;
};

#endif  // LOGGER_H