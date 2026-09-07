#pragma once

#include <filesystem>

#include "AppConfig.h"
#include "FileError.h"
#include "Result.h"

namespace File {

Result<Config::AppConfig, Error> readConfig(const std::filesystem::path& path);

}