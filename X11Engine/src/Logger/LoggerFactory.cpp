#include "LoggerFactory.h"

#include <chrono>
#include <cstddef>
#include <vector>

#include "Logger.h"
#include "spdlog/cfg/env.h"
#include "spdlog/common.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

Logger LoggerFactory::getLogger(const char* name, LogLevel level) {
    constexpr size_t MAX_SIZE = 1 << 22;
    constexpr uint32_t MAX_FILE_COUNT = 5;

    spdlog::flush_every(std::chrono::seconds(1));

    static std::vector<spdlog::sink_ptr> sinks = {
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/log.txt", MAX_SIZE, MAX_FILE_COUNT, true)};

    return Logger(name, sinks);
}