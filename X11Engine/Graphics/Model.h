#pragma once
#include <vector>
#include "Mesh.h"
#include "Material.h"

using std::vector;

struct Model {
	vector<Mesh> meshes;
	vector<Material> materials;
};