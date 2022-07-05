#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <wincodec.h>
#include <memory>

#include "Graphics/Texture.h"

using std::unique_ptr;

class Loader {
public:
	static Loader& get() {
		static Loader instance;
		return instance;
	}

	Texture LoadTextureFromFile(const wchar_t* filename);
	
private:
	Loader() = default;

	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
};