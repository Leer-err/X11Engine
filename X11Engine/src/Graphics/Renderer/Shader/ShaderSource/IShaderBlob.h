#ifndef I_SHADER_BLOB
#define I_SHADER_BLOB

class IShaderBlob {
   public:
    virtual void* get() = 0;
    virtual size_t getSize() = 0;
};

#endif  // I_SHADER_BLOB