#pragma once

#include <memory>
#include <vector>

#include "IScript.h"

struct Scripts {
    std::vector<std::unique_ptr<IScript>> scripts;
};