#include "STBImageReader.h"

#include "Format.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image STBImageReader::readFromMemory(const char* data, uint32_t size) {
    int width;
    int height;
    int num_channels;

    char* loaded = (char*)stbi_load_from_memory(
        (const uint8_t*)data, size, &width, &height, &num_channels, 4);

    return Image(loaded, width, height, ImageFormat::RGBA_32BPP);
}