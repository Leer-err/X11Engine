#ifndef I_ANIMATION_READER_H
#define I_ANIMATION_READER_H

#include <optional>
#include <string>

#include "Animation.h"

class IAnimationReader {
   public:
    virtual std::vector<Animation> readAll(
        const std::string& filename) const = 0;
    virtual std::optional<Animation> read(const std::string& filename,
                                          const std::string& name) const = 0;
};

#endif  // I_ANIMATION_READER_H