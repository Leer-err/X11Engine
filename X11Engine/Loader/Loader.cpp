#include "Loader.h"

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <d3dcompiler.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <queue>

#include "ECS/Component/Components/AnimationComponent.h"
#include "ECS/Component/Components/CameraComponent.h"
#include "ECS/Component/Components/PointLightComponent.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Entity/EntityManager.h"
#include "ECS/System/SystemManager.h"
#include "ECS/System/Systems/LookSystem.h"
#include "ECS/System/Systems/MovementSystem.h"
#include "ECS/System/Systems/RenderSystem.h"
#include "Graphics/Bone.h"
#include "Logger/Logger.h"
#include "MaterialRegistry/MaterialRegistry.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using std::ifstream;
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

    ProcessPlayer(scene["player"]);
    ProcessDirectionalLight(scene["lights"]["directional"]);

    for (const auto& light : scene["lights"]["point"]) {
        ProcessPointLight(light);
    }
    for (const auto& entity : scene["entities"]) {
        ProcessEntity(entity);
    }
}

void Loader::ProcessEntity(json entityObject) {
    EntityId entity = ECS::EntityManager::get()->CreateEntity();

    json positionData = entityObject["position"];
    json rotationData = entityObject["rotation"];
    json scaleData = entityObject["scale"];

    vector3 position = entityObject["position"].get<vector3>();
    vector3 rotation;
    vector3 scale = vector3(1.f, 1.f, 1.f);

    if (!rotationData.empty()) {
        rotation = entityObject["rotation"].get<vector3>();
    }
    if (!scaleData.empty()) {
        scale = entityObject["scale"].get<vector3>();
    }

    TransformComponent* entityPosition =
        entity.AddComponent<TransformComponent>(Scene::get()->GetWorldNode(),
                                                position, rotation, scale);

    string modelFile = entityObject["model"].get<string>();
    if (!modelFile.empty()) {
        Model* model = LoadModelFromFile(modelFile.data());

        entity.AddComponent<RenderComponent>(entityPosition->sceneNode, model);
        entity.AddComponent<AnimationComponent>("mixamo.com", model);
    }
}

void Loader::ProcessPointLight(json lightObject) {
    EntityId light = ECS::EntityManager::get()->CreateEntity();

    json data = lightObject["data"];

    json position = lightObject["position"];

    PointLight lightData;
    lightData.constant = data["constant"].get<float>();
    lightData.lin = data["lin"].get<float>();
    lightData.quadratic = data["quadratic"].get<float>();

    lightData.ambient = data["ambient"].get<vector3>();
    lightData.diffuse = data["diffuse"].get<vector3>();
    lightData.specular = data["specular"].get<vector3>();

    light.AddComponent<TransformComponent>(Scene::get()->GetWorldNode(),
                                           position.get<vector3>(),
                                           quaternion(), vector3());
    light.AddComponent<PointLightComponent>(lightData);
}

void Loader::ProcessDirectionalLight(json lightObject) {
    vector3 direction = lightObject["direction"].get<vector3>();

    vector3 ambient = lightObject["ambient"].get<vector3>();
    vector3 diffuse = lightObject["diffuse"].get<vector3>();
    vector3 specular = lightObject["specular"].get<vector3>();

    Graphics::get()->SetDirLight({direction, ambient, diffuse, specular});
}

