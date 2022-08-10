#pragma once
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <memory>
#include <mutex>
#include <vector>

#include "Components/CameraComponent.h"
#include "Helper.h"
#include "Light.h"
#include "Model.h"

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::mutex;
using std::unique_ptr;
using std::vector;

class SwapChain;

constexpr uint32_t VERTEX_BUFFER_SIZE = 60000;
constexpr uint32_t INDEX_BUFFER_SIZE = 100000;

constexpr int MAX_POINT_LIGHTS = 4;

class Graphics {
    struct {
        matrix projection;
    } CB_VS_PER_WINDOW;

    struct {
        matrix view;
    } CB_VS_PER_FRAME;

    struct {
        matrix world;
        matrix normalMatrix;
    } CB_VS_PER_MODEL;

    struct {
        vector3 viewPos;
        int lightCount;
        struct {
            vector3 direction;
            char p0[4];
            vector3 ambient;
            char p1[4];
            vector3 diffuse;
            char p2[4];
            vector3 specular;
            char p3[4];
        } dirLight;
    } CB_PS_PER_FRAME;  // char's here just for 16 byte memory alignment

    struct PointLight {
        vector3 position;

        vector3 ambient;
        vector3 diffuse;
        vector3 specular;

        float constant;
        float lin;
        float quadratic;
    };

    vector<PointLight> pointLights;

   public:
    inline static Graphics* get() {
        static Graphics instance;
        return &instance;
    }

    void Clear();
    void Present();
    void Draw(const Model* model);

    void SetProjectionMatrix();
    void SetViewMatrix(const quaternion& viewDirection,
                       const vector3& cameraPosition);
    void SetWorldMatrix(const matrix& world);
    void SetDirLight(const DirLight& light);
    void SetPointLight(const ::PointLight& light, const vector3& position);

    void UpdatePerFrameBuffers();
    void UpdatePerModelBuffers();
    void UpdatePerWindowBuffers();

    ComPtr<ID3D11Buffer> CreateConstantBuffer(bool CPUWritable,
                                              const void* data,
                                              size_t dataSize) const;
    ComPtr<ID3D11Buffer> CreateStructuredBuffer(UINT count, UINT structureSize,
                                                bool CPUWritable,
                                                bool GPUWritable,
                                                const void* data) const;
    ComPtr<ID3D11Buffer> CreateVertexBuffer(UINT size, bool dynamic,
                                            bool streamout,
                                            const void* data) const;
    IndexBuffer CreateIndexBuffer(UINT size, bool dynamic,
                                  const void* data) const;
    bool UpdateConstantBuffer(ComPtr<ID3D11Buffer> buffer,
                              const void* data) const;
    bool UpdateBuffer(ComPtr<ID3D11Buffer> buf, const void* data,
                      size_t size) const;
    ComPtr<ID3D11Texture2D> CreateTexture2D(DXGI_FORMAT format,
                                            bool CPUWritable, bool GPUWritable,
                                            int width, int height,
                                            const void* data) const;
    ComPtr<ID3D11ShaderResourceView> CreateBufferSRV(ComPtr<ID3D11Resource> res,
                                                     UINT elementSize,
                                                     UINT numElements) const;
    ComPtr<ID3D11ShaderResourceView> CreateTexture2DSRV(
        ComPtr<ID3D11Resource> res,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN) const;
    ComPtr<ID3D11InputLayout> CreateInputLayoutFromShader(
        ComPtr<ID3DBlob> shaderBytecode);

    ComPtr<ID3D11PixelShader> CreatePixelShader(
        ComPtr<ID3DBlob> shaderBytecode);
    ComPtr<ID3D11VertexShader> CreateVertexShader(
        ComPtr<ID3DBlob> shaderBytecode);

    inline IDXGIFactory7* GetFactory() const { return m_factory.Get(); }
    inline ID3D11Device5* GetDevice() const { return m_device.Get(); }
    inline ID3D11DeviceContext4* GetContext() const { return m_context.Get(); }

   private:
    Graphics();
    ~Graphics();

    ComPtr<ID3D11RenderTargetView> CreateTexture2DRTV(
        ComPtr<ID3D11Resource> res, UINT mip) const;
    ComPtr<ID3D11Texture2D> CreateDepthStencil2D(DXGI_FORMAT format,
                                                 bool GPUReadable, int width,
                                                 int height) const;
    ComPtr<ID3D11DepthStencilView> CreateTexture2DDSV(
        ComPtr<ID3D11Resource> res, UINT mip) const;

    ComPtr<IDXGIFactory7> m_factory;
    ComPtr<ID3D11Device5> m_device;
    ComPtr<ID3D11DeviceContext4> m_context;
    ComPtr<ID3D11RenderTargetView> m_rtv;
    ComPtr<IDXGIAdapter4> m_adapter;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;

#ifdef _DEBUG
    ComPtr<ID3D11Debug> m_debug;
#endif

    ComPtr<ID3D11SamplerState> m_sampler;

    unique_ptr<SwapChain> m_swapChain;
    ComPtr<ID3D11Buffer> m_CBVSFrame;
    ComPtr<ID3D11Buffer> m_CBVSModel;
    ComPtr<ID3D11Buffer> m_CBVSWindow;
    ComPtr<ID3D11Buffer> m_CBPSFrame;
    ComPtr<ID3D11Buffer> m_lightBuffer;

    D3D_FEATURE_LEVEL m_featureLevel;

    mutex m_renderMutex;
};