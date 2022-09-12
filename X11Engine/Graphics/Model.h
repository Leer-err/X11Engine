#pragma once
#include <vector>

#include "Bone.h"
#include "Material.h"
#include "Mesh.h"

using std::vector;

struct Model {
    vector<Mesh> meshes;
    vector<Material*> materials;
    vector<Bone> bones;

    AABB box;
};