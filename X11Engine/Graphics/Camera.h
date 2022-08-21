#pragma once

#include <math.h>

#include "Vector3.h"

struct Camera {
    vector3 forward;
    vector3 up;
    vector3 right;

    float far;
    float near;
    float aspectRatio;
    float fov;
    int height;
    int width;
}

struct Frustum {
    struct Plan {
        vector3 normal;

        float distance;
    }

    Frustum(const vector3& cameraPos, const vector3& cameraFront,
            const vector3& cameraUp, const vector3& cameraRight, float farZ,
            float nearZ, float aspectRatio, float fovY) {
        vector3 halfHeight = farZ * tanf(fovY / 2);
        vector3 halfWidth = halfHeight * aspectRatio;
        vector3 farDist = cameraFront * farZ;

        far = {-cameraFront, cameraPos + farDist};
        near = {cameraFront, cameraPos + cameraFront * nearZ};

        right = {cross(cameraUp, farDist + cameraRight * halfWidth), cameraPos};
        left = {cross(cameraUp, farDist - cameraRight * halfWidth), cameraPos};

        top = {cross(cameraRight, farDist + cameraRight * halfHeight),
               cameraPos};
        bottom = {cross(cameraRight, farDist - cameraRight * halfHeight),
                  cameraPos};
    }

    Plan top;
    Plan bottom;

    Plan right;
    Plan left;

    Plan far;
    Plan near;
}