#include "grenade.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/gpu_particles2d.hpp>

void Grenade::_bind_methods() {
  BIND_PROPERTY(Grenade, timeout, Variant::FLOAT);
  BIND_PROPERTY(Grenade, damages, Variant::INT);
  BIND_PROPERTY(Grenade, explode_on_collision, Variant::BOOL);
  ClassDB::bind_method(D_METHOD("body_entered"), &Grenade::body_entered);
}

static void wait_until_exploded(Grenade& grenade) {
  auto const particles = grenade.get_node<GPUParticles2D>("Particles");
  if (particles && particles->get_amount() == 0) {
    return;
  }

  auto const animated_sprite =
      grenade.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite && animated_sprite->is_playing()) {
    return;
  }
  grenade.queue_free();
}

void Grenade::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  if (!m_explode_on_collision) {
    m_timer.start();
  } else {
    m_timer.set_timeout(10);
    m_timer.set_callback([&]() { explode(); });
  }
}

void Grenade::_physics_process(double delta) {
  m_timer.tick(delta);
  if (m_exploding) {
    wait_until_exploded(*this);
  }
}

static void play_particles(Grenade& grenade) {
  auto const particles = grenade.get_node<GPUParticles2D>("Particles");
  if (particles) {
    particles->set_emitting(true);
  }
}

static void make_damages(Grenade& grenade) {
  auto const explosion_area = grenade.get_node<Area2D>("ExplosionArea");
  if (explosion_area && explosion_area->has_overlapping_bodies()) {
    auto bodies = explosion_area->get_overlapping_bodies();
    for (auto i = 0; i < bodies.size(); ++i) {
      auto const body = Grenade::cast_to<Node2D>(bodies[i]);
      if (body->is_in_group("Player")) {
        body->call("hit");
      } else {
        body->call("take_hit", grenade.get_damages(),
                   grenade.get_global_position());
      }
    }
  }
}

static void play_animation(Grenade& grenade) {
  auto const animated_sprite =
      grenade.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Explode");
    grenade.call_deferred("set_freeze_enabled", true);
    grenade.set_rotation(0);
  }
}

void Grenade::explode() {
  play_particles(*this);
  make_damages(*this);
  play_animation(*this);
  m_exploding = true;
}

void Grenade::set_timeout(TimeDelta timeout) {
  m_timer.set_timeout(timeout);
}

TimeDelta Grenade::get_timeout() const {
  return m_timer.get_timeout();
}
void Grenade::set_damages(int damages) {
  m_damages = damages;
}

int Grenade::get_damages() const {
  return m_damages;
}

void Grenade::set_explode_on_collision(bool explode_on_collision) {
  m_explode_on_collision = explode_on_collision;
}

bool Grenade::get_explode_on_collision() const {
  return m_explode_on_collision;
}

void Grenade::body_entered(Node*) {
  if (m_explode_on_collision) {
    explode();
  }
}
