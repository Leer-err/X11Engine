#pragma once
#include <unordered_map>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <wincodec.h>
#include <filesystem>

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

	Model* LoadModelFromFile(const char* filename);
	ComPtr<ID3D11ShaderResourceView> LoadTextureFromFile(const char* filename);
	Material LoadMaterial(const aiMaterial* material);
private:
	Loader() : m_currentPath(std::filesystem::current_path().string()) {
		CoInitialize(NULL);

		CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_factory));
	}
	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
	Loader(const Loader&&) = delete;
	Loader& operator=(const Loader&&) = delete;

	unordered_map<string, Model> m_modelRegistry;
	unordered_map<string, ComPtr<ID3D11ShaderResourceView>> m_textureRegistry;
	string m_currentPath;
	ComPtr<IWICImagingFactory> m_factory;
};