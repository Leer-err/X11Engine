#include "Transform.h"

#include <format>

#include "Matrix.h"

Transform::Transform() { scale = Vector3(1, 1, 1); }

void Transform::setPosition(const Vector3& position) {
    this->position = position;
    dirty_matrix = true;
}

void Transform::setOrientation(const Quaternion& orientation) {
    this->orientation = orientation;
    dirty_matrix = true;
}

void Transform::setScale(const Vector3& scale) {
    this->scale = scale;
    dirty_matrix = true;
}

Vector3 Transform::getPosition() const { return position; }

Quaternion Transform::getOrientation() const { return orientation; }

Vector3 Transform::getScale() const { return scale; }

Matrix Transform::getLocalMatrix() {
    if (dirty_matrix == false) return local_matrix;

    Matrix translation = Matrix::translation(position);
    Matrix rotation = Matrix::rotation(orientation);
    Matrix scale_matrix = Matrix::scale(scale);

    local_matrix = scale_matrix * rotation * translation;
    dirty_matrix = false;

    return local_matrix;
}

Vector3 Transform::getForward() const {
    return Vector3(0, 0, 1).rotate(orientation);
}

Vector3 Transform::getRight() const {
    return Vector3(1, 0, 0).rotate(orientation);
}

Vector3 Transform::getUp() const {
    return Vector3(0, 1, 0).rotate(orientation);
}

bool Transform::isDirty() const { return dirty_matrix; }

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
