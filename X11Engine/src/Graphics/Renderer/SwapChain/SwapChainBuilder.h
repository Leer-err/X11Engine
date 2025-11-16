#pragma once

#include <cstdint>

#include "SwapChain.h"

class SwapChainBuilder {
   public:
    SwapChainBuilder(uint32_t width, uint32_t height);

    SwapChainBuilder& windowed();
    SwapChainBuilder& fullscren();

    SwapChain create();

   private:
    uint32_t width;
    uint32_t height;
    bool is_fullscreen;
};