#pragma once

#include "Shape.h"

namespace Physics {

class Actor {
   public:
    virtual void addShape(const Shape& shape) = 0;
};

};  // namespace Physics