#include "bumblebee.hpp"
#include <godot_cpp/classes/engine.hpp>

void BumbleBee::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_jump_velocity"),
                       &BumbleBee::set_jump_velocity);
  ClassDB::bind_method(D_METHOD("get_jump_velocity"),
                       &BumbleBee::get_jump_velocity);
  ClassDB::bind_method(D_METHOD("set_jump_interval"),
                       &BumbleBee::set_jump_interval);
  ClassDB::bind_method(D_METHOD("get_jump_interval"),
                       &BumbleBee::get_jump_interval);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "jump_velocity"),
               "set_jump_velocity", "get_jump_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_interval"),
               "set_jump_interval", "get_jump_interval");
}

IdleState BumbleBee::idle_state = IdleState();
JumpState BumbleBee::jumping    = JumpState();
OnWallState BumbleBee::on_wall  = OnWallState();
static auto constexpr idle      = IdleCommand();
static auto constexpr jump      = JumpCommand();
static auto constexpr flip      = FlipCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  m_front_ray = get_node<RayCast2D>("FrontRay");
  set_velocity({0, 0});
  m_timer.set_callback([this]() { jump(*this); });
  m_timer.set_timeout(m_jump_interval_s);
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

void BumbleBee::set_jump_velocity(Vector2 const& velocity) {
  m_jump_velocity = velocity;
}

Vector2 BumbleBee::get_jump_velocity() const {
  return m_jump_velocity;
}
void BumbleBee::set_jump_interval(TimeDelta interval) {
  m_jump_interval_s = interval;
}

TimeDelta BumbleBee::get_jump_interval() const {
  return m_jump_interval_s;
}

// Commands
void IdleCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_animated_sprite2D->play("Idle");
  bumble_bee.set_velocity({0, 0});
  bumble_bee.set_state(&BumbleBee::idle_state);
}

void JumpCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_animated_sprite2D->play("JumpIn");
  auto direction     = static_cast<int>(bumble_bee.m_direction);
  auto jump_velocity = bumble_bee.m_jump_velocity;
  jump_velocity.x *= (float)direction;
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
    flip(bumble_bee);
    return;
  }
  if (bumble_bee.get_velocity().y > 0 && bumble_bee.is_on_floor()) {
    idle(bumble_bee);
  }
}

void OnWallState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.get_velocity().y < 0) {
    bumble_bee.set_state(&BumbleBee::jumping);
  } else if (bumble_bee.is_on_floor()) {
    idle(bumble_bee);
  }
}