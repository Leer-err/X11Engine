#ifndef STATIC_PROJECTION_CAMERA_H
#define STATIC_PROJECTION_CAMERA_H

#include "ICamera.h"

class StaticProjectionCamera : public ICamera {
   public:
    StaticProjectionCamera() {}
    StaticProjectionCamera(float fov, float aspect_ratio, float near_clip,
                           float far_clip);

    Matrix getProjectionMatrix() const override;

   private:
    float fov;
    float aspect_ratio;
    float near_clip;
    float far_clip;
};

#endif  // STATIC_PROJECTION_CAMERA_H