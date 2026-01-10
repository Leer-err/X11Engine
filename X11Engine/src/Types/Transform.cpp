#include "Transform.h"

#include <format>

#include "Matrix.h"

Transform::Transform() {
    parent_matrix = Matrix::identity();
    local_scale = Vector3(1, 1, 1);
    scale = local_scale;
}

void Transform::setLocalPosition(const Vector3& position) {
    local_position = position;
    dirty_globals = true;
    dirty_local_matrix = true;
}

void Transform::setLocalOrientation(const Quaternion& orientation) {
    local_orientation = orientation;
    dirty_globals = true;
    dirty_local_matrix = true;
}

void Transform::setLocalScale(const Vector3& scale) {
    local_scale = scale;
    dirty_globals = true;
    dirty_local_matrix = true;
}

void Transform::setParentTransformMatrix(const Matrix& parent_global_matrix) {
    parent_matrix = parent_global_matrix;
    dirty_globals = true;
}

Vector3 Transform::getPosition() {
    if (dirty_globals) updateGlobals();
    return position;
}

Quaternion Transform::getOrientation() {
    if (dirty_globals) updateGlobals();
    return orientation;
}

Vector3 Transform::getScale() {
    if (dirty_globals) updateGlobals();
    return scale;
}

Vector3 Transform::getLocalPosition() { return local_position; }

Quaternion Transform::getLocalOrientation() { return local_orientation; }

Vector3 Transform::getLocalScale() { return local_scale; }

Matrix Transform::getLocalMatrix() {
    if (dirty_local_matrix) updateLocalMatrix();

    return local_matrix;
}

Matrix Transform::getGlobalMatrix() {
    if (dirty_globals) updateGlobals();

    return global_matrix;
}

Vector3 Transform::getForward() { return Vector3(0, 0, 1).rotate(orientation); }

Vector3 Transform::getRight() { return Vector3(1, 0, 0).rotate(orientation); }

Vector3 Transform::getUp() { return Vector3(0, 1, 0).rotate(orientation); }

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

void Transform::updateLocalMatrix() {
    Matrix translation = Matrix::translation(local_position);
    Matrix rotation = Matrix::rotation(local_orientation);
    Matrix scale_matrix = Matrix::scale(local_scale);

    local_matrix = scale_matrix * rotation * translation;

    dirty_local_matrix = false;
}

void Transform::updateGlobals() {
    if (dirty_local_matrix) updateLocalMatrix();

    global_matrix = parent_matrix * local_matrix;

    global_matrix.decompose(position, scale, orientation);

    dirty_globals = false;
}
