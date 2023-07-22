#include "enemy_soldier.hpp"
#include "health_bar.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>

EnemySoldier::IdleState EnemySoldier::idle   = EnemySoldier::IdleState();
EnemySoldier::DyingState EnemySoldier::dying = EnemySoldier::DyingState();
static auto constexpr die                    = EnemySoldier::DieCommand();

void EnemySoldier::_bind_methods() {
  BIND_PROPERTY(EnemySoldier, gravity, godot::Variant::FLOAT);
  ClassDB::bind_method(D_METHOD("take_hit"), &EnemySoldier::take_hit);
}

void EnemySoldier::_ready() {
  m_health        = m_total_health;
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_max(m_health);
    health_bar->set_value(m_health);
  }
}

void EnemySoldier::_process(double) {
  m_state->update(*this);
}

void EnemySoldier::take_hit(int damage) {
  m_health -= damage;
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_value(m_health);
  }
  if (m_health <= 0) {
    die(*this);
  }
}

void EnemySoldier::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  auto velocity = get_velocity();
  velocity.y    = m_gravity * static_cast<float>(delta);
  set_velocity(velocity);
  move_and_slide();
}

void EnemySoldier::set_gravity(float gravity) {
  m_gravity = gravity;
}

float EnemySoldier::get_gravity() const {
  return m_gravity;
}

void EnemySoldier::set_total_health(int health) {
  m_total_health = health;
}

void EnemySoldier::set_state(EnemySoldierState* state) {
  m_state = state;
}

// Commands
void EnemySoldier::DieCommand::operator()(EnemySoldier& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Die");
  }
  enemy.set_state(&EnemySoldier::dying);
}

// States
void EnemySoldier::IdleState::update(EnemySoldier&) const {
}

void EnemySoldier::DyingState::update(EnemySoldier& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite && animated_sprite->is_playing()) {
    return;
  }
  enemy.queue_free();
}