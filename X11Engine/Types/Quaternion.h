#pragma once
#include <immintrin.h>
#include <DirectXMath.h>
#include "Vector4.h"
#include "Vector3.h"

struct quaternion {
    inline quaternion() : x(0.f), y(0.f), z(0.f), w(1.f) {}
    inline quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    inline quaternion(float pitch, float yaw, float roll) { DirectX::XMStoreFloat4(&this->vec, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)); }
    inline quaternion(vector3 angles) { DirectX::XMStoreFloat4(&this->vec, DirectX::XMQuaternionRotationRollPitchYawFromVector(angles)); }
    inline quaternion(const DirectX::XMVECTOR vec) { DirectX::XMStoreFloat4(&this->vec, vec); }

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

    inline operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(&this->vec); }

	union {
		struct { float x, y, z, w; };
        DirectX::XMFLOAT4 vec;
	};
};