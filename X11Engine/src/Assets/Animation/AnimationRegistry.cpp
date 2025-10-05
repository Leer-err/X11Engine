#include "AnimationRegistry.h"

#include <string>

#include "Animation.h"

void AnimationRegistry::add(const Animation& animation) {
    animations.emplace(animation.name, animation);
}

const Animation* AnimationRegistry::get(const std::string& name) const {
    auto animation = animations.find(name);

    if (animation == animations.end()) return nullptr;

    return &animation->second;
}