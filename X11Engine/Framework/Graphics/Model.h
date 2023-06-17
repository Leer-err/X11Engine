#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Types/Matrix.h"
#include "Animation.h"
#include "Material.h"
#include "Mesh.h"

using std::string;
using std::unordered_map;
using std::vector;

constexpr int INVALID_PARENT = -1;

struct Skeleton {
    vector<matrix> offsetMatrices;
    vector<int> parents;
    unordered_map<string, int> boneNames;
};

struct Model {
    inline const Animation* GetAnimation(const string& name) const {
        int index = animationNames.at(name);
        return &animations.at(index);
    }

    float scalingFactor;

    Skeleton skeleton;

    vector<int> materials;
    vector<Mesh> meshes;

    vector<Animation> animations;
    unordered_map<string, int> animationNames;
};