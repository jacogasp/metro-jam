#include "bumblebee.hpp"
#include "ray_cast.hpp"
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shape2d.hpp>

void BumbleBee::_bind_methods() {
  BIND_PROPERTY(BumbleBee, speed, Variant::FLOAT);
  BIND_PROPERTY(BumbleBee, jump_velocity, Variant::VECTOR2);
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
BumbleBee::WalkingState BumbleBee::walking_state = BumbleBee::WalkingState();
BumbleBee::AttackState BumbleBee::attack_state   = BumbleBee::AttackState();
BumbleBee::OnWallState BumbleBee::on_wall        = BumbleBee::OnWallState();
BumbleBee::DyingState BumbleBee::dying           = BumbleBee::DyingState();
static auto constexpr idle                       = BumbleBee::IdleCommand();
static auto constexpr go_walk                    = BumbleBee::WalkCommand();
static auto constexpr jump                       = BumbleBee::JumpCommand();
static auto constexpr attack                     = BumbleBee::AttackCommand();
static auto constexpr hit                        = BumbleBee::HitCommand();
static auto constexpr die                        = BumbleBee::DieCommand();

void BumbleBee::_ready() {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_animated_sprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animated_sprite2D->play("Idle");
  auto material = m_animated_sprite2D->get_material();
  m_animated_sprite2D->set_material(material->duplicate());
  set_velocity({0, 0});
  set_direction(right);

  m_health     = m_total_health;
  m_health_bar = get_node<HealthBar>("HealthBar");
  if (m_health_bar) {
    m_health_bar->set_max(m_total_health);
    m_health_bar->set_value(m_health);
    m_health_bar->hide();
  }
  m_vfx = get_node<AnimationPlayer>("VFX");
  m_timer.set_callback([this]() { jump(*this); });
  m_timer.set_timeout(m_jump_interval_s);
  m_timer.set_repeat(true);
  m_timer.stop();
  update_bounds();
  go_walk(*this);
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
  move_and_slide();
}

void BumbleBee::set_state(BumbleBeeState* state) {
  m_state = state;
}

void BumbleBee::set_speed(float speed) {
  m_speed = speed;
}

float BumbleBee::get_speed() const {
  return m_speed;
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

void BumbleBee::take_hit(int damage, Vector2 const&) {
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
  set_direction(node_position.x < position.x ? left : right);
}

void BumbleBee::acquire_target(Node2D* node) {
  m_target = node;
}

void BumbleBee::release_target(Node2D* node) {
  m_timer.stop();
  update_bounds();
  go_walk(*this);
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
  auto velocity = get_velocity();
  auto v_x      = abs(velocity.x);
  if (direction == right) {
    velocity.x = v_x;
    m_animated_sprite2D->set_flip_h(false);
  } else {
    velocity.x = -v_x;
    m_animated_sprite2D->set_flip_h(true);
  }
  set_velocity(velocity);
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

Node2D* BumbleBee::get_target() {
  return m_target;
}

Timer& BumbleBee::get_attack_timer() {
  return m_timer;
}

Vector2& BumbleBee::get_bounds() {
  return m_bounds;
}

// Commands
void BumbleBee::IdleCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Idle");
  bumble_bee.set_velocity({0, 0});
  bumble_bee.set_state(&BumbleBee::idle_state);
}

void BumbleBee::WalkCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Walk");
  bumble_bee.get_attack_timer().stop();
}

void BumbleBee::JumpCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("JumpIn");
  auto jump_velocity = bumble_bee.get_jump_velocity();
  if (bumble_bee.get_direction() == left) {
    jump_velocity.x *= -1;
  }
  auto velocity = bumble_bee.get_velocity() + jump_velocity;
  bumble_bee.set_velocity(velocity);
}

void BumbleBee::AttackCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_attack_timer().start();
  bumble_bee.set_state(&BumbleBee::attack_state);
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
  bumble_bee.set_state(&BumbleBee::attack_state);
}

