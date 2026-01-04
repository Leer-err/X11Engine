#pragma once

#include <d3d11_1.h>
#include <dxgi1_4.h>

#include <string>
#include <vector>

#include "InputLayout.h"
#include "VertexShader.h"

class InputLayoutBuilder {
    struct Element {
        std::string name;
        DXGI_FORMAT format;
    };

   public:
    InputLayoutBuilder(VertexShader shader);
    ~InputLayoutBuilder();

    InputLayoutBuilder& addElement(std::string name, DXGI_FORMAT format);

    InputLayout create();

   private:
    std::vector<Element> elements;
    VertexShader shader;
};