#include "Loader.h"

#include <memory>

#include "Logger/Logger.h"

using std::unique_ptr;

DXGI_FORMAT WICToDXGI(WICPixelFormatGUID format) {
    if (format == GUID_WICPixelFormat128bppRGBAFloat) {
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    if (format == GUID_WICPixelFormat64bppRGBAHalf) {
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    }
    if (format == GUID_WICPixelFormat64bppRGBA) {
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    }
    if (format == GUID_WICPixelFormat32bppRGBA) {
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    if (format == GUID_WICPixelFormat32bppBGRA) {
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    if (format == GUID_WICPixelFormat32bppBGR) {
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    }
    return DXGI_FORMAT_UNKNOWN;
}

ComPtr<ID3D11Texture2D> Loader::LoadTextureFromFile(const char* filename) {
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frameDecoder;
    WICPixelFormatGUID format;
    UINT width, height;

    const auto& tex = m_textureRegistry.find(filename);
    if (tex != m_textureRegistry.end()) {
        return tex->second.Get();
    }

    int size = strlen(filename) + 1;
    wchar_t* wpath = new wchar_t[size];
    mbstowcs(wpath, filename, size);

    m_factory->CreateDecoderFromFilename(
        wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

    decoder->GetFrame(0, &frameDecoder);

    frameDecoder->GetSize(&width, &height);

    unique_ptr<char[]> buffer(new char[height * width * 4]);
    frameDecoder->GetPixelFormat(&format);
    frameDecoder->CopyPixels(nullptr, width * 4, width * height * 4,
                             reinterpret_cast<BYTE*>(buffer.get()));
    ComPtr<ID3D11Texture2D> texture = Graphics::get()->CreateTexture2D(
        WICToDXGI(format), false, false, width, height, buffer.get());
    if (texture != nullptr) {
        m_textureRegistry.emplace(filename, texture);
    }
    return texture;
}

Material Loader::LoadMaterial(const aiMaterial* material) {
    Material mat;
    if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
        aiString relPath;
        if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &relPath) ==
            AI_SUCCESS) {
            string path = m_currentPath + "\\assets\\" + relPath.data;
            mat.baseColor = LoadTextureFromFile(path.c_str());
        }
    } else {
        string path = m_currentPath + "\\assets\\TexturePlaceholder.png";
        mat.baseColor = LoadTextureFromFile(path.c_str());
    }

    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString relPath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &relPath) ==
            AI_SUCCESS) {
            string path = m_currentPath + "\\assets\\" + relPath.data;
            mat.diffuse = LoadTextureFromFile(path.c_str());
        }
    } else {
        string path = m_currentPath + "\\assets\\WhitePlaceholder.png";
        mat.diffuse = LoadTextureFromFile(path.c_str());
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        aiString relPath;
        if (material->GetTexture(aiTextureType_SPECULAR, 0, &relPath) ==
            AI_SUCCESS) {
            string path = m_currentPath + "\\assets\\" + relPath.data;
            mat.specular = LoadTextureFromFile(path.c_str());
        }
    } else {
        string path = m_currentPath + "\\assets\\WhitePlaceholder.png";
        mat.specular = LoadTextureFromFile(path.c_str());
    }

    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
        aiString relPath;
        if (material->GetTexture(aiTextureType_EMISSIVE, 0, &relPath) ==
            AI_SUCCESS) {
            string path = m_currentPath + "\\assets\\" + relPath.data;
            mat.emission = LoadTextureFromFile(path.c_str());
        }
    } else {
        string path = m_currentPath + "\\assets\\BlackPlaceholder.png";
        mat.emission = LoadTextureFromFile(path.c_str());
    }

    ComPtr<ID3DBlob> vertexShader = CompileShaderFromFile(
        L"Shaders\\VertexShader.hlsl", "vs_5_0", shaderFlags);
    ComPtr<ID3DBlob> pixelShader = CompileShaderFromFile(
        L"Shaders\\PixelShader.hlsl", "ps_5_0", shaderFlags);

    if (vertexShader.Get() != nullptr && pixelShader.Get() != nullptr) {
        mat.inputLayout =
            Graphics::get()->CreateInputLayoutFromShader(vertexShader.Get());
        mat.pixelShader = Graphics::get()->CreatePixelShader(pixelShader.Get());
        mat.vertexShader =
            Graphics::get()->CreateVertexShader(vertexShader.Get());
    }
    return mat;
}

Mesh Loader::LoadMesh(const aiMesh* mesh) {
    vector<vertex> vert;
    vector<uint32_t> ind;

    for (int j = 0; j < mesh->mNumVertices; j++) {
        const aiVector3D pos = mesh->mVertices[j];
        const aiVector3D normal = mesh->mNormals[j];
        aiVector3D uv = {0.f, 0.f, 0.f};
        if (mesh->mTextureCoords != nullptr) {
            uv = mesh->mTextureCoords[0][j];
        }
        vert.emplace_back(vector3{pos.x, pos.y, pos.z}, vector2{uv.x, uv.y},
                          vector3{normal.x, normal.y, normal.z});
    }
    for (int j = 0; j < mesh->mNumFaces; j++) {
        const aiFace face = mesh->mFaces[j];
        ind.push_back(face.mIndices[0]);
        ind.push_back(face.mIndices[1]);
        ind.push_back(face.mIndices[2]);
    }

    ComPtr<ID3D11Buffer> vbuf = Graphics::get()->CreateVertexBuffer(
        vert.size() * sizeof(vertex), false, false, vert.data());
    IndexBuffer ibuf = Graphics::get()->CreateIndexBuffer(
        ind.size() * sizeof(uint32_t), false, ind.data());

    return {vbuf, ibuf, mesh->mMaterialIndex};
}

ComPtr<ID3DBlob> Loader::CompileShaderFromFile(const wchar_t* filename,
                                               const char* target, UINT flags) {
    ComPtr<ID3DBlob> shader, error;
    if (FAILED(D3DCompileFromFile(filename, nullptr, nullptr, "main", target,
                                  flags, NULL, shader.GetAddressOf(),
                                  error.GetAddressOf()))) {
        Logger::get()->Error(L"Failed to compile %s : %s", filename,
                             error->GetBufferPointer());
        return nullptr;
    }
    return shader;
}

Model* Loader::LoadModelFromFile(const char* filename) {
    Model model;
    Assimp::Importer importer;
    string path = m_currentPath + filename;

    const auto& mod = m_modelRegistry.find(filename);
    if (mod != m_modelRegistry.end()) {
        return &(mod->second);
    }

    const aiScene* scene = importer.ReadFile(
        path.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    for (int i = 0; i < scene->mNumMeshes; i++) {
        model.meshes.emplace_back(LoadMesh(scene->mMeshes[i]));
    }

    for (int i = 0; i < scene->mNumMaterials; i++) {
        model.materials.emplace_back(LoadMaterial(scene->mMaterials[i]));
    }

    return &(m_modelRegistry.emplace(filename, model).first->second);
}