void BumbleBee::DieCommand::operator()(BumbleBee& bumble_bee) const {
  bumble_bee.get_animated_sprite()->play("Die");
  bumble_bee.set_state(&BumbleBee::dying);
}

static Node2D* player_is_visible(BumbleBee& bumble_bee, Vector2 const& target) {
  static uint32_t collision_mask = 65535 ^ bumble_bee.get_collision_layer();
  auto world                     = bumble_bee.get_world_2d();
  auto from                      = bumble_bee.get_global_position();
  auto hit_target = ray_hits(from, target, collision_mask, world);

  return (hit_target && hit_target->is_in_group("Player")) ? hit_target
                                                           : nullptr;
}

static void flip(BumbleBee& bumble_bee) {
  auto const new_direction = bumble_bee.get_direction() == BumbleBee::right
                               ? BumbleBee::left
                               : BumbleBee::right;
  bumble_bee.set_direction(new_direction);
  const auto offset = 1.0f;
  auto position     = bumble_bee.get_position();
  position.x += new_direction == BumbleBee::right ? +offset : -offset;
  bumble_bee.set_position(position);
}

static void walk(BumbleBee& bumble_bee) {
  auto velocity    = bumble_bee.get_velocity();
  const auto speed = bumble_bee.get_speed();
  velocity.x = bumble_bee.get_direction() == BumbleBee::right ? speed : -speed;
  bumble_bee.set_velocity(velocity);
  // Keep in aggro area
  auto const x       = bumble_bee.get_position().x;
  auto const& bounds = bumble_bee.get_bounds();

  if (x < bounds.x || x > bounds.y || bumble_bee.is_on_wall()) {
    flip(bumble_bee);
    return;
  }
}

// BumbleBee's States
void BumbleBee::IdleState::update(BumbleBee&) const {
}

void BumbleBee::WalkingState::update(BumbleBee& bumble_bee) const {
  auto const target = bumble_bee.get_target();
  if (target) {
    auto const player = player_is_visible(bumble_bee, target->get_position());
    if (player) {
      attack(bumble_bee);
      bumble_bee.set_state(&BumbleBee::attack_state);
      return;
    }
  }
  walk(bumble_bee);
}

void BumbleBee::AttackState::update(BumbleBee& bumble_bee) const {
  auto const target = bumble_bee.get_target();
  if (target == nullptr) {
    go_walk(bumble_bee);
    bumble_bee.set_state(&BumbleBee::walking_state);
    return;
  }
  // If player is hidden,walk
  auto const player =
      player_is_visible(bumble_bee, target->get_global_position());
  if (player == nullptr) {
    go_walk(bumble_bee);
    bumble_bee.set_state(&BumbleBee::walking_state);
    return;
  }
  // Player is now visible
  if (bumble_bee.is_on_wall()) {
    flip(bumble_bee);
  }
  // On Ground
  if (bumble_bee.is_on_floor() && bumble_bee.get_velocity().y > 0) {
    bumble_bee.set_velocity(Vector2{});
    bumble_bee.look_at(target);
    auto const animated_sprite =
        bumble_bee.get_node<AnimatedSprite2D>("AnimatedSprite2D");
    if (animated_sprite) {
      animated_sprite->play("Idle");
    }
  }
}

void BumbleBee::OnWallState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.get_velocity().y < 0) {
    bumble_bee.set_state(&BumbleBee::attack_state);
  } else if (bumble_bee.is_on_floor()) {
    idle(bumble_bee);
  }
}

void BumbleBee::DyingState::update(BumbleBee& bumble_bee) const {
  if (bumble_bee.m_animated_sprite2D->is_playing()
      && bumble_bee.m_animated_sprite2D->get_animation().match("Die")) {
    return;
  }
  auto loader               = ResourceLoader::get_singleton();
  Ref<PackedScene> resource = loader->load("res://scenes/life.tscn");
  auto life                 = cast_to<Node2D>(resource->instantiate());
  bumble_bee.get_parent()->add_child(life);
  life->set_global_position(bumble_bee.get_global_position());
  bumble_bee.queue_free();
}
