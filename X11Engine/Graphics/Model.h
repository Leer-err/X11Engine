#pragma once
#include <vector>

#include "Material.h"
#include "Mesh.h"

using std::vector;

struct Model {
    vector<Mesh> meshes;
    vector<Material*> materials;

    AABB box;
};