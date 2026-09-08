#include "PoolAllocator.h"

#include <bit>
#include <cassert>
#include <new>

PoolAllocator::PoolAllocator(size_t max_object_count, size_t object_size,
                             size_t alignment) {
    assert(object_size >= sizeof(uint8_t*));
    assert(max_object_count > 0);

    pool_size = max_object_count * object_size;

    ptr = std::bit_cast<uint8_t*>(
        ::operator new(pool_size, std::align_val_t(alignment)));
    aligned_object_size = align(object_size, alignment);

    auto capacity = pool_size / object_size;
    auto node = std::bit_cast<Node*>(ptr);
    for (int i = 0; i < capacity; i++) {
        node->next = std::bit_cast<Node*>(std::bit_cast<uint8_t*>(node) +
                                          aligned_object_size);
        if (std::bit_cast<uint8_t*>(node->next) >= ptr + pool_size) {
            node->next = nullptr;
            break;
        }

        node = node->next;
    }

    free_list = std::bit_cast<Node*>(ptr);
}

PoolAllocator::~PoolAllocator() { delete[] ptr; }

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