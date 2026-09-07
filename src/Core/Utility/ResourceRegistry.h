#pragma once

#include <cstdint>

template <typename T>
class ResourceRegistry {
   public:
    using Id = uint32_t;

   private:
};

template <typename T>
class Handle {
   public:
   private:
    ResourceRegistry<T>::Id id;
    ResourceRegistry<T>* registry;
};
