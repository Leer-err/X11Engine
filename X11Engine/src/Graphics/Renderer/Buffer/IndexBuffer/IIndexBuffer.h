#pragma once

#include <cstdint>

#include "IBuffer.h"


class IIndexBuffer : public IBuffer {
   public:
    virtual uint32_t getIndexCount() = 0;
};