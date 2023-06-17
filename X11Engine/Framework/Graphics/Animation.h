#pragma once

#include <string>
#include <vector>

#include "Framework/Types/Quaternion.h"
#include "Framework/Types/Vector3.h"

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
            double timeNext =
                m_boneKeys[boneId].positions[keyIndex + 1].timeStamp;
            double timePrev = m_boneKeys[boneId].positions[keyIndex].timeStamp;

            vector3 posNext =
                m_boneKeys[boneId].positions[keyIndex + 1].position;
            vector3 posPrev = m_boneKeys[boneId].positions[keyIndex].position;

            double delta = timeNext - timePrev;

            float t = (time * m_ticksPerSecond - timePrev) / delta;

            return vector3::lerp(posPrev, posNext, t);
        } else {
            return vector3();
        }
    }

    inline quaternion GetRotationKey(int boneId, float time) const {
        int keyIndex = time * m_ticksPerSecond;

        if (m_boneKeys[boneId].rotations.size() > keyIndex) {
            double timeNext =
                m_boneKeys[boneId].rotations[keyIndex + 1].timeStamp;
            double timePrev = m_boneKeys[boneId].rotations[keyIndex].timeStamp;

            quaternion rotNext =
                m_boneKeys[boneId].rotations[keyIndex + 1].rotation;
            quaternion rotPrev =
                m_boneKeys[boneId].rotations[keyIndex].rotation;

            double delta = timeNext - timePrev;

            float t = (time * m_ticksPerSecond - timePrev) / delta;

            return quaternion::slerp(rotPrev, rotNext, t);
        } else {
            return quaternion();
        }
    }

    inline vector3 GetScaleKey(int boneId, float time) const {
        int keyIndex = time * m_ticksPerSecond;

        if (m_boneKeys[boneId].scalings.size() > keyIndex) {
            double timeNext =
                m_boneKeys[boneId].scalings[keyIndex + 1].timeStamp;
            double timePrev = m_boneKeys[boneId].scalings[keyIndex].timeStamp;

            vector3 scaleNext = m_boneKeys[boneId].scalings[keyIndex + 1].scale;
            vector3 scalePrev = m_boneKeys[boneId].scalings[keyIndex].scale;

            double delta = timeNext - timePrev;

            float t = (time * m_ticksPerSecond - timePrev) / delta;

            return vector3::lerp(scalePrev, scaleNext, t);
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