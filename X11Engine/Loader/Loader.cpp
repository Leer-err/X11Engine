#include "Loader.h"

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <d3dcompiler.h>

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Logger/Logger.h"
#include "Window.h"

using nlohmann::json;
using std::ifstream;
using std::string;
using std::unique_ptr;

void Loader::LoadScene(const char* filename) {
    string name = m_currentPath + "\\Assets\\" + filename;

    ifstream file(name.data());
    if (!file.is_open()) {
        FatalError(L"Failed to open file");
    }
    json scene = json::parse(file);

    string skyboxName = scene["skybox"].get<string>();

    Graphics::get()->SetSkybox(LoadSkyboxFromFile(skyboxName.data()));
}

ComPtr<ID3D11Texture2D> Loader::LoadTextureFromFile(const char* filename) {
    string name = m_currentPath + "\\Assets\\" + filename;
    ComPtr<ID3D11Texture2D> texture;

    const auto& tex = m_textureRegistry.find(filename);
    if (tex != m_textureRegistry.end()) {
        return tex->second.Get();
    }

    wchar_t* wpath = new wchar_t[name.size() + 1];
    mbstowcs(wpath, name.c_str(), name.size() + 1);

    DirectX::CreateWICTextureFromFile(
        (ID3D11Device*)Graphics::get()->GetDevice(), wpath,
        (ID3D11Resource**)texture.GetAddressOf(), nullptr);

    delete (wpath);

    if (texture != nullptr) {
        m_textureRegistry.emplace(filename, texture);
    }
    return texture;
}

ComPtr<ID3D11Texture2D> Loader::LoadSkyboxFromFile(const char* filename) {
    string name = m_currentPath + "\\Assets\\" + filename;
    ComPtr<ID3D11Texture2D> texture;

    const auto& tex = m_textureRegistry.find(filename);
    if (tex != m_textureRegistry.end()) {
        return tex->second.Get();
    }

    wchar_t* wpath = new wchar_t[name.size() + 1];
    mbstowcs(wpath, name.c_str(), name.size() + 1);

    HRESULT hr = DirectX::CreateDDSTextureFromFile(
        (ID3D11Device*)Graphics::get()->GetDevice(), wpath,
        (ID3D11Resource**)texture.GetAddressOf(), nullptr);

    delete (wpath);

    if (texture != nullptr) {
        m_textureRegistry.emplace(filename, texture);
    }
    return texture;
}

Material Loader::LoadMaterial(const aiMaterial* material) {
    Material mat;

    ComPtr<ID3DBlob> vertexShader = CompileVertexShaderFromFile(
        L"Shaders\\VertexShader.hlsl", "main", shaderFlags);
    ComPtr<ID3DBlob> pixelShader = CompilePixelShaderFromFile(
        L"Shaders\\PixelShader.hlsl", "main", shaderFlags);

    if (vertexShader.Get() != nullptr && pixelShader.Get() != nullptr) {
        mat.pixelShader = Graphics::get()->CreatePixelShader(pixelShader.Get());
        mat.vertexShader =
            Graphics::get()->CreateVertexShader(vertexShader.Get());
    }

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.baseColorIndex] =
        LoadTexture(material, aiTextureType_DIFFUSE, "ColorPlaceholder.png");

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.specularIndex] =
        LoadTexture(material, aiTextureType_SPECULAR, "WhitePlaceholder.png");

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.emissionIndex] =
        LoadTexture(material, aiTextureType_EMISSIVE, "BlackPlaceholder.png");

    return mat;
}

Mesh Loader::LoadMesh(const aiMesh* mesh) {
    vector3 min(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());

    vector3 max(std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min());

    vector<vertex> vert;
    vector<uint32_t> ind;

    for (int j = 0; j < mesh->mNumVertices; j++) {
        const aiVector3D pos = mesh->mVertices[j];
        const aiVector3D normal = mesh->mNormals[j];
        aiVector3D uv = {0.f, 0.f, 0.f};
        if (mesh->mTextureCoords != nullptr) {
            uv = mesh->mTextureCoords[0][j];
        }

        if (pos.x < min.x) min.x = pos.x;
        if (pos.y < min.y) min.y = pos.y;
        if (pos.z < min.z) min.z = pos.z;

        if (pos.x > max.x) max.x = pos.x;
        if (pos.y > max.y) max.y = pos.y;
        if (pos.z > max.z) max.z = pos.z;

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

    return {vbuf, ibuf, mesh->mMaterialIndex, AABB(min, max)};
}

ComPtr<ID3DBlob> Loader::CompilePixelShaderFromFile(const wchar_t* filename,
                                                    const char* entryPoint,
                                                    UINT flags) {
    ComPtr<ID3DBlob> shader, error;

    if (FAILED(D3DCompileFromFile(filename, nullptr, nullptr, entryPoint,
                                  "ps_5_0", flags, NULL, shader.GetAddressOf(),
                                  error.GetAddressOf()))) {
        wchar_t* message = new wchar_t[error->GetBufferSize()];
        mbstowcs(message, (char*)error->GetBufferPointer(),
                 error->GetBufferSize());

        Logger::get()->Error(L"Failed to compile %s : %s", filename, message);

        delete (message);
        return nullptr;
    }
    return shader;
}

ComPtr<ID3DBlob> Loader::CompileVertexShaderFromFile(const wchar_t* filename,
                                                     const char* entryPoint,
                                                     UINT flags) {
    ComPtr<ID3DBlob> shader, error;
    if (FAILED(D3DCompileFromFile(filename, nullptr, nullptr, entryPoint,
                                  "vs_5_0", flags, NULL, shader.GetAddressOf(),
                                  error.GetAddressOf()))) {
        wchar_t* message = new wchar_t[error->GetBufferSize()];
        mbstowcs(message, (char*)error->GetBufferPointer(),
                 error->GetBufferSize());

        Logger::get()->Error(L"Failed to compile %s : %s", filename, message);

        delete (message);
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

ComPtr<ID3D11Texture2D> Loader::LoadTexture(const aiMaterial* material,
                                            aiTextureType type,
                                            const char* defaultTexture) {
    aiString relPath;

    if (material->GetTextureCount(type) > 0 &&
        material->GetTexture(type, 0, &relPath) == AI_SUCCESS) {
        return LoadTextureFromFile(relPath.C_Str());
    } else {
        return LoadTextureFromFile(defaultTexture);
    }
}