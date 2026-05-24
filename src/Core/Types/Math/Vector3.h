#ifndef VECTOR_3_H
#define VECTOR_3_H

namespace physx {
template <typename T>
class PxVec3T;

typedef PxVec3T<float> PxVec3;
}  // namespace physx

struct Quaternion;

struct Vector3 {
    constexpr Vector3() : Vector3(0, 0, 0) {}
    constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const physx::PxVec3& vector);

    operator physx::PxVec3() const;

    static Vector3 lerp(const Vector3& a, const Vector3& b, float factor);

    Vector3 normalized() const;

    Vector3 rotate(const Quaternion& quat) const;

    float length() const;

    float x, y, z;
};

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, float b);
Vector3 operator/(const Vector3& a, float b);
bool operator==(const Vector3& a, const Vector3& b);

#endif