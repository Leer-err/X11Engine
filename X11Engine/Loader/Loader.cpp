#include "Loader.h"

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <d3dcompiler.h>

#include <fstream>
#include <memory>
#include <queue>

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

    vector3 position(positionData[0].get<float>(), positionData[1].get<float>(),
                     positionData[2].get<float>());
    vector3 rotation;
    vector3 scale(1.f, 1.f, 1.f);

    if (!rotationData.empty()) {
        rotation =
            vector3(rotationData[0].get<float>(), rotationData[1].get<float>(),
                    rotationData[2].get<float>());
    }
    if (!scaleData.empty()) {
        scale = vector3(scaleData[0].get<float>(), scaleData[1].get<float>(),
                        scaleData[2].get<float>());
    }
    TransformComponent* entityPosition =
        entity.AddComponent<TransformComponent>(Scene::get()->GetWorldNode(),
                                                position, rotation, scale);

    string modelFile = entityObject["model"].get<string>();
    if (!modelFile.empty()) {
        Model* model = Loader::get()->LoadModelFromFile(modelFile.data());

        entity.AddComponent<RenderComponent>(entityPosition->sceneNode, model);
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

    json ambient = data["ambient"];
    lightData.ambient =
        vector3(ambient[0].get<float>(), ambient[1].get<float>(),
                ambient[2].get<float>());

    json diffuse = data["diffuse"];
    lightData.diffuse =
        vector3(diffuse[0].get<float>(), diffuse[1].get<float>(),
                diffuse[2].get<float>());

    json specular = data["specular"];
    lightData.specular =
        vector3(specular[0].get<float>(), specular[1].get<float>(),
                specular[2].get<float>());

    light.AddComponent<TransformComponent>(
        Scene::get()->GetWorldNode(),
        vector3(position[0].get<float>(), position[1].get<float>(),
                position[2].get<float>()),
        quaternion(), vector3());
    light.AddComponent<PointLightComponent>(lightData);
}

void Loader::ProcessDirectionalLight(json lightObject) {
    json directionData = lightObject["direction"];

    vector3 direction(directionData[0].get<float>(),
                      directionData[1].get<float>(),
                      directionData[2].get<float>());

    json ambientData = lightObject["ambient"];
    vector3 ambient =
        vector3(ambientData[0].get<float>(), ambientData[1].get<float>(),
                ambientData[2].get<float>());

    json diffuseData = lightObject["diffuse"];
    vector3 diffuse =
        vector3(diffuseData[0].get<float>(), diffuseData[1].get<float>(),
                diffuseData[2].get<float>());

    json specularData = lightObject["specular"];
    vector3 specular =
        vector3(specularData[0].get<float>(), specularData[1].get<float>(),
                specularData[2].get<float>());

    Graphics::get()->SetDirLight({direction, ambient, diffuse, specular});
}

void Loader::ProcessPlayer(json playerObject) {
    EntityId player = ECS::EntityManager::get()->CreateEntity();

    json positionData = playerObject["position"];
    vector3 position(positionData[0], positionData[1], positionData[2]);

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

Material* Loader::LoadMaterial(const aiMaterial* material) {
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

    return MaterialRegistry::get()->AddMaterial(std::move(mat));
}

Mesh Loader::LoadMesh(const aiMesh* mesh,
                      const unordered_map<string, int>& boneNames) {
    vector3 min(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());

    vector3 max(std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min());

    vector<Vertex> vert;
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
    for (int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];

        for (int j = 0; j < bone->mNumWeights; j++) {
            aiVertexWeight weight = bone->mWeights[j];
            string boneName = bone->mName.C_Str();

            Vertex& v = vert[weight.mVertexId];

            int weightIndex;
            for (weightIndex = 0;
                 weightIndex < 4 && v.boneIds[weightIndex] != -1;
                 weightIndex++) {
            }

            v.boneIds[weightIndex] = boneNames.at(boneName);
            v.boneWeights[weightIndex] = weight.mWeight;
        }
    }

    for (int j = 0; j < mesh->mNumFaces; j++) {
        const aiFace face = mesh->mFaces[j];
        ind.push_back(face.mIndices[0]);
        ind.push_back(face.mIndices[1]);
        ind.push_back(face.mIndices[2]);
    }

    ComPtr<ID3D11Buffer> vbuf = Graphics::get()->CreateVertexBuffer(
        vert.size() * sizeof(Vertex), false, false, vert.data());
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
    string path = m_currentPath + "\\Assets\\" + filename;

    const auto iter = m_modelRegistry.find(path);
    if (iter != m_modelRegistry.end()) {
        return &iter->second;
    }

    const aiScene* scene = importer.ReadFile(
        path.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    map<aiNode*, bool> nodeNecessity;
    unordered_map<string, aiNode*> nodeNames;
    queue<aiNode*> nodeQueue;
    nodeQueue.push(scene->mRootNode);

    for (aiNode* node = nodeQueue.front(); !nodeQueue.empty();
         node = nodeQueue.front(), nodeQueue.pop()) {
        for (int i = 0; i < node->mNumChildren; i++) {
            nodeQueue.push(node->mChildren[i]);
        }
        nodeNecessity.emplace(node, false);
        nodeNames.emplace(node->mName.C_Str(), node);
    }

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[i];

            string boneName(bone->mName.C_Str());
            aiNode* boneNode = nodeNames.find(boneName)->second;

            nodeNecessity[boneNode] = true;
            while (boneNode->mParent) {
                nodeNecessity[boneNode->mParent] = true;
            }
        }
    }

    Bone rootBone;
    rootBone.parentId = 0;
    vector<Bone> bones = {rootBone};
    map<aiNode*, int> boneIndices;
    unordered_map<string, int> boneNames;
    boneIndices.emplace(scene->mRootNode, 0);
    nodeQueue.push(scene->mRootNode);
    for (aiNode* node = nodeQueue.front(); !nodeQueue.empty();
         node = nodeQueue.front(), nodeQueue.pop()) {
        if (nodeNecessity[node] == true) {
            for (int i = 0; i < node->mNumChildren; i++) {
                nodeQueue.push(node->mChildren[i]);
            }
            Bone bone;
            bone.parentId = boneIndices[node->mParent];

            int boneIndex = bones.size();
            string boneName = node->mName.C_Str();
            boneIndices.emplace(node, boneIndex);
            boneNames.emplace(boneName, boneIndex);
            bones.push_back(bone);
        }
    }
    model.bones = move(bones);

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        model.meshes.push_back(LoadMesh(mesh, boneNames));
        for (int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];

            string boneName = bone->mName.C_Str();
            int boneIndex = boneNames[boneName];
            bones[boneIndex].offsetMatrix = bone->mOffsetMatrix;
        }
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