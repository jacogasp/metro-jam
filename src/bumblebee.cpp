#include "bumblebee.hpp"
#include "io.hpp"
#include <godot_cpp/classes/engine.hpp>

void BumbleBee::_bind_methods() {
}

IdleState BumbleBee::idle             = IdleState();
JumpState BumbleBee::jumping          = JumpState();
OnWallState BumbleBee::on_wall        = OnWallState();
static auto constexpr idle_command    = IdleCommand();
static auto constexpr jump_command    = JumpCommand();
static auto constexpr flip_command    = FlipCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  m_front_ray = get_node<RayCast2D>("FrontRay");
  set_velocity({0, 0});
  m_timer.set_callback([this]() { jump_command(*this); });
  m_timer.set_timeout(2);
  m_timer.set_repeat(true);
  m_timer.start();
}

void BumbleBee::_physics_process(float delta) {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto velocity = get_velocity();
  velocity.y += 980 * delta;
  set_velocity(velocity);
  m_timer.tick(delta);
  m_state->update(*this);
  m_animated_sprite2D->set_flip_h(m_direction == left);
  move_and_slide();
}

void BumbleBee::set_state(BumbleBeeState* state) {
  m_state = state;
}

// Commands
void IdleCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_animated_sprite2D->play("Idle");
  bumble_bee.set_velocity({0, 0});
  bumble_bee.set_state(&BumbleBee::idle);
}

void JumpCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_animated_sprite2D->play("JumpIn");
  Vector2 jump_velocity{150 * static_cast<float>(bumble_bee.m_direction), -300};
  auto velocity = bumble_bee.get_velocity() + jump_velocity;
  bumble_bee.set_velocity(velocity);
  bumble_bee.set_state(&BumbleBee::jumping);
}

void FlipCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_direction = bumble_bee.m_direction == BumbleBee::Direction::right
                             ? BumbleBee::Direction::left
                             : BumbleBee::Direction::right;
  bumble_bee.set_state(&BumbleBee::on_wall);
}

// BumbleBee's States
void IdleState::update(BumbleBee& bumble_bee) const {
}

void JumpState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.is_on_wall()) {
    flip_command(bumble_bee);
    return;
  }

  if (bumble_bee.get_velocity().y > 0 && bumble_bee.is_on_floor()) {
    idle_command(bumble_bee);
  }
}

void OnWallState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.get_velocity().y < 0) {
    bumble_bee.set_state(&BumbleBee::jumping);
  } else if (bumble_bee.is_on_floor()) {
    idle_command(bumble_bee);
  }
}