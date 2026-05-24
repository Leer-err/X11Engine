#include "PhysicsComponents.h"

#include <format>
#include <string>

std::string StaticRigidBodyComponent::serialize() const {
    return "Static rigid body";
}

std::string DynamicRigidBodyComponent::serialize() const {
    return "Dynamic rigid body";
}

std::string Velocity::serialize() const {
    return std::format("Body velocity is {}", velocity.length());
}