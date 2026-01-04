#pragma once

#include "Buffer.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"

class Stars {
   public:
    Stars();

    void draw();

   private:
    Mesh screen_plane;
    GraphicsPipeline pipeline;
    Buffer camera_parameters_buffer;
    Buffer star_parameters_buffer;
};