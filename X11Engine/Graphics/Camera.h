#pragma once

#include <math.h>

#include "Types/Matrix.h"
#include "Types/Vector3.h"

struct Camera {
    Camera(float farZ, float nearZ, float fov, float aspectRatio,
           const vector3& forward, const vector3& up)
        : forward(forward),
          up(up),
          right(cross(forward, up)),
          farZ(farZ),
          nearZ(nearZ),
          aspectRatio(aspectRatio),
          fov(fov) {
        CalcProjectionMatrix();
    }

    void CalcProjectionMatrix() {
        projection = PerspectiveProjectionMatrix(aspectRatio, fov, farZ, nearZ);
    }

    vector3 forward;
    vector3 up;
    vector3 right;

    float farZ;
    float nearZ;
    float aspectRatio;
    float fov;

    matrix projection;
};

struct Frustum {
    struct Plan {
        Plan() = default;
        Plan(const vector3& normal, const vector3& distance)
            : normal(normal), distance(distance.length()) {}
        vector3 normal;

        float distance;
    };

    Frustum(const Camera& camera, const vector3& cameraPos) {
        float halfHeight = camera.farZ * tanf(camera.fov / 2);
        float halfWidth = halfHeight * camera.aspectRatio;
        vector3 farDist = camera.forward * camera.farZ;

        farPlan = {-camera.forward, cameraPos + farDist};
        nearPlan = {camera.forward, cameraPos + camera.forward * camera.nearZ};

        rightPlan = {cross(camera.up, farDist + camera.right * halfWidth),
                     cameraPos};
        leftPlan = {cross(camera.up, farDist - camera.right * halfWidth),
                    cameraPos};

        topPlan = {cross(camera.right, farDist + camera.right * halfHeight),
                   cameraPos};
        bottomPlan = {cross(camera.right, farDist - camera.right * halfHeight),
                      cameraPos};
    }

    Plan topPlan;
    Plan bottomPlan;

    Plan rightPlan;
    Plan leftPlan;

    Plan farPlan;
    Plan nearPlan;
};