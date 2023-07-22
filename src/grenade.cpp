#include "grenade.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/gpu_particles2d.hpp>

Grenade::Grenade() {
  m_timer.set_timeout(10);
  m_timer.set_callback([&]() { explode(); });
}

void Grenade::_bind_methods() {
  BIND_PROPERTY(Grenade, timeout, Variant::FLOAT);
}

void Grenade::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  m_timer.start();
}

void Grenade::_process(double delta) {
  m_timer.tick(delta);
}

void Grenade::explode() {
  auto const particles = get_node<GPUParticles2D>("Particles");
  if (particles) {
    remove_child(particles);
    particles->set_global_position(get_global_position());
    get_parent()->add_child(particles);
    particles->set_emitting(true);
  }
  queue_free();
}

void Grenade::set_timeout(TimeDelta timeout) {
  m_timer.set_timeout(timeout);
}

TimeDelta Grenade::get_timeout() const {
  return m_timer.get_timeout();
}