#pragma once

#include <cstddef>
#include <optional>
#include <set>

#include "ParticleHandle.h"

namespace Graphics {

class ParticleHandleAllocator {
   public:
    explicit ParticleHandleAllocator(size_t size);

    std::optional<ParticleHandle> allocate();
    void free(ParticleHandle index);

    std::set<ParticleHandle> getAllocated() const;

   private:
    std::set<ParticleHandle> free_list;
    std::set<ParticleHandle> allocated_list;
};

}  // namespace Graphics