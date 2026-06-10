#include "Clouds.h"

#include <tracy/Tracy.hpp>

#include "CloudsData.h"
#include "Engine.h"
#include "Vector3.h"

Clouds::Clouds() : cloud_plane_height(100), cloud_plane_size(1000) {}

void Clouds::draw() {
    ZoneScoped;

    auto cloud_parameters = CloudsData();
    cloud_parameters.time = Engine::Engine::get().getTime();
    cloud_parameters.color = Vector3(1, 0, 1);
    cloud_parameters.cloud_plane_scale = cloud_plane_size;
    cloud_parameters.height = cloud_plane_height;
}
