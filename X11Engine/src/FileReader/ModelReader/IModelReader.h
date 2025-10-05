#ifndef I_MODEL_READER_H
#define I_MODEL_READER_H

#include <optional>
#include <string>

#include "Model.h"

class IModelReader {
   public:
    virtual std::optional<Model> read(const std::string& filename) = 0;
};

#endif  // I_MODEL_READER_H