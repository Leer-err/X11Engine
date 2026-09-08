#pragma once

#include <bit>

#include "ResourceRegistryBase.h"

template <typename T>
class Handle {
   public:
    Handle() = default;
    explicit Handle(T* ptr, ResourceRegistryBase* registry)
        : resource(ptr), registry(registry) {}
    ~Handle() {
        if (resource && registry)
            registry->release(std::bit_cast<uint8_t*>(resource));
    }

    Handle& operator=(const Handle& other) {
        resource = other.resource;
        registry = other.registry;

        registry->acquire(std::bit_cast<uint8_t*>(resource));

        return *this;
    }
    Handle(const Handle& other) {
        resource = other.resource;
        registry = other.registry;

        registry->acquire(std::bit_cast<uint8_t*>(resource));
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
    const T* operator->() const { return resource; }

    T& get() { return *resource; }
    const T& get() const { return *resource; }

    bool isValid() const { return resource != nullptr; }

   private:
    T* resource;
    ResourceRegistryBase* registry;
};