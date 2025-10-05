#pragma once

#include "Format.h"

class ITexture {
   public:
    virtual ImageFormat getFormat() const = 0;
};