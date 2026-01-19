#include "Matrix.h"

Matrix Matrix::identity() {
    DirectX::XMMATRIX mat = DirectX::XMMatrixIdentity();

    Matrix result = {};
    DirectX::XMStoreFloat4x4(&result.matrix, mat);
    return result;
}

Matrix Matrix::projection(float fov, float aspect_ratio, float far,
                          float near) {
    DirectX::XMMATRIX mat =
        DirectX::XMMatrixPerspectiveFovLH(1.04, aspect_ratio, near, far);

    Matrix result = {};
    DirectX::XMStoreFloat4x4(&result.matrix, mat);
    return result;
}