void Loader::ProcessPlayer(json playerObject) {
    EntityId player = ECS::EntityManager::get()->CreateEntity();

    vector3 position = playerObject["position"].get<vector3>();

    TransformComponent* playerTransform =
        player.AddComponent<TransformComponent>(Scene::get()->GetWorldNode(),
                                                position, quaternion(),
                                                vector3(1.f, 1.f, 1.f));
    player.AddComponent<CameraComponent>(
        1000.f, .01f, DirectX::XMConvertToRadians(60.f), 16.f / 9.f);

    ECS::SystemManager::get()->GetSystem<MovementSystem>()->SetPlayer(
        playerTransform);
    ECS::SystemManager::get()->GetSystem<LookSystem>()->SetPlayer(
        playerTransform);
    ECS::SystemManager::get()->GetSystem<RenderSystem>()->SetCamera(player);
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

ComPtr<ID3D11Texture2D> Loader::LoadTextureFromMemory(const char* name,
                                                      const aiTexture* texPtr) {
    ComPtr<ID3D11Texture2D> texture;

    const auto& tex = m_textureRegistry.find(name);
    if (tex != m_textureRegistry.end()) {
        return tex->second.Get();
    }

    int x, y, n;

    unsigned char* data = stbi_load_from_memory((unsigned char*)texPtr->pcData,
                                                texPtr->mWidth, &x, &y, &n, 4);

    texture = Graphics::get()->CreateTexture2D(DXGI_FORMAT_R8G8B8A8_UNORM,
                                               false, false, x, y, data);

    if (texture != nullptr) {
        m_textureRegistry.emplace(name, texture);
    }
    return texture;
}

int Loader::LoadMaterial(const aiScene* scene, const aiMaterial* material) {
    Material mat;

    ComPtr<ID3DBlob> vertexShader = CompileVertexShaderFromFile(
        L"Shaders\\BoneVShader.hlsl", "main", shaderFlags);
    ComPtr<ID3DBlob> pixelShader = CompilePixelShaderFromFile(
        L"Shaders\\PixelShader.hlsl", "main", shaderFlags);

    if (vertexShader.Get() != nullptr && pixelShader.Get() != nullptr) {
        mat.pixelShader = Graphics::get()->CreatePixelShader(pixelShader.Get());
        mat.vertexShader =
            Graphics::get()->CreateVertexShader(vertexShader.Get());
    }

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.baseColorIndex] =
        LoadTexture(scene, material, aiTextureType_DIFFUSE,
                    "ColorPlaceholder.png");

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.specularIndex] =
        LoadTexture(scene, material, aiTextureType_SPECULAR,
                    "WhitePlaceholder.png");

    mat.resources[PIXEL_SHADER_STAGE].textures[mat.pixelShader.emissionIndex] =
        LoadTexture(scene, material, aiTextureType_EMISSIVE,
                    "BlackPlaceholder.png");

    return MaterialRegistry::get()->AddMaterial(std::move(mat));
}

Mesh Loader::LoadMesh(const aiMesh* mesh) {
    vector3 min = vector3::max();
    vector3 max = vector3::min();

    vector<Vertex> vert;
    vector<uint32_t> ind;

    Mesh res;

    for (int j = 0; j < mesh->mNumVertices; j++) {
        const aiVector3D pos = mesh->mVertices[j];
        const aiVector3D norm = mesh->mNormals[j];
        const aiVector3D uv = mesh->mTextureCoords[0][j];

        vector3 position = {pos.x, pos.y, pos.z};
        vector3 normal = {norm.x, norm.y, norm.z};
        vector2 texCoords = {uv.x, uv.y};

        min.x = std::min(min.x, pos.x);
        min.y = std::min(min.y, pos.y);
        min.z = std::min(min.z, pos.z);

        max.x = std::max(max.x, pos.x);
        max.y = std::max(max.y, pos.y);
        max.z = std::max(max.z, pos.z);

        vert.emplace_back(position, normal, texCoords);
    }

    for (int j = 0; j < mesh->mNumFaces; j++) {
        const aiFace face = mesh->mFaces[j];
        ind.push_back(face.mIndices[0]);
        ind.push_back(face.mIndices[1]);
        ind.push_back(face.mIndices[2]);
    }

    res.vertices = Graphics::get()->CreateVertexBuffer(
        vert.size() * sizeof(Vertex), false, false, vert.data());
    res.indices = Graphics::get()->CreateIndexBuffer(
        ind.size() * sizeof(uint32_t), false, ind.data());
    res.materialIndex = mesh->mMaterialIndex;
    res.boundingBox = AABB(min, max);
    return res;
}

