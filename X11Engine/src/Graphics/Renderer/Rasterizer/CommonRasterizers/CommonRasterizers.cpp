#include "CommonRasterizers.h"

#include "RasterizerBuilder.h"

Engine::Graphics::Rasterizer Engine::Graphics::CommonRasterizers::fill() {
    return get().fill_rasterizer;
}

Engine::Graphics::Rasterizer Engine::Graphics::CommonRasterizers::wireframe() {
    return get().wireframe_rasterizer;
}

Engine::Graphics::CommonRasterizers&
Engine::Graphics::CommonRasterizers::get() {
    static CommonRasterizers instance;

    return instance;
}

Engine::Graphics::CommonRasterizers::CommonRasterizers() {
    fill_rasterizer =
        RasterizerBuilder().fillMode(Fill::Solid).cullMode(Cull::Back).create();
    wireframe_rasterizer = RasterizerBuilder()
                               .fillMode(Fill::Wireframe)
                               .cullMode(Cull::Back)
                               .create();
}