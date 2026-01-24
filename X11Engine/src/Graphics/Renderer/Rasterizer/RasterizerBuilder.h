#pragma once

#include "Rasterizer.h"

namespace Engine::Graphics {

enum class Fill { Solid, Wireframe };
enum class Cull { Front, Back, None };

class RasterizerBuilder {
   public:
    RasterizerBuilder();

    RasterizerBuilder& fillMode(Fill fill_mode);
    RasterizerBuilder& cullMode(Cull cull_mode);
    RasterizerBuilder& depthClip(bool depth_cilp);

    Rasterizer create();

   private:
    Fill fill;
    Cull cull;

    bool depth_clip;
};

};  // namespace Engine::Graphics