Mesh Loader::LoadMeshSkinning(const aiMesh* mesh, Skeleton& skeleton,
                              float scale) {
    vector3 min = vector3::max();
    vector3 max = vector3::min();

    vector<VertexSkinning> vert;
    vector<uint32_t> ind;

    Mesh res;

    for (int j = 0; j < mesh->mNumVertices; j++) {
        const aiVector3D pos = mesh->mVertices[j];
        const aiVector3D norm = mesh->mNormals[j];
        const aiVector3D uv = mesh->mTextureCoords[0][j];

        vector3 position = {pos.x, pos.y, pos.z};
        position /= scale;

        vector3 normal = {norm.x, norm.y, norm.z};
        vector2 texCoords = {uv.x, uv.y};

        min.x = std::min(min.x, position.x);
        min.y = std::min(min.y, position.y);
        min.z = std::min(min.z, position.z);

        max.x = std::max(max.x, position.x);
        max.y = std::max(max.y, position.y);
        max.z = std::max(max.z, position.z);

        vert.emplace_back(position, normal, texCoords);
    }

    for (int j = 0; j < mesh->mNumFaces; j++) {
        const aiFace face = mesh->mFaces[j];
        ind.push_back(face.mIndices[0]);
        ind.push_back(face.mIndices[1]);
        ind.push_back(face.mIndices[2]);
    }

    for (int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        string boneName = bone->mName.C_Str();

        int boneIndex = skeleton.boneNames.at(boneName);
        skeleton.offsetMatrices[boneIndex] =
            matrix(bone->mOffsetMatrix).Transpose().ChangeUnit(scale);

        for (int j = 0; j < bone->mNumWeights; j++) {
            aiVertexWeight weight = bone->mWeights[j];

            VertexSkinning v = vert[weight.mVertexId];

            int weightIndex;
            for (weightIndex = 0;
                 weightIndex < 4 && v.indices[weightIndex] != -1;
                 weightIndex++) {
            }

            v.indices[weightIndex] = boneIndex;
            v.weights[weightIndex] = weight.mWeight;

            vert[weight.mVertexId] = v;
        }
    }

    res.vertices = Graphics::get()->CreateVertexBuffer(
        vert.size() * sizeof(VertexSkinning), false, false, vert.data());
    res.indices = Graphics::get()->CreateIndexBuffer(
        ind.size() * sizeof(uint32_t), false, ind.data());
    res.materialIndex = mesh->mMaterialIndex;
    res.boundingBox = AABB(min, max);
    return res;
}

Animation Loader::LoadAnimation(const aiAnimation* modelAnimation,
                                const unordered_map<string, int>& boneNames) {
    Animation animation;

    animation.name = modelAnimation->mName.C_Str();
    animation.m_duration = modelAnimation->mDuration;
    animation.m_ticksPerSecond = modelAnimation->mTicksPerSecond;
    animation.m_boneKeys.resize(boneNames.size());

    for (int i = 0; i < modelAnimation->mNumChannels; i++) {
        const aiNodeAnim* anim = modelAnimation->mChannels[i];
        int index = boneNames.at(anim->mNodeName.C_Str());

        BoneKeys boneKeys;
        for (int j = 0; j < anim->mNumPositionKeys; j++) {
            vector3 position = anim->mPositionKeys[j].mValue;
            boneKeys.positions.emplace_back(position / 100.f,
                                            anim->mPositionKeys[j].mTime);
        }
        for (int j = 0; j < anim->mNumRotationKeys; j++) {
            auto& assimpQuaternion = anim->mRotationKeys[j].mValue;
            quaternion rotation = {assimpQuaternion.x, assimpQuaternion.y,
                                   assimpQuaternion.z, assimpQuaternion.w};
            boneKeys.rotations.emplace_back(rotation,
                                            anim->mRotationKeys[j].mTime);
        }
        for (int j = 0; j < anim->mNumScalingKeys; j++) {
            boneKeys.scalings.emplace_back(anim->mScalingKeys[j].mValue,
                                           anim->mPositionKeys[j].mTime);
        }
        animation.m_boneKeys[index] = boneKeys;
    }

    return animation;
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

vector<Mesh> Loader::GetMeshes(const aiScene* scene, Skeleton& skeleton,
                               float scale) {
    vector<Mesh> meshes;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        meshes.push_back(LoadMeshSkinning(scene->mMeshes[i], skeleton, scale));
    }
    return meshes;
}

