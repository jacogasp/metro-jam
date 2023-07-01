#include "bumblebee.hpp"
#include "io.hpp"
#include <godot_cpp/classes/engine.hpp>

void BumbleBee::_bind_methods() {
}

IdleState BumbleBee::idle             = IdleState();
JumpState BumbleBee::jumping          = JumpState();
static auto constexpr idle_command    = IdleCommand();
static auto constexpr jump_command    = JumpCommand();
static auto constexpr jumpout_command = JumpOutCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  set_velocity({0, 0});
  m_timer.set_callback([this]() { jump_command.execute(*this); });
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
  move_and_slide();
}

void BumbleBee::set_state(BumbleBeeState* state) {
  m_state = state;
}

// Commands
template<>
void IdleCommand::execute(BumbleBee& game_actor) const {
  game_actor.m_animated_sprite2D->play("Idle");
  game_actor.set_velocity({0, 0});
  game_actor.set_state(&BumbleBee::idle);
}

template<>
void JumpCommand::execute(BumbleBee& game_actor) const {
  game_actor.m_animated_sprite2D->play("JumpIn");
  game_actor.set_velocity(game_actor.get_velocity() + Vector2{-150, -300});
  game_actor.set_state(&BumbleBee::jumping);
}

template<>
void JumpOutCommand::execute(BumbleBee& game_actor) const {
  game_actor.m_animated_sprite2D->play("JumpOut");
}

// BumbleBee's States
void IdleState::update(BumbleBee& bumble_bee) const {
}

void JumpState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.get_velocity().y > 0 && bumble_bee.is_on_floor()) {
    idle_command.execute(bumble_bee);
  }
}
