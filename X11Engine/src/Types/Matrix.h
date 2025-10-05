#ifndef MATRIX_H
#define MATRIX_H

#include <DirectXMath.h>

#include <cstring>

#include "Quaternion.h"
#include "Vector3.h"

struct Matrix {
    DirectX::XMFLOAT4X4 matrix;

    static Matrix identity();

    static Matrix projection(float fov, float aspect_ratio, float far,
                             float near);

    static Matrix translation(const Vector3& pos) {
        DirectX::XMMATRIX mat =
            DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }
    static Matrix rotation(float roll, float pitch, float yaw) {
        DirectX::XMMATRIX mat =
            DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }
    static Matrix rotation(const Quaternion& q) {
        DirectX::XMVECTOR q_loaded = DirectX::XMLoadFloat4(&q.quat);

        DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(q_loaded);

        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }
    static Matrix scale(const Vector3& scale) {
        DirectX::XMMATRIX mat =
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }
    static Matrix view(const Vector3& position, const Vector3& forward,
                       const Vector3& up) {
        DirectX::XMVECTOR position_loaded =
            DirectX::XMLoadFloat3(&position.vec);
        DirectX::XMVECTOR forward_loaded = DirectX::XMLoadFloat3(&forward.vec);
        DirectX::XMVECTOR up_loaded = DirectX::XMLoadFloat3(&up.vec);

        DirectX::XMMATRIX mat = DirectX::XMMatrixLookToLH(
            position_loaded, forward_loaded, up_loaded);

        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }

    Matrix transpose() const {
        DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&matrix);

        mat = DirectX::XMMatrixTranspose(mat);
        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }

    Matrix inverse() const {
        DirectX::XMVECTOR determinant = {};
        DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&matrix);

        mat = DirectX::XMMatrixInverse(&determinant, mat);
        Matrix result = {};
        DirectX::XMStoreFloat4x4(&result.matrix, mat);
        return result;
    }

    Vector3 getTranslation() const {
        DirectX::XMVECTOR translation_vector = {};
        DirectX::XMVECTOR rotation_vector = {};
        DirectX::XMVECTOR scale_vector = {};
        DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&matrix);

        DirectX::XMMatrixDecompose(&scale_vector, &rotation_vector,
                                   &translation_vector, mat);

        DirectX::XMFLOAT3 translation;
        DirectX::XMStoreFloat3(&translation, translation_vector);

        return {translation.x, translation.y, translation.z};
    };

    Quaternion getRotation() const {
        DirectX::XMVECTOR translation_vector = {};
        DirectX::XMVECTOR rotation_vector = {};
        DirectX::XMVECTOR scale_vector = {};
        DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&matrix);

        DirectX::XMMatrixDecompose(&scale_vector, &rotation_vector,
                                   &translation_vector, mat);

        DirectX::XMFLOAT4 rotation;
        DirectX::XMStoreFloat4(&rotation, rotation_vector);

        return {rotation.w, rotation.x, rotation.y, rotation.z};
    };

    Vector3 getScale() const {
        DirectX::XMVECTOR translation_vector = {};
        DirectX::XMVECTOR rotation_vector = {};
        DirectX::XMVECTOR scale_vector = {};
        DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&matrix);

        DirectX::XMMatrixDecompose(&scale_vector, &rotation_vector,
                                   &translation_vector, mat);

        DirectX::XMFLOAT3 scale;
        DirectX::XMStoreFloat3(&scale, scale_vector);

        return {scale.x, scale.y, scale.z};
    };
};

inline Matrix operator*(const Matrix& a, const Matrix& b) {
    DirectX::XMMATRIX a_loaded = DirectX::XMLoadFloat4x4(&a.matrix);
    DirectX::XMMATRIX b_loaded = DirectX::XMLoadFloat4x4(&b.matrix);

    DirectX::XMMATRIX mat = DirectX::XMMatrixMultiply(a_loaded, b_loaded);
    Matrix result = {};
    DirectX::XMStoreFloat4x4(&result.matrix, mat);
    return result;
}

inline bool operator==(const Matrix& a, const Matrix& b) {
    return memcmp(&a, &b, sizeof(Matrix)) == 0;
}

#endif  // MATRIX_H