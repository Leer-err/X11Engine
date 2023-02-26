#pragma once

#include <string>
#include <vector>

#include "Types/Quaternion.h"
#include "Types/Vector3.h"

using std::string;
using std::vector;

struct KeyPosition {
    KeyPosition(const vector3& position, float timeStamp)
        : position(position), timeStamp(timeStamp) {}
    vector3 position;
    float timeStamp;
};

struct KeyRotation {
    KeyRotation(const quaternion& rotation, float timeStamp)
        : rotation(rotation), timeStamp(timeStamp) {}
    quaternion rotation;
    float timeStamp;
};

struct KeyScale {
    KeyScale(const vector3& scale, float timeStamp)
        : scale(scale), timeStamp(timeStamp) {}
    vector3 scale;
    float timeStamp;
};

struct BoneKeys {
    vector<KeyPosition> positions;
    vector<KeyRotation> rotations;
    vector<KeyScale> scalings;
};

struct Animation {
    string name;
    double m_duration;
    double m_ticksPerSecond;

    vector<BoneKeys> m_boneKeys;
};