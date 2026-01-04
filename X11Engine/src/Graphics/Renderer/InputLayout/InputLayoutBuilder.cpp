#include "InputLayoutBuilder.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <stdexcept>
#include <vector>

#include "APIResources.h"
#include "InputLayout.h"
#include "VertexShader.h"

InputLayoutBuilder::InputLayoutBuilder(VertexShader shader)
    : elements(), shader(shader) {}

InputLayoutBuilder::~InputLayoutBuilder() {}

InputLayoutBuilder& InputLayoutBuilder::addElement(std::string name,
                                                   DXGI_FORMAT format) {
    elements.push_back(Element{name, format});

    return *this;
}

InputLayout InputLayoutBuilder::create() {
    auto device = APIResources::get().getDevice();
    auto shader_bytecode = shader.getBytecode();

    std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_elements(elements.size());

    for (int i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];

        D3D11_INPUT_ELEMENT_DESC desc;
        desc.SemanticName = element.name.c_str();
        desc.InputSlot = 0;
        desc.Format = element.format;
        desc.SemanticIndex = 0;
        desc.InstanceDataStepRate = 0;
        desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

        d3d_input_elements[i] = (desc);
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout = nullptr;
    HRESULT hr = device->CreateInputLayout(
        d3d_input_elements.data(), static_cast<UINT>(d3d_input_elements.size()),
        shader_bytecode->GetBufferPointer(), shader_bytecode->GetBufferSize(),
        input_layout.GetAddressOf());

    return InputLayout(input_layout);
}
