#include "CameraManager.h"

#include <cstddef>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "CameraData.h"
#include "Context.h"
#include "Matrix.h"
#include "Transform.h"

CameraManager::CameraManager() {
    camera_buffer = BufferBuilder(sizeof(CameraData))
                        .isConstantBuffer()
                        .isCPUWritable()
                        .create()
                        .getResult();
}

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

void CameraManager::updateCameraData() {
    auto transform = main_camera_entity.get<Transform>();
    Matrix view = Matrix::view(transform->getPosition(),
                               transform->getForward(), transform->getUp());
    Matrix projection = main_camera->getProjectionMatrix();

    Matrix view_projection = view * projection;

    auto context = Context();
    auto camera_data = context.mapConstantBuffer<CameraData>(camera_buffer);
    camera_data->view_projection = view_projection;
    camera_data->inverse_view_projection = view_projection.inverse();
    context.unmapConstantBuffer(camera_buffer);
}

Buffer CameraManager::getCameraData() const { return camera_buffer; }