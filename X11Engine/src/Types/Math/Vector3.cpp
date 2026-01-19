#include "Vector3.h"

#include <DirectXMath.h>

#include "Quaternion.h"

constexpr Vector3::Vector3() : Vector3(0, 0, 0) {}

constexpr Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3 Vector3::lerp(const Vector3& a, const Vector3& b, float factor) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat3(&a.vec);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat3(&b.vec);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMVectorLerp(a_loaded, b_loaded, factor);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Vector3::normalized() const {
    DirectX::XMVECTOR vec_loaded = DirectX::XMLoadFloat3(&vec);

    DirectX::XMVECTOR result_loaded = DirectX::XMVector3Normalize(vec_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}

Vector3 Vector3::rotate(Quaternion quat) const {
    DirectX::XMVECTOR vec_loaded = DirectX::XMLoadFloat3(&vec);
    DirectX::XMVECTOR quat_loaded = DirectX::XMLoadFloat4(&quat.quat);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMVector3Rotate(vec_loaded, quat_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}

float Vector3::length() const {
    DirectX::XMVECTOR vec_loaded = DirectX::XMLoadFloat3(&vec);
    DirectX::XMVECTOR result_loaded = DirectX::XMVector3Length(vec_loaded);

    float result;
    DirectX::XMStoreFloat(&result, result_loaded);
    return result;
}

inline Vector3 operator+(const Vector3& a, const Vector3& b) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat3(&a.vec);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat3(&b.vec);

    DirectX::XMVECTOR result_loaded = DirectX::XMVectorAdd(a_loaded, b_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat3(&a.vec);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat3(&b.vec);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMVectorSubtract(a_loaded, b_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}

inline Vector3 operator*(const Vector3& a, float b) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat3(&a.vec);

    DirectX::XMVECTOR b_loaded = DirectX::XMVectorSet(b, b, b, b);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMVectorMultiply(a_loaded, b_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}

inline Vector3 operator/(const Vector3& a, float b) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat3(&a.vec);

    DirectX::XMVECTOR b_loaded = DirectX::XMVectorSet(b, b, b, b);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMVectorDivide(a_loaded, b_loaded);

    Vector3 result;
    DirectX::XMStoreFloat3(&result.vec, result_loaded);
    return result;
}