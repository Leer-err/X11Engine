#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Loader {
public:
	static Loader& get() {
		static Loader instance;
		return instance;
	}
private:
	Loader();

	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
};