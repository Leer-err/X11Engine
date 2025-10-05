#ifndef D3D_SHADER_BLOB
#define D3D_SHADER_BLOB

#include <d3dcommon.h>
#include <wrl/client.h>

#include "IShaderBlob.h"

class D3DShaderBlob : public IShaderBlob {
   public:
    D3DShaderBlob(Microsoft::WRL::ComPtr<ID3DBlob> shader);

    void* get() override;
    size_t getSize() override;

   private:
    Microsoft::WRL::ComPtr<ID3DBlob> shader;
};

#endif  // D3D_SHADER_BLOB