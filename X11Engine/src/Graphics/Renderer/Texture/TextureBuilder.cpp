#include "TextureBuilder.h"

#include <d3d11.h>

#include "APIResources.h"

constexpr std::optional<DXGI_FORMAT> imageFormatToDXGIFormat(
    ImageFormat format) {
    switch (format) {
        case ImageFormat::BGRA_32BPP:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case ImageFormat::RGBA_32BPP:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            return std::nullopt;
    }
}

TextureBuilder::TextureBuilder(ImageFormat format, uint32_t width,
                               uint32_t height)
    : data(nullptr),
      format(format),
      width(width),
      height(height),
      shader_resource(false),
      render_target(false),
      depth_stencil(false),
      cpu_writable(false),
      gpu_writable(false) {}

TextureBuilder TextureBuilder::fromImage(const Image& image) {
    return TextureBuilder(image.getFormat(), image.getWidth(),
                          image.getHeight())
        .withData((char*)image.getData());
}

TextureBuilder& TextureBuilder::withData(const char* data) {
    this->data = data;
    return *this;
}

TextureBuilder& TextureBuilder::isShaderResource() {
    shader_resource = true;
    return *this;
}

TextureBuilder& TextureBuilder::isRenderTarget() {
    render_target = true;
    return *this;
}

TextureBuilder& TextureBuilder::isDepthStencil() {
    depth_stencil = true;
    return *this;
}

TextureBuilder& TextureBuilder::isCPUWritable() {
    cpu_writable = true;
    return *this;
}

TextureBuilder& TextureBuilder::isGPUWritable() {
    gpu_writable = true;
    return *this;
}

Result<Texture, TextureError> TextureBuilder::create() {
    if (cpu_writable && gpu_writable) return TextureError::WriteFromGPUAndCPU;
    if (!cpu_writable && !gpu_writable && !render_target && data == nullptr)
        return TextureError::NoDataForImmutableResource;

    auto dxgi_format_optional = imageFormatToDXGIFormat(format);
    if (dxgi_format_optional == std::nullopt)
        return TextureError::UnsupportedFormat;
    auto dxgi_format = dxgi_format_optional.value();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = dxgi_format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;

    if (shader_resource) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    if (render_target) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    if (depth_stencil) desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

    if (!cpu_writable && !gpu_writable)
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    else if (cpu_writable)
        desc.Usage = D3D11_USAGE_DYNAMIC;
    else if (gpu_writable)
        desc.Usage = D3D11_USAGE_DEFAULT;

    if (cpu_writable) desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    size_t bytes_per_pixel = imageFormatBytesPerPixel(format);

    D3D11_SUBRESOURCE_DATA sub_data = {};
    sub_data.pSysMem = data;
    sub_data.SysMemPitch = width * bytes_per_pixel;
    sub_data.SysMemSlicePitch = width * height * bytes_per_pixel;

    D3D11_SUBRESOURCE_DATA* sub_data_ptr =
        data == nullptr ? nullptr : &sub_data;

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    auto result = device->CreateTexture2D(&desc, sub_data_ptr, &texture);

    return Texture(texture, width, height, dxgi_format);
}
