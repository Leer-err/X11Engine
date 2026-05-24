#include "Vector3.h"

#include <foundation/PxVec3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Quaternion.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

using namespace glm;

static const vec3& get(const Vector3& vec) {
    return reinterpret_cast<const vec3&>(vec);
}
static vec3& get(Vector3& vec) { return reinterpret_cast<vec3&>(vec); }

Vector3::Vector3(const physx::PxVec3& vector)
    : Vector3(vector.x, vector.y, vector.z) {}

Vector3 Vector3::lerp(const Vector3& a, const Vector3& b, float factor) {
    auto result = glm::mix(get(a), get(b), factor);

    return Vector3(result.x, result.y, result.z);
}

Vector3 Vector3::normalized() const {
    auto result = glm::normalize(get(*this));
    return Vector3(result.x, result.y, result.z);
}

Vector3 Vector3::rotate(const Quaternion& rotation) const {
    auto& q = reinterpret_cast<const glm::quat&>(rotation);
    auto result = q * get(*this);
    return Vector3(result.x, result.y, result.z);
}

float Vector3::length() const { return glm::length(get(*this)); }

Vector3::operator physx::PxVec3() const { return physx::PxVec3(x, y, z); }

Vector3 operator+(const Vector3& a, const Vector3& b) {
    auto result = get(a) + get(b);
    return Vector3(result.x, result.y, result.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b) {
    auto result = get(a) - get(b);
    return Vector3(result.x, result.y, result.z);
}

Vector3 operator*(const Vector3& a, float b) {
    auto result = get(a) * b;
    return Vector3(result.x, result.y, result.z);
}

Vector3 operator/(const Vector3& a, float b) {
    auto result = get(a) / b;
    return Vector3(result.x, result.y, result.z);
}

bool operator==(const Vector3& a, const Vector3& b) { return get(a) == get(b); }
