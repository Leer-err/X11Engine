#ifndef RESOURCE_FACTORY_H
#define RESOURCE_FACTORY_H

#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include "Logger.h"
#include "Result.h"

class Texture;
class SwapChain;
class RenderContext;
class RenderTarget;
class DepthStencil;

enum class ResourceError { UnsupportedFormat, WriteFromGPUAndCPU };

class ResourceFactory {
   public:
    static ResourceFactory& get() {
        static ResourceFactory instance;
        return instance;
    }

    SwapChain createSwapChain(uint32_t width, uint32_t height,
                              bool is_fullscreen);
    // RenderContext createContext();

    // Result<Texture, ResourceError> createTexture(const Image& image,
    //                                              bool cpu_witable,
    //                                              bool gpu_writable);
    // Texture createTexture(ImageFormat format, uint32_t width, uint32_t
    // height,
    //                       bool cpu_witable, bool gpu_writable);

    // RenderTarget createRenderTarget(Texture texture);
    // DepthStencil createDepthStencil(uint32_t width, uint32_t height);

   private:
    void createDXGIFactory();
    void createDXDevice();

    ResourceFactory();
    ResourceFactory(const ResourceFactory&) = delete;
    ResourceFactory(ResourceFactory&&) = delete;
    ResourceFactory& operator=(const ResourceFactory&) = delete;
    ResourceFactory& operator=(ResourceFactory&&) = delete;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

    Logger logger;
};

#endif  // RESOURCE_FACTORY_H