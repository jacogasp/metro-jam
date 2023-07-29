#include "bumblebee.hpp"
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shape2d.hpp>

void BumbleBee::_bind_methods() {
  BIND_PROPERTY(BumbleBee, jump_velocity, Variant::FLOAT);
  BIND_PROPERTY(BumbleBee, jump_interval, Variant::FLOAT);
  BIND_PROPERTY(BumbleBee, hit_animation_time, Variant::FLOAT);
  BIND_PROPERTY(BumbleBee, hit_bounce_factor, Variant::FLOAT);

  ClassDB::bind_method(D_METHOD("on_body_entered"),
                       &BumbleBee::on_body_entered);
  ClassDB::bind_method(D_METHOD("take_hit"), &BumbleBee::take_hit);
  ClassDB::bind_method(D_METHOD("look_at"), &BumbleBee::look_at);
  ClassDB::bind_method(D_METHOD("acquire_target"), &BumbleBee::acquire_target);
  ClassDB::bind_method(D_METHOD("release_target"), &BumbleBee::release_target);
}

BumbleBee::IdleState BumbleBee::idle_state       = BumbleBee::IdleState();
BumbleBee::JumpState BumbleBee::jumping          = BumbleBee::JumpState();
BumbleBee::WalkingState BumbleBee::walking_state = BumbleBee::WalkingState();
BumbleBee::OnWallState BumbleBee::on_wall        = BumbleBee::OnWallState();
BumbleBee::DyingState BumbleBee::dying           = BumbleBee::DyingState();
static auto constexpr idle                       = BumbleBee::IdleCommand();
static auto constexpr walk                       = BumbleBee::WalkCommand();
static auto constexpr jump                       = BumbleBee::JumpCommand();
static auto constexpr hit                        = BumbleBee::HitCommand();
static auto constexpr flip                       = BumbleBee::FlipCommand();
static auto constexpr die                        = BumbleBee::DieCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  auto material = m_animated_sprite2D->get_material();
  m_animated_sprite2D->set_material(material->duplicate());
  set_velocity({0, 0});

  m_health     = m_total_health;
  m_health_bar = get_node<HealthBar>("HealthBar");
  if (m_health_bar) {
    m_health_bar->set_max(m_total_health);
    m_health_bar->set_value(m_health);
    m_health_bar->hide();
  }

  m_vfx = get_node<AnimationPlayer>("VFX");

  m_timer.set_callback([this]() {
    if (m_state != &jumping) {
      jump(*this);
    }
  });
  m_timer.set_timeout(m_jump_interval_s);
  m_timer.set_repeat(true);
  m_timer.stop();
  update_bounds();
  walk(*this);
}

void BumbleBee::_process(double) {
  if (m_target) {
    look_at(m_target);
  }
}

void BumbleBee::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto velocity = get_velocity();
  velocity.y += 980 * static_cast<float>(delta);
  set_velocity(velocity);
  m_timer.tick(delta);
  if (m_health_bar->is_visible()) {
    m_health_bar_timer.tick(delta);
  }
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
  hit(*this, damage);
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

void BumbleBee::set_hit_animation_time(float t) const {
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

void BumbleBee::look_at(Node2D* node) {
  auto position      = get_position();
  auto node_position = node->get_position();
  m_direction        = node_position.x < position.x ? left : right;
}

void BumbleBee::acquire_target(Node2D* node) {
  m_timer.start();
  m_target = node;
}

void BumbleBee::release_target(Node2D* node) {
  m_timer.stop();
  update_bounds();
  walk(*this);
  if (m_target == node) {
    m_target = nullptr;
  }
}

void BumbleBee::update_bounds() {
  auto const cs   = get_node<CollisionShape2D>("AggroArea/CollisionShape2D");
  auto const rect = cs->get_shape()->get_rect();
  auto const pos  = get_position();
  m_bounds        = {pos.x - rect.size.x / 2, pos.x + rect.size.x / 2};
}
AnimatedSprite2D* BumbleBee::get_animated_sprite() const {
  return m_animated_sprite2D;
}

void BumbleBee::set_direction(const BumbleBee::Direction& direction) {
  m_direction = direction;
}

const BumbleBee::Direction& BumbleBee::get_direction() const {
  return m_direction;
}
void BumbleBee::set_hit_bounce_factor(float f) {
  m_hit_bounce_factor = f;
}

float BumbleBee::get_hit_bounce_factor() const {
  return m_hit_bounce_factor;
}

// Commands
void BumbleBee::IdleCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Idle");
  bumble_bee.set_velocity({0, 0});
  bumble_bee.set_state(&BumbleBee::idle_state);
}

