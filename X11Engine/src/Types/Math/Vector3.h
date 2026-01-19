#ifndef VECTOR_3_H
#define VECTOR_3_H

#include <DirectXMath.h>

struct Quaternion;

struct Vector3 {
    constexpr Vector3();

    constexpr Vector3(float x, float y, float z);

    static Vector3 lerp(const Vector3& a, const Vector3& b, float factor);

    Vector3 normalized() const;

    Vector3 rotate(Quaternion quat) const;

    float length() const;

    union {
        struct {
            float x, y, z;
        };
        DirectX::XMFLOAT3 vec;
    };
};

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, float b);
Vector3 operator/(const Vector3& a, float b);

#endif