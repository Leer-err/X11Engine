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

inline Quaternion operator*(const Quaternion& a, const Quaternion& b) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat4(&a.quat);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat4(&b.quat);
    Quaternion quat;

    DirectX::XMStoreFloat4(&quat.quat,
                           DirectX::XMQuaternionMultiply(a_loaded, b_loaded));

    return quat;
}

#endif