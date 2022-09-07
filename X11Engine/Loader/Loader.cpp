#include "Loader.h"

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <d3dcompiler.h>

#include <fstream>
#include <memory>

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
#include "Logger/Logger.h"
#include "MaterialRegistry/MaterialRegistry.h"
#include "Window.h"

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

void Loader::LoadNode(const aiScene* scene, const aiNode* sceneNode,
                      Scene::Node* node) {
    Model model;

    for (int i = 0; i < sceneNode->mNumMeshes; i++) {
        int meshIndex = sceneNode->mMeshes[i];
        Mesh mesh = LoadMesh(scene->mMeshes[meshIndex]);
        model.meshes.emplace_back(mesh);
    }
    node->SetModel(model);

    for (int i = 0; i < sceneNode->mNumChildren; i++) {
        aiNode* childNode = sceneNode->mChildren[i];

        aiMatrix4x4 local = childNode->mTransformation;
        aiVector3D position, rotation, scale;
        local.Decompose(scale, rotation, position);

        Scene::Node* child =
            node->AddChild(position, quaternion(rotation), scale);

        LoadNode(scene, childNode, child);
    }
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

    aiNode* rootNode = scene->mRootNode;

    aiMatrix4x4 local = rootNode->mTransformation;
    aiVector3D position, rotation, scale;
    local.Decompose(scale, rotation, position);

    Scene::Node* child = Scene::get()->GetWorldNode()->AddChild(
        position, quaternion(rotation), scale);
    LoadNode(scene, rootNode, child);

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