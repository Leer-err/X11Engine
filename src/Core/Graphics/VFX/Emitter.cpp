#include "Emitter.h"

#include "EffectDescription.h"

namespace Graphics {

Emitter::Emitter(const EmitterDescription& description)
    : spawner(description.spawner),
      color(description.color),
      size(description.size),
      spawn_rate(description.spawn_rate),
      particle_lifetime(description.particle_lifetime),
      texture(description.texture) {}

}  // namespace Graphics