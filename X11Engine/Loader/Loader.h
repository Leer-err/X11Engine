#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <wincodec.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <iostream>
#include <map>
#include <unordered_map>

#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

using std::map;
using std::string;
using std::unordered_map;

#ifdef _DEBUG
constexpr UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG |
                             D3DCOMPILE_SKIP_OPTIMIZATION;
#else
constexpr UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

class Loader {
   public:
    inline static Loader* get() {
        static Loader instance;
        return &instance;
    }

    void LoadScene(const char* filename);

    Model* LoadModelFromFile(const char* filename);
    ComPtr<ID3D11Texture2D> LoadTextureFromFile(const char* filename);
    ComPtr<ID3D11Texture2D> LoadSkyboxFromFile(const char* filename);
    ComPtr<ID3DBlob> CompilePixelShaderFromFile(const wchar_t* filename,
                                                const char* entryPoint,
                                                UINT flags);
    ComPtr<ID3DBlob> CompileVertexShaderFromFile(const wchar_t* filename,
                                                 const char* entryPoint,
                                                 UINT flags);

   private:
    Material LoadMaterial(const aiMaterial* material);
    Mesh LoadMesh(const aiMesh* mesh);

    // Tries to load texture of specified type, if failed return defaultTexture
    ComPtr<ID3D11Texture2D> LoadTexture(const aiMaterial* material,
                                        aiTextureType type,
                                        const char* defaultTexture);

    Loader() : m_currentPath(std::filesystem::current_path().string()) {
        CoInitialize(NULL);
    }
    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;
    Loader(const Loader&&) = delete;
    Loader& operator=(const Loader&&) = delete;

    unordered_map<string, Model> m_modelRegistry;
    unordered_map<string, ComPtr<ID3D11Texture2D>> m_textureRegistry;

    string m_currentPath;
};