#include "WICImageReader.h"

#include <Windows.h>
#include <intsafe.h>
#include <wincodec.h>
#include <winnt.h>

#include <optional>
#include <string>
#include <vector>

#include "Format.h"
#include "Logger.h"
#include "LoggerFactory.h"

WICImageReader::WICImageReader()
    : logger(LoggerFactory::getLogger("WICImageReader")) {
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));

    is_created = SUCCEEDED(hr);

    if (is_created) {
        logger.info("Successfully initialized");
    } else {
        logger.error("Failed to initialize");
    }
}

std::optional<ImageFormat> WICImageReader::convertFormat(
    WICPixelFormatGUID format) {
    if (format == GUID_WICPixelFormat24bppBGR) return ImageFormat::BGR_24BPP;
    if (format == GUID_WICPixelFormat24bppRGB) return ImageFormat::RGB_24BPP;
    if (format == GUID_WICPixelFormat32bppBGRA) return ImageFormat::BGRA_32BPP;

    return std::nullopt;
}

std::optional<Image> WICImageReader::read(std::string filename) {
    if (!is_created) return std::nullopt;

    logger.debug("Reading {}", filename);

    std::wstring wide_filename(filename.begin(), filename.end());

    IWICBitmapDecoder* decoder = NULL;
    HRESULT hr = factory->CreateDecoderFromFilename(
        wide_filename.c_str(), NULL, GENERIC_READ,
        WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) {
        logger.error("Failed to create decoder for {}", filename);
        return std::nullopt;
    }

    IWICBitmapFrameDecode* frame = NULL;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) {
        logger.error("Failed to get frame from {}", filename);
        return std::nullopt;
    }

    WICPixelFormatGUID wicFormat;
    frame->GetPixelFormat(&wicFormat);
    auto format_optional = convertFormat(wicFormat);
    if (format_optional == std::nullopt) {
        logger.error("File {} has unsupported format", filename);
        return std::nullopt;
    }

    ImageFormat format = format_optional.value();

    UINT width, height;
    hr = frame->GetSize(&width, &height);
    if (FAILED(hr)) {
        logger.error("Failed to get size for {}", filename);
        return std::nullopt;
    }

    std::vector<uint8_t> frame_data;
    auto bytes_per_pixel = imageFormatBytesPerPixel(format);

    UINT stride = height * bytes_per_pixel;
    UINT size = width * height * bytes_per_pixel;

    frame_data.resize(size);
    hr = frame->CopyPixels(NULL, stride, size, frame_data.data());
    if (FAILED(hr)) {
        logger.error("Failed to read pixels from {}", filename);
        return std::nullopt;
    }

    logger.debug("Successfully read {} with size {}x{}", filename, width,
                 height);

    return Image(frame_data, width, height, format);
}