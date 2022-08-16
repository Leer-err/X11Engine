#pragma once
#include <DirectXMath.h>

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

    inline matrix Transpose() const {
        return DirectX::XMMatrixTranspose(*this);
    }
    inline matrix Inverse() const {
        return DirectX::XMMatrixInverse(nullptr, *this);
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