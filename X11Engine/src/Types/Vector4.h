#ifndef VECTOR_4_H
#define VECTOR_4_H

struct Vector4 {
    float x, y, z, w;
};

inline float dot(const Vector4& a, const Vector4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

#endif