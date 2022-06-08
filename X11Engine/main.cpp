#include "Window.h"
#include <thread>
#include <atomic>
#include "Graphics/Graphics.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Components/MeshComponent.h"
#include "Components/PositionComponent.h"
#include "Components/CameraComponent.h"
#include "Entities/Cube.h"
#include "TaskManager/TaskManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using std::thread;

std::atomic<bool> IsRunning = true;

void Update(Graphics& g, CameraComponent* camera) {
	while (IsRunning) {
		vector<future<void>> completed_tasks;
		g.PreFrame(camera);
		for (auto mesh = ECS::ComponentManager::Get()->begin<MeshComponent>(); mesh != ECS::ComponentManager::Get()->end<MeshComponent>(); ++mesh) {
			EntityId entity = mesh->GetOwner();
			const PositionComponent* pos = ECS::ComponentManager::Get()->GetComponent<PositionComponent>(entity);
			matrix model = TranslationMatrix(pos->position)/* * RotationMatrix(q)*/;
			completed_tasks.emplace_back(TaskManager::get().submit(&Graphics::Draw, &g, std::ref(mesh->vertices), std::ref(mesh->indices), std::ref(model), camera));
		}
		for (const auto& task : completed_tasks) {
			task.wait();
		}
		g.PostFrame();
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	vector<vertex> vert;
	vector<uint32_t> ind;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("Minecraft_Axolotl.fbx",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded);
	aiMesh* pMesh = scene->mMeshes[0];
	for (int i = 0; i < pMesh->mNumVertices; i++) {
		const aiVector3D pos = pMesh->mVertices[i];
		const aiVector3D uv = pMesh->mTextureCoords[0][i];
		vert.emplace_back(vector3{ (float)pos.x, (float)pos.y, (float)pos.z }, vector2{ (float)uv.x, (float)uv.y });
	}
	for (int i = 0; i < pMesh->mNumFaces; i++) {
		const aiFace face = pMesh->mFaces[i];
		ind.push_back(face.mIndices[0]);
		ind.push_back(face.mIndices[1]);
		ind.push_back(face.mIndices[2]);
	}

	vert = {
		{{0.5f, 0.5f, 0.5f}, {0.f, 0.f}},
		{{-0.5f, 0.5f, 0.5f}, {0.f, 0.f}},
		{{0.5f, -0.5f, 0.5f}, {0.f, 0.f}},
		{{-0.5f, -0.5f, 0.5f}, {0.f, 0.f}}
	};
	ind = {
		0, 2, 1,
		1, 2, 3
	};


	EntityId a = ECS::EntityManager::Get()->CreateEntity<Cube>();
	EntityId camera = ECS::EntityManager::Get()->CreateEntity<Cube>();
	PositionComponent* b = ECS::ComponentManager::Get()->AddComponent<PositionComponent>(a, vector3( -13.0f, 0.0f, 8.0f ));
	PositionComponent* cameraPos = ECS::ComponentManager::Get()->AddComponent<PositionComponent>(camera, vector3( 0.0f, 0.0f, 0.0f ));
	CameraComponent* cameraComponent = ECS::ComponentManager::Get()->AddComponent<CameraComponent>(camera, vector3(0.f, 0.f, 1.f), cameraPos);

	ECS::ComponentManager::Get()->AddComponent<MeshComponent>(a, vert, ind);

	Window::Get();
	//thread th(&Window::Run, &Window::Get());
	Graphics g{ 1920, 1080, Window::Get().GetHandle() };
	thread th2(Update, std::ref(g), cameraComponent);
	Window::Get().Run();
	IsRunning = false;
	th2.join();
	//th.join();
	return 0;
}
