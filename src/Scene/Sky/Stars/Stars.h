#pragma once

// #include "Mesh.h"

#include "RenderWorld.h"
class Stars {
   public:
    Stars();

    void draw();

   private:
    float star_density;
    float blinking_speed;
    float blink_strength;

    Graphics::RenderWorld& render_world;
};