#include "ImageReaderFactory.h"

#include <filesystem>
#include <string>

#include "STBImageReader/STBImageReader.h"

std::shared_ptr<IImageReader> ImageReaderFactory::getReaderByFileExtension(
    const std::string filename) const {
    std::filesystem::path path(filename);

    std::string extension = path.extension().string();

    return std::make_shared<STBImageReader>();
}