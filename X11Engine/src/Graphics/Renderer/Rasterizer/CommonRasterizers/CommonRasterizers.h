#pragma once

#include "Rasterizer.h"

namespace Engine::Graphics {

class CommonRasterizers {
   public:
    static Rasterizer fill();
    static Rasterizer wireframe();

   private:
    CommonRasterizers();

    static CommonRasterizers& get();

    Rasterizer fill_rasterizer;
    Rasterizer wireframe_rasterizer;
};

};  // namespace Engine::Graphics