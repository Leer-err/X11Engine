#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Quaternion.h"
#include "Vector3.h"

struct PositionKey {
    float timestamp;
    Vector3 position;
};

struct RotationKey {
    float timestamp;
    Quaternion rotation;
};

struct ScalingKey {
    float timestamp;
    Vector3 scaling;
};

struct Animation {
    std::string name;
    float ticks_per_second;
    std::unordered_map<std::string, std::vector<PositionKey>> positions;
    std::unordered_map<std::string, std::vector<RotationKey>> rotations;
    std::unordered_map<std::string, std::vector<ScalingKey>> scalings;
};

#endif  // ANIMATION_H