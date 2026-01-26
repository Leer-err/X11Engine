#pragma once

#include <string>

#include "Image.h"

class IImageReader {
   public:
    virtual Image readFromMemory(const char* data, uint32_t size) = 0;
    virtual Image readFromFile(const std::string& filepath) = 0;
};