#pragma once

class ImageRegistry {
   public:
   private:
    ImageHandle next_handle;
    std::vector<Image> images;
};