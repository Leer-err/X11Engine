#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <wincodec.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "Framework/Graphics/Animation.h"
#include "Framework/Graphics/Graphics.h"
#include "Framework/Graphics/Model.h"

// TODO: get rid of Engine dependency
#include "Engine/Scene/Scene.h"

using nlohmann::json;
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
    ComPtr<ID3D11Texture2D> LoadTextureFromMemory(const char* name,
                                                  const aiTexture* texPtr);
    ComPtr<ID3D11Texture2D> LoadSkyboxFromFile(const char* filename);
    ComPtr<ID3DBlob> CompilePixelShaderFromFile(const wchar_t* filename,
                                                const char* entryPoint,
                                                UINT flags);
    ComPtr<ID3DBlob> CompileVertexShaderFromFile(const wchar_t* filename,
                                                 const char* entryPoint,
                                                 UINT flags);

   private:
    int LoadMaterial(const aiScene* scene, const aiMaterial* material);
    Mesh LoadMesh(const aiMesh* mesh);
    Mesh LoadMeshSkinning(const aiMesh* mesh, Skeleton& skeleton, float scale);
    Animation LoadAnimation(const aiAnimation* modelAnimation,
                            const unordered_map<string, int>& boneNames);
    vector<Mesh> GetMeshes(const aiScene* scene, Skeleton& skeleton,
                           float scale);

    void ProcessPointLight(json lightObject);
    void ProcessDirectionalLight(json lightObject);
    void ProcessPlayer(json playerObject);
    void ProcessEntity(json entityObject);

    // Tries to load texture of specified type, if failed return defaultTexture
    ComPtr<ID3D11Texture2D> LoadTexture(const aiScene* scene,
                                        const aiMaterial* material,
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