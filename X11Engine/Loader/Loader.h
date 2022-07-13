#pragma once
#include <unordered_map>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <wincodec.h>

#include "Graphics/Model.h"
#include "Graphics/Graphics.h"

using std::unordered_map;
using std::string;

class Loader {
public:
	static Loader& get() {
		static Loader instance;
		return instance;
	}

	Model LoadModelFromFile(const char* filename);
	ComPtr<ID3D11Texture2D> LoadTextureFromFile(const char* filename);
	Material LoadMaterial(const aiMaterial* material);
private:
	Loader() = default;
	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
	Loader(const Loader&&) = delete;
	Loader& operator=(const Loader&&) = delete;

	unordered_map<string, Model> m_modelRegistry;
};