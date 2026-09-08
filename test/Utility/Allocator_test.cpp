#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "PoolAllocator.h"

struct TestObject {
    char* a;
};

TEST(AllocatorTest, allocationTest) {
    auto memory = new uint8_t[sizeof(TestObject)];

    auto allocator = PoolAllocator(memory, sizeof(TestObject),
                                   sizeof(TestObject), alignof(TestObject));

    auto ptr = allocator.allocate();

    ASSERT_NE(ptr, nullptr);

    delete[] memory;
}

TEST(AllocatorTest, overAllocationTest) {
    auto memory = new uint8_t[sizeof(TestObject)];

    auto allocator = PoolAllocator(memory, sizeof(TestObject),
                                   sizeof(TestObject), alignof(TestObject));

    auto ptr = allocator.allocate();
    ptr = allocator.allocate();

    ASSERT_EQ(ptr, nullptr);

    delete[] memory;
}