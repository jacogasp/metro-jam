#include "player.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

StandingState Player::standing      = StandingState();
WalkingState Player::walking        = WalkingState();
JumpingState Player::jumping        = JumpingState();
AirJumpingState Player::air_jumping = AirJumpingState();

void Player::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_speed"), &Player::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
}

void Player::_ready() {
  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
}

void Player::_physics_process(float delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  assert(m_state);
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
  abs(velocity.x) > 0 ? m_animatedSprite2D->play() : m_animatedSprite2D->stop();

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

void Player::set_state(PlayerState* state) {
  m_state = state;
}

void Player::set_animation(const char* animation) const {
  m_animatedSprite2D->set_animation(animation);
}