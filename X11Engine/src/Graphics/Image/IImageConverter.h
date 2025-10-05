#pragma once

#include <optional>

#include "Image.h"

class IImageConverter {
   public:
    virtual std::optional<Image> convert(const Image& image) = 0;
};