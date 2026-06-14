#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Graphics {

struct InputLayout {
    std::vector<size_t> push_constants;
};

}  // namespace Graphics
