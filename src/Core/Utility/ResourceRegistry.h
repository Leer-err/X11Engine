#pragma once

#include <atomic>
#include <bit>
#include <cassert>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <utility>

#include "PoolAllocator.h"

template <typename T>
class ResourceRegistry {
    using RefCounter = std::atomic<uint32_t>;

   public:
    explicit ResourceRegistry(PoolAllocator& allocator)
        : allocator(allocator) {}
    ~ResourceRegistry() {
        for (const auto& [ptr, counter] : ref_counters) remove(ptr);
    }

    class Handle {
       public:
        Handle() = default;
        explicit Handle(T* ptr, ResourceRegistry<T>* registry)
            : resource(ptr), registry(registry) {}
        ~Handle() {
            if (resource && registry) registry->release(resource);
        }

        Handle& operator=(const Handle& other) {
            resource = other.resource;
            registry = other.registry;

            registry->acquire(resource);

            return *this;
        }
        Handle(const Handle& other) {
            resource = other.resource;
            registry = other.registry;

            registry->acquire(resource);
        }
        Handle& operator=(Handle&& other) noexcept {
            resource = other.resource;
            registry = other.registry;

            other.resource = nullptr;
            other.registry = nullptr;

            return *this;
        }
        Handle(Handle&& other) noexcept {
            resource = other.resource;
            registry = other.registry;

            other.resource = nullptr;
            other.registry = nullptr;
        }

        T* operator->() { return resource; }

        T& get() { return *resource; }
        const T& get() const { return *resource; }

        bool isValid() const { return resource != nullptr; }

       private:
        T* resource;
        ResourceRegistry* registry;
    };

    template <typename... ARGS>
    Handle create(ARGS&&... args) {
        auto memory = allocator.allocate();
        if (memory == nullptr) return Handle();

        ref_counters.emplace(std::bit_cast<const T*>(memory), 1);

        return Handle(new (memory) T(std::forward<ARGS>(args)...), this);
    }

   private:
    void acquire(const T* ptr) {
        auto it = ref_counters.find(ptr);
        assert(it != ref_counters.end());

        it->second.fetch_sub(1);
    }

    void release(T* ptr) {
        auto it = ref_counters.find(ptr);
        assert(it != ref_counters.end());

        auto count = it->second.fetch_sub(1);
        if (count == 1) {
            ref_counters.erase(it);
            remove(ptr);
        }
    }

    void remove(const T* ptr) {
        ptr->~T();
        allocator.free(std::bit_cast<uint8_t*>(ptr));
    }

    PoolAllocator& allocator;
    std::unordered_map<const T*, RefCounter> ref_counters;
};

template <typename T>
using Handle = ResourceRegistry<T>::Handle;