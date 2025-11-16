#pragma once

#include "Format.h"
#include "Image.h"
#include "Result.h"
#include "Texture.h"

enum class TextureError {
    UnsupportedFormat,
    NoDataForImmutableResource,
    WriteFromGPUAndCPU
};

class TextureBuilder {
   public:
    TextureBuilder(ImageFormat format, uint32_t width, uint32_t height);
    static TextureBuilder fromImage(const Image& image);

    TextureBuilder& withData(const char* data);
    TextureBuilder& isShaderResource();
    TextureBuilder& isRenderTarget();
    TextureBuilder& isDepthStencil();
    TextureBuilder& isCPUWritable();
    TextureBuilder& isGPUWritable();

    Result<Texture, TextureError> create();

   private:
    const char* data;
    ImageFormat format;
    uint32_t width;
    uint32_t height;

    bool shader_resource;
    bool render_target;
    bool depth_stencil;
    bool cpu_writable;
    bool gpu_writable;
};