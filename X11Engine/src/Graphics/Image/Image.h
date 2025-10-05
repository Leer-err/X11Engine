#pragma once

#include <cstdint>
#include <vector>

#include "Format.h"

class Image {
   public:
    Image(const char* data, uint32_t width, uint32_t height,
          ImageFormat format);

    Image(std::vector<uint8_t> data, uint32_t width, uint32_t height,
          ImageFormat format);

    const void* getData() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    ImageFormat getFormat() const;

   private:
    std::vector<uint8_t> data;

    ImageFormat format;

    uint32_t width;
    uint32_t height;
};