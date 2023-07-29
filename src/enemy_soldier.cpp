#include "enemy_soldier.hpp"
#include "gun.hpp"
#include "health_bar.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>

EnemySoldier::IdleState EnemySoldier::idle       = EnemySoldier::IdleState();
EnemySoldier::FallingState EnemySoldier::falling = EnemySoldier::FallingState();
EnemySoldier::DyingState EnemySoldier::dying     = EnemySoldier::DyingState();
static auto constexpr go_idle                    = EnemySoldier::IdleCommand();
static auto constexpr die                        = EnemySoldier::DieCommand();
static auto constexpr hit                        = EnemySoldier::HitCommand();

void EnemySoldier::_bind_methods() {
  BIND_PROPERTY(EnemySoldier, gravity, Variant::FLOAT);
  BIND_PROPERTY(EnemySoldier, total_health, Variant::INT);
  BIND_PROPERTY(EnemySoldier, fire_rate, Variant::FLOAT);
  BIND_PROPERTY(EnemySoldier, hit_bounce_velocity, Variant::VECTOR2);
  ClassDB::bind_method(D_METHOD("take_hit"), &EnemySoldier::take_hit);
}

void EnemySoldier::_ready() {
  m_health        = m_total_health;
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_max(m_health);
    health_bar->set_value(m_health);
  }
  m_fire_timer.set_callback([&]() { fire(); });
  m_fire_timer.set_timeout(m_fire_rate);
  m_fire_timer.set_repeat(true);
  m_fire_timer.start();
  auto animated_sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Idle");
  }
}

void EnemySoldier::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_fire_timer.tick(delta);
}

void EnemySoldier::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  m_state->update(*this);
  auto velocity = get_velocity();
  velocity.y += m_gravity * static_cast<float>(delta);
  set_velocity(velocity);
  move_and_slide();
}

void EnemySoldier::take_hit(int damage, Vector2 const& from_direction) {
  m_health -= damage;
  hit(*this, from_direction);
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_value(m_health);
  }
  if (m_health <= 0) {
    die(*this);
  }
}

void EnemySoldier::set_state(EnemySoldierState* state) {
  m_state = state;
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

int EnemySoldier::get_total_health() const {
  return m_total_health;
}

void EnemySoldier::set_fire_rate(double rate) {
  m_fire_rate = rate;
}

double EnemySoldier::get_fire_rate() const {
  return m_fire_rate;
}

void EnemySoldier::fire() {
  auto gun = get_node<Gun>("Gun");
  if (gun) {
    gun->fire({});
  }
}

void EnemySoldier::set_hit_bounce_velocity(const Vector2& velocity) {
  m_hit_bounce_velocity = velocity;
}

Vector2 EnemySoldier::get_hit_bounce_velocity() const {
  return m_hit_bounce_velocity;
}

EnemySoldier::Direction EnemySoldier::get_direction() const {
  return m_direction;
}

// Commands
void EnemySoldier::DieCommand::operator()(EnemySoldier& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Die");
  }
  enemy.set_state(&EnemySoldier::dying);
}

void EnemySoldier::HitCommand::operator()(EnemySoldier& enemy,
                                          Vector2 const& from_direction) const {
  auto const position  = enemy.get_global_position();
  auto const direction = position.x > from_direction.x ? 1.0f : -1.0f;
  auto bounce_velocity = enemy.get_hit_bounce_velocity();
  bounce_velocity.x *= direction;
  auto velocity = enemy.get_velocity();
  velocity += bounce_velocity;
  enemy.set_velocity(velocity);
  enemy.set_state(&EnemySoldier::falling);
}

// States
void EnemySoldier::IdleState::update(EnemySoldier&) const {
}

void EnemySoldier::FallingState::update(EnemySoldier& enemy) const {
  auto const v_y = enemy.get_velocity().y;
  if (v_y >= 0 && enemy.is_on_floor()) {
    go_idle(enemy);
  } else {
    auto const animated_sprite =
        enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
    if (animated_sprite) {
      animated_sprite->play("Fall");
    }
  }
}

void EnemySoldier::DyingState::update(EnemySoldier& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite && animated_sprite->is_playing()) {
    return;
  }
  enemy.queue_free();
}

void EnemySoldier::IdleCommand::operator()(EnemySoldier& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Idle");
  }
  enemy.set_velocity({0, 0});
  enemy.set_state(&EnemySoldier::idle);
}