void BumbleBee::WalkCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Walk");
  bumble_bee.set_state(&BumbleBee::walking_state);
}

void BumbleBee::JumpCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("JumpIn");
  auto direction     = static_cast<int>(bumble_bee.get_direction());
  auto jump_velocity = bumble_bee.get_jump_velocity();
  jump_velocity.x *= static_cast<float>(direction);
  auto velocity = bumble_bee.get_velocity() + jump_velocity;
  bumble_bee.set_velocity(velocity);
  bumble_bee.set_state(&BumbleBee::jumping);
}

void BumbleBee::HitCommand::operator()(BumbleBee& bumble_bee,
                                       int damage) const {
  bumble_bee.get_animated_sprite()->play("JumpIn");
  auto const direction = -1.0f * static_cast<float>(bumble_bee.get_direction());
  auto jump_velocity   = bumble_bee.get_jump_velocity();
  jump_velocity.x *= direction;
  auto velocity = bumble_bee.get_velocity();
  velocity += jump_velocity * static_cast<float>(damage)
            * bumble_bee.m_hit_bounce_factor / 100.0f;
  bumble_bee.set_velocity(velocity);
  bumble_bee.set_state(&BumbleBee::jumping);
}

void BumbleBee::FlipCommand::operator()(BumbleBee& bumble_bee) const {
  auto direction = bumble_bee.get_direction() == BumbleBee::Direction::right
                     ? BumbleBee::Direction::left
                     : BumbleBee::Direction::right;
  bumble_bee.set_direction(direction);
  bumble_bee.set_state(&BumbleBee::on_wall);
}

void BumbleBee::DieCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Die");
  bumble_bee.set_state(&BumbleBee::dying);
}

// BumbleBee's States
void BumbleBee::IdleState::update(BumbleBee&) const {
}

void BumbleBee::WalkingState::update(BumbleBee& bumble_bee) const {
  auto velocity    = bumble_bee.get_velocity();
  const auto speed = 25.f;
  velocity.x = bumble_bee.m_direction == BumbleBee::right ? speed : -speed;

  auto const x = bumble_bee.get_position().x;
  if (x < bumble_bee.m_bounds.x) {
    velocity.x *= -1;
    bumble_bee.m_direction = BumbleBee::right;
  } else if (x > bumble_bee.m_bounds.y) {
    velocity.x *= -1;
    bumble_bee.m_direction = BumbleBee::left;
  }

  bumble_bee.set_velocity(velocity);
}

void BumbleBee::JumpState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.is_on_wall()) {
    flip(bumble_bee);
    return;
  }
  if (bumble_bee.get_velocity().y > 0 && bumble_bee.is_on_floor()) {
    idle(bumble_bee);
  }
}

void BumbleBee::OnWallState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.get_velocity().y < 0) {
    bumble_bee.set_state(&BumbleBee::jumping);
  } else if (bumble_bee.is_on_floor()) {
    idle(bumble_bee);
  }
}

void BumbleBee::DyingState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.m_animated_sprite2D->is_playing()
      && bumble_bee.m_animated_sprite2D->get_animation().match("Die")) {
    return;
  }
  bumble_bee.queue_free();
}
