#include "Sky.h"

#include <dxgiformat.h>

void Sky::draw() {
    stars.draw();
    clouds.draw();
}