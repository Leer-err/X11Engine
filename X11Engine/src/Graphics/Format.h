#pragma once

#include <dxgiformat.h>

#include <cstdint>

enum class ImageFormat { BGR_24BPP, RGB_24BPP, BGRA_32BPP, RGBA_32BPP };

constexpr uint32_t imageFormatBytesPerPixel(ImageFormat format) {
    switch (format) {
        case ImageFormat::BGR_24BPP:
        case ImageFormat::RGB_24BPP:
            return 3;
        case ImageFormat::BGRA_32BPP:
        case ImageFormat::RGBA_32BPP:
            return 4;
    }
}