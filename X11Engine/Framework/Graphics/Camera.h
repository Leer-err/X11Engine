#pragma once

#include <math.h>

#include "../Types/Matrix.h"
#include "../Types/Quaternion.h"
#include "../Types/Vector3.h"

// TODO: Remove engine dependency
#include "Camera.h"
#include "Engine/Scene/Transform.h"

struct Camera {
    Camera(float farZ, float nearZ, float fov, float aspectRatio,
           const vector3& forward, const vector3& up)
        : forward(forward),
          up(up),
          right(cross(up, forward)),
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
        Plan(const vector3& norm, const vector3& p)
            : normal(norm.normalized()), distance(dot(normal, p)) {}
        vector3 normal;

        float distance;
    };

    Frustum(const Camera& camera, const Transform& transform) {
        vector3 cameraPos = transform.position;
        quaternion rotation = transform.rotation;

        vector3 forward = camera.forward.rotate(rotation);
        vector3 up = camera.up.rotate(rotation);
        vector3 right = camera.right.rotate(rotation);

        float halfHeight = camera.farZ * tanf(camera.fov / 2);
        float halfWidth = halfHeight * camera.aspectRatio;
        vector3 farDist = forward * camera.farZ;

        farPlan = {-forward, cameraPos + farDist};
        nearPlan = {forward, cameraPos + forward * camera.nearZ};

        rightPlan = {cross(farDist + right * halfWidth, up), cameraPos};
        leftPlan = {cross(up, farDist - right * halfWidth), cameraPos};

        topPlan = {cross(farDist - up * halfHeight, right), cameraPos};
        bottomPlan = {cross(right, farDist + up * halfHeight), cameraPos};
    }

    Plan topPlan;
    Plan bottomPlan;

    Plan rightPlan;
    Plan leftPlan;

    Plan farPlan;
    Plan nearPlan;
};