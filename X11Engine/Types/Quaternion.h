#pragma once
#include <DirectXMath.h>
#include <assimp/quaternion.h>
#include <immintrin.h>
#include <memory.h>

#include "Vector3.h"
#include "Vector4.h"

struct quaternion {
    inline quaternion() : w(1.f) { memset(&x, 0, sizeof(float) * 3); }
    inline quaternion(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {}
    inline quaternion(float pitch, float yaw, float roll) {
        DirectX::XMStoreFloat4(
            &this->vec,
            DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
    }
    inline quaternion(const vector3& angles) {
        DirectX::XMStoreFloat4(
            &this->vec,
            DirectX::XMQuaternionRotationRollPitchYawFromVector(angles));
    }
    inline quaternion(const DirectX::XMVECTOR& vec) {
        DirectX::XMStoreFloat4(&this->vec, vec);
    }
    inline quaternion(const aiQuaternion& quat) {
        memcpy(this, &quat, sizeof(*this));
    }

    inline quaternion inverse() const {
        return DirectX::XMQuaternionInverse(*this);
    }

    inline quaternion(const quaternion&) = default;
    inline quaternion& operator=(const quaternion&) = default;
    inline quaternion(quaternion&&) = default;
    inline quaternion& operator=(quaternion&&) = default;

    inline quaternion __vectorcall operator*(const quaternion& other) {
        return DirectX::XMQuaternionMultiply(*this, other);
    }

    inline quaternion& __vectorcall operator*=(const quaternion& other) {
        *this = DirectX::XMQuaternionMultiply(*this, other);
    }

    inline operator DirectX::XMVECTOR() const {
        return DirectX::XMLoadFloat4(&this->vec);
    }

    union {
        struct {
            float x, y, z, w;
        };
        DirectX::XMFLOAT4 vec;
    };
};