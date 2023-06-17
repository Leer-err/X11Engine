#include "Transform.h"

void Transform::CalcWorldMatrix(const matrix& parentMatrix) {
    matrix localMatrix = ScalingMatrix(scale) * RotationMatrix(rotation) *
                         TranslationMatrix(position);
    worldMatrix = parentMatrix * localMatrix;
}