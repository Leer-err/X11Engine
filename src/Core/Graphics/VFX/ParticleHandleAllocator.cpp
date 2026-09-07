#include "ParticleHandleAllocator.h"

#include <optional>

namespace Graphics {

ParticleHandleAllocator::ParticleHandleAllocator(size_t size) {
    for (size_t i = 0; i < size; i++) {
        free_list.emplace(i);
    }
}

std::optional<ParticleHandle> ParticleHandleAllocator::allocate() {
    if (free_list.empty()) return std::nullopt;

    auto it = free_list.begin();
    auto handle = *it;
    free_list.erase(it);
    allocated_list.emplace(handle);

    return handle;
}

void ParticleHandleAllocator::free(ParticleHandle index) {
    allocated_list.erase(index);
    free_list.emplace(index);
}

std::set<ParticleHandle> ParticleHandleAllocator::getAllocated() const {
    return allocated_list;
}

}  // namespace Graphics