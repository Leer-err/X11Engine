#ifndef VECTOR_4_H
#define VECTOR_4_H

struct Vector4 {
    float x;
    float y;
    float z;
    float w;

    static Vector4 lerp(const Vector4& a, const Vector4& b, float factor);
};

inline float dot(const Vector4& a, const Vector4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

#endif