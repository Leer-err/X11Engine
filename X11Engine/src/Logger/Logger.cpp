#include "Logger.h"

#include <spdlog/spdlog.h>

#include <memory>

#include "spdlog/common.h"
#include "spdlog/logger.h"

Logger::Logger(const char* name, std::vector<spdlog::sink_ptr> sinks)
    : logger(spdlog::get(name)) {
    if (logger != nullptr) return;

    logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    spdlog::register_logger(logger);
}

Logger::~Logger() { logger->flush(); }

void Logger::setLevel(LogLevel level) {
    spdlog::level::level_enum spdlog_level;

    switch (level) {
        case LogLevel::Debug:
            spdlog_level = spdlog::level::debug;
            break;
        case LogLevel::Info:
            spdlog_level = spdlog::level::info;
            break;
        case LogLevel::Warn:
            spdlog_level = spdlog::level::warn;
            break;
        case LogLevel::Error:
            spdlog_level = spdlog::level::err;
            break;
    }

    logger->set_level(spdlog_level);
}
