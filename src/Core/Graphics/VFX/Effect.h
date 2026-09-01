#pragma once

#include <vector>

#include "Emitter.h"

namespace Graphics {

struct Effect {
    std::vector<Emitter> emitters;
};

}  // namespace Graphics