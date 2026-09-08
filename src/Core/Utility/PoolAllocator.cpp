#include "PoolAllocator.h"

#include <cassert>

PoolAllocator::PoolAllocator(uint8_t* ptr, size_t pool_size, size_t object_size,
                             size_t alignment)
    : ptr(ptr), pool_size(pool_size) {
    assert(object_size >= sizeof(uint8_t*));
    assert(pool_size >= object_size);

    auto aligned_address = align(ptr, alignment);
    auto aligned_pool_size = pool_size - (aligned_address - ptr);
    aligned_object_size = align(object_size, alignment);

    auto capacity = aligned_pool_size / object_size;
    auto node = std::bit_cast<Node*>(aligned_address);
    for (int i = 0; i < capacity; i++) {
        node->next = std::bit_cast<Node*>(std::bit_cast<uint8_t*>(node) +
                                          aligned_object_size);
        if (std::bit_cast<uint8_t*>(node->next) >=
            aligned_address + aligned_pool_size) {
            node->next = nullptr;
            break;
        }

        node = node->next;
    }

    free_list = std::bit_cast<Node*>(aligned_address);
}

uint8_t* PoolAllocator::allocate() {
    auto data = std::bit_cast<uint8_t*>(free_list);

    if (free_list) free_list = free_list->next;

    return data;
}

void PoolAllocator::free(const uint8_t* data) {
    if (data < ptr || data > (ptr + pool_size)) return;

    // free list empty -> ptr is first
    if (free_list == nullptr) {
        free_list = std::bit_cast<Node*>(data);
        free_list->next = nullptr;
        return;
    }

    // search for insertion place
    auto node = free_list;
    while (node->next && std::bit_cast<uint8_t*>(node->next) < data)
        node = node->next;

    assert(std::bit_cast<uint8_t*>(node->next) != data);

    // all node->next before ptr -> ptr is last
    if (node->next == nullptr) {
        node->next = std::bit_cast<Node*>(data);
        node->next->next = nullptr;
        return;
    }

    // ptr between nodes
    auto new_node = std::bit_cast<Node*>(data);
    new_node->next = node->next;
    node->next = new_node;
}