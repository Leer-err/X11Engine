#ifndef STB_IMAGE_READER
#define STB_IMAGE_READER

#include "IImageReader.h"

class STBImageReader : public IImageReader {
   public:
    STBImageReader() = default;

    Image readFromMemory(const char* data, uint32_t size) override;

   private:
};

#endif  // STB_IMAGE_READER