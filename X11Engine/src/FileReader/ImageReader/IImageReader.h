#pragma once

#include "Image.h"

class IImageReader {
   public:
    virtual Image readFromMemory(const char* data, uint32_t size) = 0;
};