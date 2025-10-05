#include "RGBA8ImageConverter.h"

#include "Format.h"


std::optional<Image> RGBA8ImageConverter::convert(const Image& image) {
    auto format = image.getFormat();

    switch (format) { case ImageFormat::BGRA_32BPP: }
}