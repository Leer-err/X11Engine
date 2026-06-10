#include "Stars.h"

#include <tracy/Tracy.hpp>

#include "Engine.h"
#include "Graphics.h"
#include "StarsData.h"

Stars::Stars()
    : star_density(30),
      blinking_speed(1),
      blink_strength(0.6f),
      render_world(Graphics::getRenderEngine()->getRenderWorld()) {}

void Stars::draw() {
    ZoneScoped;

    auto star_parameters = StarsData();
    star_parameters.time = Engine::Engine::get().getTime();
    star_parameters.star_density = star_density;
    star_parameters.blink_strength = blink_strength;
    star_parameters.blinking_speed = blinking_speed;

    render_world.setStarsData(star_parameters);
}
