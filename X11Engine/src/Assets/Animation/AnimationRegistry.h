#ifndef ANIMATION_REGISTRY_H
#define ANIMATION_REGISTRY_H

#include <string>
#include <unordered_map>

#include "Animation.h"

class AnimationRegistry {
   public:
    static AnimationRegistry& get() {
        static AnimationRegistry instance;
        return instance;
    }

    void add(const Animation& animation);
    const Animation* get(const std::string& name) const;

   private:
    AnimationRegistry() {}

    AnimationRegistry(const AnimationRegistry&) = delete;
    AnimationRegistry(AnimationRegistry&&) = delete;

    AnimationRegistry& operator=(const AnimationRegistry&) = delete;
    AnimationRegistry& operator=(AnimationRegistry&&) = delete;

    std::unordered_map<std::string, Animation> animations;
};

#endif  // ANIMATION_REGISTRY_H