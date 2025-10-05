#pragma once

class ISwapChain {
   public:
    virtual void present() = 0;
};