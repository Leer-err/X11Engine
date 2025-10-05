#ifndef I_BUFFER_H
#define I_BUFFER_H

class IBuffer {
   public:
    virtual void update(const char* data, size_t size, size_t offset) = 0;
};

#endif  // I_BUFFER_H