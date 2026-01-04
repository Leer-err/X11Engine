#ifndef CONTEXT_H
#define CONTEXT_H

// #include "Binding.h"
#include <d3d11.h>
#include <d3d11_1.h>

#include "Binding.h"
#include "Buffer.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Sampler.h"
#include "ShaderResource.h"
#include "Texture.h"

struct ID3D11DeviceContext1;

class Context {
   public:
    Context();

    void setPipeline(const GraphicsPipeline& pipeline);
    void cleanPipeline();

    void draw(const Mesh& mesh);

    void copy(const Texture& source, Texture destination);

    template <typename T>
    T* mapConstantBuffer(Buffer buffer) {
        return reinterpret_cast<T*>(internalMap(buffer));
    }
    void unmapConstantBuffer(const Buffer& buffer);

    void bindConstantBuffer(const Buffer& resource, const Binding& binding);
    void unbindConstantBuffer(const Binding& binding);
    void bindShaderResource(const ShaderResource& resource,
                            const Binding& binding);
    void unbindShaderResource(const Binding& binding);
    void bindSampler(const Sampler& sampler, const Binding& binding);
    void unbindSampler(const Binding& binding);

    void clean(RenderTarget render_target);

    // void setRenderTarget();

    // template <typename T>
    // void bindConstantBuffer();

    // void bindTexture(const Binding& binding, const Texture& texture);
    //  void bindSampler();

   private:
    void* internalMap(Buffer buffer);

    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context;
};

#endif  // CONTEXT_H