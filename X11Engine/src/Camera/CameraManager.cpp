#include "CameraManager.h"

std::shared_ptr<ICamera> CameraManager::getMainCamera() const {
    return main_camera;
}

void CameraManager::setMainCamera(std::shared_ptr<ICamera> camera) {
    main_camera = camera;
}

void CameraManager::setMainCameraEntity(Entity entity) {
    main_camera_entity = entity;
}

Entity CameraManager::getMainCameraEntity() const { return main_camera_entity; }