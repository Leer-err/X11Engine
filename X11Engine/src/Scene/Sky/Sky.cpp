#include "Sky.h"

#include <dxgiformat.h>

#include <tracy/Tracy.hpp>


void Sky::draw() {
    ZoneScoped;

    stars.draw();
    clouds.draw();
}