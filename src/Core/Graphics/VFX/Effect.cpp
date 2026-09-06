#include "Effect.h"

#include <vector>

#include "ParticleHandle.h"
#include "Vector3.h"

namespace Graphics {

Effect::Effect(const EffectDescription& description,
               ParticleHandleAllocator& allocator,
               std::vector<Particle>& particles)
    : allocator(allocator),
      particles(particles),
      emitter{description.center, description.extents, description.spawn_rate},
      color(description.color),
      size(description.size),
      rotation(description.rotation),
      lifetime(description.particle_lifetime),
      texture(description.texture) {}

void Effect::update(float delta_time) {
    auto alive_particles = std::vector<ParticleHandle>();

    for (const auto& handle : owned_particles) {
        auto& particle = particles[handle];
        particle.age += delta_time;

        auto lifetime_ratio = particle.age / particle.lifetime;
        if (particle.age >= particle.lifetime) {
            allocator.free(handle);
            continue;
        }
        alive_particles.push_back(handle);

        particle.position = particle.position + particle.velocity * delta_time;

        particle.size = size.evaluate(lifetime_ratio);
        particle.color = color.evaluate(lifetime_ratio);
    }

    owned_particles = alive_particles;
}

void Effect::emit(float delta_time) {
    auto should_spawn = spawn_error + delta_time * emitter.spawn_rate;
    auto will_spawn = static_cast<int>(should_spawn);

    spawn_error = should_spawn - will_spawn;

    std::random_device rd;
    std::mt19937 gen(rd());

    Vector3 min_point = emitter.center - emitter.extents;
    Vector3 max_point = emitter.center + emitter.extents;
    std::uniform_real_distribution<float> x_dist(min_point.x, max_point.x);
    std::uniform_real_distribution<float> y_dist(min_point.y, max_point.y);
    std::uniform_real_distribution<float> z_dist(min_point.z, max_point.z);

    for (int i = 0; i < will_spawn; i++) {
        auto handle_opt = allocator.allocate();
        if (!handle_opt.has_value()) return;

        auto handle = handle_opt.value();

        owned_particles.push_back(handle);

        auto particle = Particle();
        particle.age = 0;
        particle.lifetime = lifetime;

        particle.position = Vector3(x_dist(gen), y_dist(gen), z_dist(gen));
        particle.texture = texture;
        particle.velocity = Vector3();
        particle.color = color.evaluate(0);
        particle.size = size.evaluate(0);
        particle.rotation = rotation.evaluate(0);

        particles[handle] = particle;
    }
}

}  // namespace Graphics