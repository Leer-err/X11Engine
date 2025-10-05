#ifndef ANIMATION_REGISTRY_H
#define ANIMATION_REGISTRY_H

#include <string>
#include <unordered_map>

#include "Animation.h"

class AnimationRegistry {
   public:
    void add(const Animation& animation);
    const Animation* get(const std::string& name) const;

   private:
    std::unordered_map<std::string, Animation> animations;
};

#endif  // ANIMATION_REGISTRY_H