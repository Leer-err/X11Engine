#include "CommonSamplers.h"

#include "SamplerBuilder.h"


Sampler Engine::Graphics::CommonSamplers::linear() {
    return get().linear_sampler;
}

Sampler Engine::Graphics::CommonSamplers::point() {
    return get().point_sampler;
}

Sampler Engine::Graphics::CommonSamplers::anisotropic() {
    return get().anisotropic_sampler;
}

Engine::Graphics::CommonSamplers& Engine::Graphics::CommonSamplers::get() {
    static CommonSamplers instance;

    return instance;
}

Engine::Graphics::CommonSamplers::CommonSamplers() {
    point_sampler = SamplerBuilder(Filter::Point).create();
    linear_sampler = SamplerBuilder(Filter::Linear).create();
    anisotropic_sampler = SamplerBuilder(Filter::Anisotropic).create();
}