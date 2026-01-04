#pragma once

#include "Clouds.h"
#include "Stars.h"

class Sky {
   public:
    Sky() = default;

    void draw();

   private:
    Clouds clouds;
    Stars stars;
};