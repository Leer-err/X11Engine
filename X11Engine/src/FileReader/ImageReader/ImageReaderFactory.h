#ifndef I_IMAGE_READER_FACTORY
#define I_IMAGE_READER_FACTORY

#include <memory>
#include <string>

#include "IImageReader.h"

class ImageReaderFactory {
   public:
    static ImageReaderFactory& get() {
        static ImageReaderFactory instance;
        return instance;
    }

    std::shared_ptr<IImageReader> getReaderByFileExtension(
        const std::string filename) const;

   private:
    ImageReaderFactory() = default;
    ~ImageReaderFactory() = default;

    ImageReaderFactory(const ImageReaderFactory&) = delete;
    ImageReaderFactory& operator=(const ImageReaderFactory&) = delete;

    ImageReaderFactory(ImageReaderFactory&&) = delete;
    ImageReaderFactory& operator=(ImageReaderFactory&&) = delete;
};

#endif  // I_IMAGE_READER_FACTORY