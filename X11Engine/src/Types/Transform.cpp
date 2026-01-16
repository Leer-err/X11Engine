#include "Transform.h"

#include <format>

Transform::Transform() { scale = Vector3(1, 1, 1); }

void Transform::setPosition(const Vector3& position) {
    this->position = position;
    is_dirty = true;
}

void Transform::setOrientation(const Quaternion& orientation) {
    this->orientation = orientation;
    is_dirty = true;
}

void Transform::setScale(const Vector3& scale) {
    this->scale = scale;
    is_dirty = true;
}

Vector3 Transform::getPosition() { return position; }

Quaternion Transform::getOrientation() { return orientation; }

Vector3 Transform::getScale() { return scale; }

Vector3 Transform::getForward() { return Vector3(0, 0, 1).rotate(orientation); }

Vector3 Transform::getRight() { return Vector3(1, 0, 0).rotate(orientation); }

Vector3 Transform::getUp() { return Vector3(0, 1, 0).rotate(orientation); }

void Transform::markDirty() { is_dirty = true; }

void Transform::markClean() { is_dirty = false; }

bool Transform::isDirty() const { return is_dirty; }

std::string Transform::serialize() const {
    auto position_str = std::format("Position: ({}, {}, {})", position.x,
                                    position.y, position.z);
    auto orientation_str =
        std::format("Orientation: ({}, {}, {}, {})", orientation.w,
                    orientation.x, orientation.y, orientation.z);
    auto scale_str =
        std::format("Scale: ({}, {}, {})", scale.x, scale.y, scale.z);

    return std::format("{}\n{}\n{}", position_str, orientation_str, scale_str);
}
