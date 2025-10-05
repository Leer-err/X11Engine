#ifndef CAMERA_H
#define CAMERA_H

#include "Matrix.h"

class ICamera {
   public:
    virtual Matrix getProjectionMatrix() const = 0;
    virtual Matrix getViewMatrix() const = 0;
};

#endif  // CAMERA_H