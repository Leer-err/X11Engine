#include "Vector4.h"

#include "glm/ext/matrix_common.hpp"

using namespace glm;

static const vec4& get(const Vector4& vec) {
    return reinterpret_cast<const vec4&>(vec);
}
static vec4& get(Vector4& vec) { return reinterpret_cast<vec4&>(vec); }

Vector4 Vector4::lerp(const Vector4& a, const Vector4& b, float factor) {
    auto result = glm::mix(get(a), get(b), factor);

    return Vector4(result.x, result.y, result.z, result.w);
}