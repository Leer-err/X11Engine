#include "Quaternion.h"

#include "Vector3.h"

Quaternion::Quaternion() {
    DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionIdentity());
}

Quaternion::Quaternion(float pitch, float yaw, float roll) {
    DirectX::XMStoreFloat4(
        &quat, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
}

Quaternion::Quaternion(float w, float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion::Quaternion(Vector3 vec) : Quaternion(vec.x, vec.y, vec.z) {}

Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b,
                             float factor) {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat4(&a.quat);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat4(&b.quat);

    DirectX::XMVECTOR result_loaded =
        DirectX::XMQuaternionSlerp(a_loaded, b_loaded, factor);

    DirectX::XMFLOAT4 result;
    DirectX::XMStoreFloat4(&result, result_loaded);
    return Quaternion(result.w, result.x, result.y, result.z);
}

Quaternion Quaternion::inverse() const {
    DirectX::XMVECTOR loaded = DirectX::XMLoadFloat4(&quat);

    DirectX::XMVECTOR result_loaded = DirectX::XMQuaternionInverse(loaded);

    DirectX::XMFLOAT4 result;
    DirectX::XMStoreFloat4(&result, result_loaded);
    return Quaternion(result.w, result.x, result.y, result.z);
}

bool Quaternion::operator==(const Quaternion& b) const {
    DirectX::XMVECTOR a_loaded = DirectX::XMLoadFloat4(&quat);
    DirectX::XMVECTOR b_loaded = DirectX::XMLoadFloat4(&b.quat);
    return DirectX::XMQuaternionEqual(a_loaded, b_loaded);
}