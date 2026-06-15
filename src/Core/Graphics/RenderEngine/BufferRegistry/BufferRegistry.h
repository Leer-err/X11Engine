#pragma once

#include <map>
#include <memory>

#include "Buffer.h"

namespace Graphics {

using BufferHandle = uint32_t;

class BufferRegistry {
   public:
    BufferHandle registerBuffer(const Buffer& buffer);

    const Buffer* getBuffer(BufferHandle handle) const;
    ````` bool hasBuffer(BufferHandle handle) const;

   private:
    std::map<BufferHandle, std::shared_ptr<const Buffer>> buffers_;
    BufferHandle next_handle_ = 1;
};

}  // namespace Graphics