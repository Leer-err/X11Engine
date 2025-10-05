#include "AssimpAnimationReader.h"

#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <optional>
#include <vector>

std::vector<Animation> AssimpAnimationReader::readAll(
    const std::string& filename) const {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename.c_str(), aiProcess_MakeLeftHanded | aiProcess_OptimizeGraph);

    std::vector<Animation> animations;

    for (int i = 0; i < scene->mNumAnimations; i++) {
        const aiAnimation* animation = scene->mAnimations[i];

        animations.push_back(read(animation));
    }

    return animations;
}

std::optional<Animation> AssimpAnimationReader::read(
    const std::string& filename, const std::string& name) const {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename.c_str(), aiProcess_MakeLeftHanded | aiProcess_OptimizeGraph);

    for (int i = 0; i < scene->mNumAnimations; i++) {
        const aiAnimation* animation = scene->mAnimations[i];

        std::string found_name = animation->mName.C_Str();

        if (found_name == name) {
            return read(animation);
        }
    }

    return std::nullopt;
}

Animation AssimpAnimationReader::read(const aiAnimation* animation) const {
    Animation result;
    result.ticks_per_second = animation->mTicksPerSecond;
    result.name = animation->mName.C_Str();

    for (int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* key = animation->mChannels[i];
        std::string bone_name = key->mNodeName.C_Str();

        std::vector<PositionKey> position_keys;
        for (int j = 0; j < key->mNumPositionKeys; j++) {
            PositionKey position = {};
            position.timestamp = key->mPositionKeys[j].mTime;

            auto value = key->mPositionKeys[j].mValue;
            position.position = {value.x / 100, value.y / 100, value.z / 100};

            position_keys.push_back(position);
        }
        result.positions[bone_name] = position_keys;

        std::vector<RotationKey> rotation_keys;
        for (int j = 0; j < key->mNumRotationKeys; j++) {
            RotationKey rotation = {};
            rotation.timestamp = key->mRotationKeys[j].mTime;

            auto value = key->mRotationKeys[j].mValue;
            rotation.rotation = {value.w, value.x, value.y, value.z};

            rotation_keys.push_back(rotation);
        }
        result.rotations[bone_name] = rotation_keys;

        std::vector<ScalingKey> scaling_keys;
        for (int j = 0; j < key->mNumScalingKeys; j++) {
            ScalingKey scaling = {};
            scaling.timestamp = key->mScalingKeys[j].mTime;

            auto value = key->mScalingKeys[j].mValue;
            scaling.scaling = {value.x, value.y, value.z};

            scaling_keys.push_back(scaling);
        }
        result.scalings[bone_name] = scaling_keys;
    }

    return result;
}
