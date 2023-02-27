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

    inline vector3 GetPositionKey(int boneId, float time) const {
        int keyIndex = time * m_ticksPerSecond;

        if (m_boneKeys[boneId].positions.size() > keyIndex) {
            return m_boneKeys[boneId].positions[keyIndex].position;
        } else {
            return vector3();
        }
    }
    inline quaternion GetRotationKey(int boneId, float time) const {
        int keyIndex = time * m_ticksPerSecond;

        if (m_boneKeys[boneId].rotations.size() > keyIndex) {
            return m_boneKeys[boneId].rotations[keyIndex].rotation;
        } else {
            return quaternion();
        }
    }
    inline vector3 GetScaleKey(int boneId, float time) const {
        int keyIndex = time * m_ticksPerSecond;

        if (m_boneKeys[boneId].scalings.size() > keyIndex) {
            return m_boneKeys[boneId].scalings[keyIndex].scale;
        } else {
            return vector3(1.f, 1.f, 1.f);
        }
    }

    inline bool IsBoneAnimated(int boneId) {
        return m_boneKeys[boneId].positions.size() != 0 &&
               m_boneKeys[boneId].rotations.size() != 0 &&
               m_boneKeys[boneId].scalings.size() != 0;
    }
};