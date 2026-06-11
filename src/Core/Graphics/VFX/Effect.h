#pragma once

#include <vector>

#include "Emitter.h"

namespace Graphics {

class Effect {
   public:
    void update();

   private:
    std::vector<Emitter> emitters;
};

}  // namespace Graphics