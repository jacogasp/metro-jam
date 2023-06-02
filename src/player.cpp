#include "player.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cassert>

StandingState Player::standing      = StandingState();
RunningState Player::running        = RunningState();
JumpingState Player::jumping        = JumpingState();
AirJumpingState Player::air_jumping = AirJumpingState();
AttackState Player::attacking       = AttackState();

void Player::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_speed"), &Player::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
  ClassDB::bind_method(D_METHOD("set_gravity"), &Player::set_gravity);
  ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
  ClassDB::bind_method(D_METHOD("get_jump_force"), &Player::get_jump_force);
  ClassDB::bind_method(D_METHOD("set_jump_force"), &Player::set_jump_force);
  ClassDB::bind_method(D_METHOD("get_air_jump_force"),
                       &Player::get_air_jump_force);
  ClassDB::bind_method(D_METHOD("set_air_jump_force"),
                       &Player::set_air_jump_force);

  // clang-format off
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"),
               "set_gravity","get_gravity");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_force"),
               "set_jump_force","get_jump_force");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "air_jump_force"),
               "set_air_jump_force", "get_air_jump_force");
  // clang-format on
}

void Player::_ready() {
  PROFILE_FUNCTION()
  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animatedSprite2D->play("Idle");

  m_weapon = get_node<Weapon>("Weapon");
  m_weapon->set_monitoring(false);
}

void Player::_physics_process(float delta) {
  PROFILE_FUNCTION()
  assert(m_state);
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto input = Input::get_singleton();
  m_state->handleInput(*this, *input);
  m_state->update(*this);

  auto const g = get_velocity().y + m_gravity * delta;
  Vector2 velocity{0, g};

  if (input->is_action_pressed("move_left")) {
    velocity.x = -m_speed;
  } else if (input->is_action_pressed("move_right")) {
    velocity.x = m_speed;
  }

  auto is_flipped = m_animatedSprite2D->is_flipped_h() && velocity.x == 0;
  m_animatedSprite2D->set_flip_h(velocity.x < 0 || is_flipped);
  set_velocity(velocity);
  move_and_slide();
}

// Setters and getters
float Player::get_speed() const {
  return m_speed;
}

void Player::set_speed(float speed) {
  m_speed = speed;
}

float Player::get_jump_force() const {
  return m_jump_force;
}

void Player::set_jump_force(float force) {
  m_jump_force = force;
}

float Player::get_air_jump_force() const {
  return m_air_jump_force;
}

void Player::set_air_jump_force(float force) {
  m_air_jump_force = force;
}

void Player::set_state(PlayerState* state) {
  m_state = state;
}

float Player::get_gravity() const {
  return m_gravity;
}

void Player::set_gravity(float gravity) {
  m_gravity = gravity;
}

Vector2 Player::get_h_direction() const {
  return m_animatedSprite2D->is_flipped_h() ? Vector2{-1, 0} : Vector2{1, 0};
}

void Player::set_weapon_monitoring(bool can_monitor) const {
  m_weapon->set_monitoring(can_monitor);
}