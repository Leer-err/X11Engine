#pragma once

#include "IImageConverter.h"
#include "Image.h"

class RGBA8ImageConverter : IImageConverter {
   public:
    std::optional<Image> convert(const Image& image) override;

   private:
    Image convertBGRA8(const Image& image);
};