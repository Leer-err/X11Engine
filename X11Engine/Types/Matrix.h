#pragma once
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>
#include <memory.h>

#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

struct matrix {
    inline matrix() { memset(this, 0, sizeof(matrix)); }
    inline matrix(const vector4& a, const vector4& b, const vector4& c,
                  const vector4& d)
        : rows{a, b, c, d} {}
    inline matrix(const DirectX::XMMATRIX& matr) {
        DirectX::XMStoreFloat4x4(&this->matr, matr);
    }
    inline matrix(const aiMatrix4x4& matr) {
        memcpy(this->field[0], &matr.a1, sizeof(vector4));
        memcpy(this->field[1], &matr.b1, sizeof(vector4));
        memcpy(this->field[2], &matr.c1, sizeof(vector4));
        memcpy(this->field[3], &matr.d1, sizeof(vector4));
    }

    inline matrix Transpose() const {
        return DirectX::XMMatrixTranspose(*this);
    }
    inline matrix Inverse() const {
        return DirectX::XMMatrixInverse(nullptr, *this);
    }

    inline vector3 Translation() {
        DirectX::XMVECTOR scale, rotation, translation;
        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, *this);
        return translation;
    }
    inline quaternion Rotation() {
        DirectX::XMVECTOR scale, rotation, translation;
        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, *this);
        return rotation;
    }
    inline vector3 Scale() {
        DirectX::XMVECTOR scale, rotation, translation;
        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, *this);
        return scale;
    }
    inline matrix ChangeUnit(float factor) {
        DirectX::XMVECTOR scale, rotation, translation;

        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, *this);
        translation = DirectX::XMVectorDivide(
            translation, DirectX::XMVectorSet(factor, factor, factor, 1.0f));

        auto result = DirectX::XMMatrixScalingFromVector(scale);
        result = DirectX::XMMatrixMultiply(
            result, DirectX::XMMatrixRotationQuaternion(rotation));
        result = DirectX::XMMatrixMultiply(
            result, DirectX::XMMatrixTranslationFromVector(translation));

        return result;
    }

    inline matrix __vectorcall operator+(const matrix& other) const {
        return DirectX::XMMATRIX(*this) + DirectX::XMMATRIX(other);
    }
    inline matrix __vectorcall operator-(const matrix& other) const {
        return DirectX::XMMATRIX(*this) - DirectX::XMMATRIX(other);
    }

    matrix __vectorcall operator*(const matrix& m) const {
        return DirectX::XMMatrixMultiply(*this, m);
    }

    inline operator DirectX::XMMATRIX() const {
        return DirectX::XMLoadFloat4x4(&this->matr);
    }

    union {
        vector4 rows[4];
        float field[4][4];
        DirectX::XMFLOAT4X4 matr;
    };
};

inline matrix PerspectiveProjectionMatrix(const float aspectRatio,
                                          const float fov, const float farZ,
                                          const float nearZ) {
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

inline matrix __vectorcall LookToMatrix(const vector3& eyePosition,
                                        const vector3& eyeDirection,
                                        const vector3& upDirection) {
    return DirectX::XMMatrixLookToLH(eyePosition, eyeDirection, upDirection);
}

inline matrix __vectorcall RotationMatrix(const quaternion& q) {
    return DirectX::XMMatrixRotationQuaternion(q);
}

inline matrix __vectorcall TranslationMatrix(const vector3& offset) {
    return DirectX::XMMatrixTranslationFromVector(offset);
}

inline matrix __vectorcall ScalingMatrix(const vector3& scale) {
    return DirectX::XMMatrixScalingFromVector(scale);
}

inline matrix __vectorcall IdentityMatrix() {
    return DirectX::XMMatrixIdentity();
}