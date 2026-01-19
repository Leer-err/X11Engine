#ifndef QUATERNION_H
#define QUATERNION_H

#include <DirectXMath.h>

struct Vector3;

struct Quaternion {
    union {
        DirectX::XMFLOAT4 quat;
        struct {
            float x, y, z, w;
        };
    };

    Quaternion();
    Quaternion(float pitch, float yaw, float roll);
    Quaternion(float w, float x, float y, float z);
    Quaternion(Vector3 vec);

    static Quaternion slerp(const Quaternion& a, const Quaternion& b,
                            float factor);
    Quaternion inverse() const;

    bool operator==(const Quaternion& b) const;
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);

#endif