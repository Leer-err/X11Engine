#pragma once

#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include "Logger.h"

class APIResources {
   public:
    static APIResources& get() {
        static APIResources instance;
        return instance;
    }

    Microsoft::WRL::ComPtr<ID3D11Device1> getDevice() const;
    Microsoft::WRL::ComPtr<IDXGIFactory4> getFactory() const;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> getAdapter() const;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> getContext() const;

   private:
    APIResources();
    APIResources(const APIResources&);
    APIResources& operator=(const APIResources&);
    APIResources(APIResources&&);
    APIResources& operator=(APIResources&&);

    Microsoft::WRL::ComPtr<ID3D11Device1> device;
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context;

    Logger logger;
};