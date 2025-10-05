#include "PositionComponents.h"

#include <format>
#include <string>

#include "Quaternion.h"
#include "Vector3.h"

std::string GlobalMatrix::serialize() const {
    Vector3 position = matrix.getTranslation();
    Vector3 scale = matrix.getScale();
    Quaternion rotation = matrix.getRotation();

    std::string position_str = std::format("Position: x:{} y:{} z:{}",
                                           position.x, position.y, position.z);
    std::string rotation_str =
        std::format("Rotation: w:{} x:{} y:{} z:{}", rotation.w, rotation.x,
                    rotation.y, rotation.z);
    std::string scale_str =
        std::format("Scale: x:{} y:{} z:{}", scale.x, scale.y, scale.z);

    return std::format("{}\n{}\n{}", position_str, rotation_str, scale_str);
}