#pragma once

#include <optional>

#include "Input.h"

struct Binding {
    char key;
    Action action;
};

class IKeyboardMouseConfigReader {
   public:
    virtual std::optional<Binding> getBinding() = 0;
};