aiNode* FindRootBone(const aiScene* scene) {
    auto& rootNode = scene->mRootNode;

    for (int i = 0; i < rootNode->mNumChildren; i++) {
        bool isRootBone = true;

        std::queue<aiNode*> nodes;
        nodes.push(rootNode->mChildren[i]);

        while (!nodes.empty()) {
            auto& node = nodes.front();

            if (node->mNumMeshes > 0) {
                // all children of root bone must be without meshes
                isRootBone = false;
                break;
            }

            for (int j = 0; j < node->mNumChildren; j++) {
                nodes.push(node->mChildren[j]);
            }

            nodes.pop();
        }

        if (isRootBone) {
            return rootNode->mChildren[i];
        }
    }

    return nullptr;  // when no root bone found
}

Skeleton LoadSkeleton(const aiNode* rootBone, matrix globalTransformation) {
    Skeleton skeleton;
    std::queue<aiNode*> nodes;
    for (int i = 0; i < rootBone->mNumChildren; i++) {
        nodes.push(rootBone->mChildren[i]);
    }

    skeleton.boneNames.emplace(rootBone->mName.C_Str(), 0);
    skeleton.parents.push_back(INVALID_PARENT);
    skeleton.offsetMatrices.push_back(matrix(rootBone->mTransformation)
                                          .Transpose()
                                          .ChangeUnit(100.f)
                                          .Inverse());

    while (!nodes.empty()) {
        auto& node = nodes.front();

        for (int i = 0; i < node->mNumChildren; i++) {
            nodes.push(node->mChildren[i]);
        }

        int id = skeleton.parents.size();

        skeleton.boneNames.emplace(node->mName.C_Str(), id);

        skeleton.offsetMatrices.push_back(matrix(node->mTransformation)
                                              .Transpose()
                                              .ChangeUnit(100.f)
                                              .Inverse());

        int parentId = skeleton.boneNames[node->mParent->mName.C_Str()];
        skeleton.parents.push_back(parentId);

        nodes.pop();
    }

    return skeleton;
}

Model* Loader::LoadModelFromFile(const char* filename) {
    Model model;
    Assimp::Importer importer;

    string path = m_currentPath + "\\Assets\\" + filename;

    const auto iter = m_modelRegistry.find(path);
    if (iter != m_modelRegistry.end()) {
        return &iter->second;
    }

    const aiScene* scene = importer.ReadFile(
        path.c_str(), aiProcess_ValidateDataStructure | aiProcess_Triangulate |
                          aiProcess_ConvertToLeftHanded);

    aiNode* rootBone = FindRootBone(scene);
    Skeleton skeleton =
        LoadSkeleton(rootBone, scene->mRootNode->mTransformation);
    model.meshes = GetMeshes(scene, skeleton, 100.0f);

    model.skeleton = skeleton;

    for (int i = 0; i < scene->mNumMaterials; i++) {
        model.materials.emplace_back(LoadMaterial(scene, scene->mMaterials[i]));
    }

    for (int i = 0; i < scene->mNumAnimations; i++) {
        Animation animation =
            LoadAnimation(scene->mAnimations[i], skeleton.boneNames);
        int index = model.animations.size();
        model.animationNames.emplace(animation.name, index);
        model.animations.emplace_back(animation);
    }

    return &(m_modelRegistry.emplace(filename, model).first->second);
}

ComPtr<ID3D11Texture2D> Loader::LoadTexture(const aiScene* scene,
                                            const aiMaterial* material,
                                            aiTextureType type,
                                            const char* defaultTexture) {
    aiString relPath;

    aiString texture_file;
    auto result = material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);
    if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {
        return LoadTextureFromMemory(texture_file.C_Str(), texture);
    } else if (result == aiReturn_SUCCESS) {
        return LoadTextureFromFile(relPath.C_Str());
    } else {
        return LoadTextureFromFile(defaultTexture);
    }
}