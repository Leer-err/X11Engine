#pragma once

#include <wincodec.h>
#include <wrl/client.h>

#include <cstddef>
#include <optional>
#include <string>

#include "IImageReader.h"
#include "Image.h"
#include "Logger.h"

class WICImageReader : public IImageReader {
   public:
    WICImageReader();

    std::optional<Image> read(std::string filename);

   private:
    std::optional<ImageFormat> convertFormat(WICPixelFormatGUID format);

    Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
    bool is_created;

    Logger logger;
};