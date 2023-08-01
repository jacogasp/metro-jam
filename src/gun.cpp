#include "gun.hpp"
#include <godot_cpp/classes/rigid_body2d.hpp>

void Gun::_bind_methods() {
  BIND_PROPERTY(Gun, bullet, Variant::OBJECT);
  BIND_PROPERTY(Gun, bullet_speed, Variant::FLOAT);
  BIND_PROPERTY(Gun, cooldown, Variant::FLOAT);
}

void Gun::_ready() {
  m_cooldown_timer.set_repeat(false);
  m_cooldown_timer.set_callback([&]() { m_cooling_down = false; });
}

void Gun::_process(double t) {
  m_cooldown_timer.tick(t);
}

void Gun::set_bullet(const Ref<PackedScene>& scene) {
  m_bullet = scene;
}

Ref<PackedScene> Gun::get_bullet() const {
  return m_bullet;
}

void Gun::fire(const Vector2& direction) {
  if (cooling_down()) {
    return;
  }
  if (m_cooldown_timer.get_timeout() > 0) {
    m_cooling_down = true;
    m_cooldown_timer.start();
  }
  auto node   = m_bullet->instantiate();
  auto bullet = cast_to<RigidBody2D>(node);
  if (bullet) {
    get_parent()->get_parent()->add_child(bullet);
    auto const impulse = direction.normalized() * m_bullet_speed;
    bullet->apply_impulse(impulse);
    bullet->set_global_position(get_global_position());
  } else {
    std::cerr << "Grenade Packed Scene must be of type Grenade\n";
  }
}

void Gun::set_bullet_speed(float speed) {
  m_bullet_speed = speed;
}

float Gun::get_bullet_speed() const {
  return m_bullet_speed;
}

void Gun::set_cooldown(double cooldown) {
  m_cooldown_timer.set_timeout(cooldown);
}

double Gun::get_cooldown() const {
  return m_cooldown_timer.get_timeout();
}

bool Gun::cooling_down() const {
  return m_cooling_down;
}
