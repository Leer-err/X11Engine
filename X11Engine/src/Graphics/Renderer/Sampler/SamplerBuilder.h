#pragma once

#include "Sampler.h"

enum class Filter { Point, Linear, Anisotropic };

class SamplerBuilder {
   public:
    SamplerBuilder(Filter filter) : filter(filter) {}

    Sampler create();

   private:
    Filter filter;
};