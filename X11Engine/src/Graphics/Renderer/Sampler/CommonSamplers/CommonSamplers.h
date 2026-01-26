#pragma once

#include "Sampler.h"

namespace Engine::Graphics {

class CommonSamplers {
   public:
    static Sampler linear();
    static Sampler point();
    static Sampler anisotropic();

   private:
    CommonSamplers();

    static CommonSamplers& get();

    Sampler linear_sampler;
    Sampler point_sampler;
    Sampler anisotropic_sampler;
};

};  // namespace Engine::Graphics