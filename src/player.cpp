#include "player.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cassert>

StandingState Player::standing      = StandingState();
WalkingState Player::walking        = WalkingState();
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
  ClassDB::bind_method(D_METHOD("animation_finished"), &Player::animation_finished);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity",
               "get_gravity");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_force"), "set_jump_force",
               "get_jump_force");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "air_jump_force"),
               "set_air_jump_force", "get_air_jump_force");
}

void Player::_ready() {
  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animatedSprite2D->set_animation("Idle");
  m_animatedSprite2D->play();
}

void Player::_physics_process(float delta) {
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
  m_animatedSprite2D->play();
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

void Player::set_animation(const char* animation) const {
  m_animatedSprite2D->set_animation(animation);
}

bool Player::get_animation_finished_flag() const {
  return m_is_animation_finished;
}

void Player::set_animation_finished_flag(bool is_animation_finished) {
  m_is_animation_finished = is_animation_finished;
}

void Player::animation_finished() {
  auto frame_on_finish = m_animatedSprite2D->get_frame();
  set_animation_finished_flag(true);
}