#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "Handle.h"
#include "PoolAllocator.h"
#include "ResourceRegistryBase.h"
#include "TransparentStringHash.h"

template <typename T>
class ResourceRegistry final : public ResourceRegistryBase {
   public:
    explicit ResourceRegistry(PoolAllocator& allocator)
        : ResourceRegistryBase(allocator) {}
    ~ResourceRegistry() {
        for (auto& [ptr, counter] : ref_counters) remove(ptr);
    }

    template <typename... ARGS>
    Handle<T> create(ARGS&&... args) {
        auto memory = allocate();
        if (memory == nullptr) return Handle<T>();

        return Handle(new (memory) T(std::forward<ARGS>(args)...), this);
    }

   private:
    void remove(uint8_t* ptr) override {
        std::bit_cast<T*>(ptr)->~T();
        ResourceRegistryBase::remove(ptr);
    }
};

template <typename T>
class ResourceIndex {
   public:
    void add(std::string_view name, const Handle<T>& resource) {
        assert(name != "");

        index.emplace(name, resource);
    }

    std::optional<Handle<T>> get(std::string_view name) {
        auto it = index.find(name);
        if (it == index.end()) return std::nullopt;

        return it->second;
    }

    void remove(std::string_view name) { index.erase(name); }

   private:
    using Index = std::unordered_map<std::string, Handle<T>,
                                     TransparentStringHash, std::equal_to<>>;

    Index index;
};