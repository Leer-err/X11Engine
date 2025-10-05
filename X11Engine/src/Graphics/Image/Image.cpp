#include "Image.h"

#include <cstring>
#include <vector>

#include "Format.h"

Image::Image(const char* data_ptr, uint32_t width, uint32_t height,
             ImageFormat format)
    : width(width), height(height), format(format) {
    auto bytes_per_pixel = imageFormatBytesPerPixel(format);
    auto image_size = height * width * bytes_per_pixel;

    data = std::vector<uint8_t>(image_size);
    memcpy(data.data(), data_ptr, image_size);
}

Image::Image(std::vector<uint8_t> data, uint32_t width, uint32_t height,
             ImageFormat format)
    : data(data), width(width), height(height), format(format) {}

const void* Image::getData() const { return data.data(); }

uint32_t Image::getWidth() const { return width; }

uint32_t Image::getHeight() const { return height; }

ImageFormat Image::getFormat() const { return format; }