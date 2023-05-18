#include "player.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void Player::_bind_methods() {
  //  ClassDB::bind_method(D_METHOD("_physics_process"),
  //  &Player::_physics_process);
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

  auto const input = Input::get_singleton();
  auto velocity    = get_velocity();

  auto jump = [&] {
    velocity.y -= m_jump;
    m_state = State::JUMPING;
  };

  auto air_jump = [&] {
    velocity.y -= m_jump;
    m_state = State::AIR_JUMPING;
  };

  switch (m_state) {
  case STANDING:
    if (velocity.x != 0) {
      m_state = WALKING;
    }
    if (input->is_action_just_pressed("jump")) {
      jump();
    }
    break;
  case JUMPING:
    if (input->is_action_just_pressed("jump")) {
      air_jump();
    }
    break;
  case AIR_JUMPING:
    break;
  case WALKING:
    m_animatedSprite2D->set_animation("Walk");
    if (input->is_action_just_pressed("jump")) {
      jump();
    }
  }

  velocity.x = 0;

  if (input->is_action_pressed("move_left")) {
    velocity.x = -m_speed;
  } else if (input->is_action_pressed("move_right")) {
    velocity.x = m_speed;
  }

  if (velocity.length() == 0) {
    m_state = STANDING;
  }

  abs(velocity.x) > 0 ? m_animatedSprite2D->play() : m_animatedSprite2D->stop();

  velocity.y += m_gravity * delta;
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