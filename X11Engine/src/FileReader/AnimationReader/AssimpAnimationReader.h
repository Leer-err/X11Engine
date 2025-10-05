#ifndef ASSIMP_ANIMATION_READER_H
#define ASSIMP_ANIMATION_READER_H

#include "Animation.h"
#include "IAnimationReader.h"

struct aiAnimation;

class AssimpAnimationReader : public IAnimationReader {
   public:
    std::vector<Animation> readAll(const std::string& filename) const override;
    std::optional<Animation> read(const std::string& filename,
                                  const std::string& name) const override;

   private:
    Animation read(const aiAnimation* animation) const;
};

#endif  // ASSIMP_ANIMATION_READER_H