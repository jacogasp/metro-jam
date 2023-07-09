#include "bumblebee.hpp"
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader_material.hpp>

void BumbleBee::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_jump_velocity"),
                       &BumbleBee::set_jump_velocity);
  ClassDB::bind_method(D_METHOD("get_jump_velocity"),
                       &BumbleBee::get_jump_velocity);
  ClassDB::bind_method(D_METHOD("set_jump_interval"),
                       &BumbleBee::set_jump_interval);
  ClassDB::bind_method(D_METHOD("get_jump_interval"),
                       &BumbleBee::get_jump_interval);
  ClassDB::bind_method(D_METHOD("on_body_entered"),
                       &BumbleBee::on_body_entered);
  ClassDB::bind_method(D_METHOD("set_hit_animation_time"),
                       &BumbleBee::set_hit_animation_time);
  ClassDB::bind_method(D_METHOD("get_hit_animation_time"),
                       &BumbleBee::get_hit_animation_time);
  ClassDB::bind_method(D_METHOD("take_hit"), &BumbleBee::take_hit);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "jump_velocity"),
               "set_jump_velocity", "get_jump_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_interval"),
               "set_jump_interval", "get_jump_interval");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hit_animation_time"),
               "set_hit_animation_time", "get_hit_animation_time");
}

IdleState BumbleBee::idle_state = IdleState();
JumpState BumbleBee::jumping    = JumpState();
OnWallState BumbleBee::on_wall  = OnWallState();
DyingState BumbleBee::dying     = DyingState();
static auto constexpr idle      = IdleCommand();
static auto constexpr jump      = JumpCommand();
static auto constexpr flip      = FlipCommand();
static auto constexpr die       = DieCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  set_velocity({0, 0});

  m_health     = m_total_health;
  m_health_bar = get_node<HealthBar>("HealthBar");
  if (m_health_bar) {
    m_health_bar->set_max(m_total_health);
    m_health_bar->set_value(m_health);
  }

  m_vfx = get_node<AnimationPlayer>("VFX");

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

void BumbleBee::on_body_entered(Node* node) {
  if (node->is_in_group("Player")) {
    node->call("hit");
  }
}

void BumbleBee::take_hit(int damage) {
  m_health -= damage;
  if (m_health_bar) {
    m_health_bar->set_value(m_health);
  }
  if (m_vfx) {
    m_vfx->play("Hit");
  }
  if (m_health <= 0) {
    die(*this);
  }
}

void BumbleBee::set_hit_animation_time(float t) {
  if (m_animated_sprite2D) {
    Ref<ShaderMaterial> material = m_animated_sprite2D->get_material();
    material->set_shader_parameter("time", t);
  }
}

float BumbleBee::get_hit_animation_time() const {
  if (m_animated_sprite2D == nullptr)
    return 0.0;
  Ref<ShaderMaterial> material = m_animated_sprite2D->get_material();
  if (material == nullptr)
    return 0.0;
  float const time = material->get_shader_parameter("time");
  return time;
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

void DieCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.m_animated_sprite2D->play("Die");
  bumble_bee.set_state(&BumbleBee::dying);
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

void DyingState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.m_animated_sprite2D->is_playing()
      && bumble_bee.m_animated_sprite2D->get_animation().match("Die")) {
    return;
  }
  bumble_bee.queue_free();
}
