#pragma once
// #include <PxPhysicsAPI.h>
#include <DirectXMath.h>
#include <assimp/vector3.h>
#include <memory.h>

#include <nlohmann/json.hpp>

struct vector3 {
    inline vector3() { memset(this, 0, sizeof(vector3)); }
    inline vector3(const DirectX::XMVECTOR& vec) {
        DirectX::XMStoreFloat3(&this->vec, vec);
    }
    inline vector3(const aiVector3D& vec) {
        memcpy(this, &vec, sizeof(vector3));
    }
    inline constexpr vector3(const float x, const float y, const float z)
        : x(x), y(y), z(z) {}

    inline float length() const {
        return DirectX::XMVector3Length(*this).m128_f32[0];
    }
    inline vector3 normalized() const {
        return DirectX::XMVector3Normalize(*this);
    }
    inline vector3 negate() const { return DirectX::XMVectorNegate(*this); }
    inline vector3 rotate(float pitch, float yaw, float roll) const {
        DirectX::XMVECTOR rot =
            DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
        return DirectX::XMVector3Rotate(*this, rot);
    }
    inline vector3 rotate(const DirectX::XMVECTOR rot) const {
        return DirectX::XMVector3Rotate(*this, rot);
    }

    inline vector3 __vectorcall operator-() const {
        return DirectX::XMVectorNegate(*this);
    }

    inline vector3 __vectorcall operator+(const vector3& other) const {
        return DirectX::XMVectorAdd(*this, other);
    }
    inline vector3 __vectorcall operator-(const vector3& other) const {
        return DirectX::XMVectorSubtract(*this, other);
    }

    inline vector3& __vectorcall operator+=(const vector3& other) {
        return *this = DirectX::XMVectorAdd(*this, other);
    }
    inline vector3& __vectorcall operator-=(const vector3& other) {
        return *this = DirectX::XMVectorSubtract(*this, other);
    }

    inline vector3 __vectorcall operator*(const float& value) const {
        return DirectX::XMVectorMultiply(*this,
                                         DirectX::XMVectorReplicate(value));
    }
    inline vector3 __vectorcall operator/(const float& value) const {
        return DirectX::XMVectorDivide(*this,
                                       DirectX::XMVectorReplicate(value));
    }

    inline vector3& __vectorcall operator*=(const float& value) {
        *this =
            DirectX::XMVectorMultiply(*this, DirectX::XMVectorReplicate(value));
        return *this;
    }
    inline vector3& __vectorcall operator/=(const float& value) {
        *this =
            DirectX::XMVectorDivide(*this, DirectX::XMVectorReplicate(value));
        return *this;
    }

    inline operator DirectX::XMVECTOR() const {
        return DirectX::XMLoadFloat3(&this->vec);
    }

    inline bool __vectorcall operator==(const vector3& other) const {
        return DirectX::XMVector3Equal(*this, other);
    }
    inline bool __vectorcall operator!=(const vector3& other) const {
        return !DirectX::XMVector3Equal(*this, other);
    }

    vector3 __vectorcall operator*(const DirectX::XMMATRIX& m) const {
        DirectX::XMVECTOR vec = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)this);
        return DirectX::XMVector4Transform(vec, m);
    }

    inline vector3 __vectorcall transform(const DirectX::XMMATRIX& m) const {
        return DirectX::XMVector3Transform(*this, m);
    }

    static inline vector3 min() {
        const vector3 min = {std::numeric_limits<float>::min(),
                             std::numeric_limits<float>::min(),
                             std::numeric_limits<float>::min()};
        return min;
    }
    static inline vector3 max() {
        const vector3 max = {std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max()};
        return max;
    }
    static inline vector3 forward() {
        const vector3 forward = {0.f, 0.f, 1.f};
        return forward;
    }
    static inline vector3 up() {
        const vector3 up = {0.f, 1.f, 0.f};
        return up;
    }
    static inline vector3 right() {
        const vector3 right = {1.f, 0.f, 0.f};
        return right;
    }

    static inline vector3 __vectorcall lerp(const vector3& a, const vector3& b,
                                            float t) {
        return DirectX::XMVectorLerp(a, b, t);
    }

    union {
        struct {
            float x, y, z;
        };
        DirectX::XMFLOAT3 vec;
    };
};

inline void to_json(nlohmann::json& j, const vector3& vec) {
    j = nlohmann::json::array({vec.x, vec.y, vec.z});
}

inline void from_json(const nlohmann::json& j, vector3& vec) {
    j.at(0).get_to(vec.x);
    j.at(1).get_to(vec.y);
    j.at(2).get_to(vec.z);
}

inline float __vectorcall dot(const vector3& a, const vector3& b) {
    return DirectX::XMVector3Dot(a, b).m128_f32[0];
}

inline vector3 __vectorcall cross(const vector3& a, const vector3& b) {
    return DirectX::XMVector3Cross(a, b);
}