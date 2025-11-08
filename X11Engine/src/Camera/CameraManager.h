#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <memory>

#include "Entity.h"
#include "ICamera.h"

class CameraManager {
   public:
    static CameraManager& get() {
        static CameraManager instance;
        return instance;
    }

    void setMainCamera(std::shared_ptr<ICamera> camera);
    void setMainCameraEntity(Entity entity);

    std::shared_ptr<ICamera> getMainCamera() const;
    Entity getMainCameraEntity() const;

   private:
    CameraManager() {}
    CameraManager(const CameraManager&) = delete;
    CameraManager(CameraManager&&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;
    CameraManager& operator=(CameraManager&&) = delete;

    std::shared_ptr<ICamera> main_camera;
    Entity main_camera_entity;
};

#endif  // CAMERA_MANAGER_H