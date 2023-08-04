#include "poison_ranger.hpp"
#include "gun.hpp"
#include "health_bar.hpp"
#include "io.hpp"
#include "ray_cast.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shape2d.hpp>

using core_game::to_str;
using Patrolling = PoisonRanger::PatrollingState;

Patrolling PoisonRanger::patrolling = PoisonRanger::PatrollingState();
PoisonRanger::FiringState PoisonRanger::firing   = PoisonRanger::FiringState();
PoisonRanger::ChasingState PoisonRanger::chasing = PoisonRanger::ChasingState();
PoisonRanger::DyingState PoisonRanger::dying     = PoisonRanger::DyingState();
static auto constexpr die                        = PoisonRanger::DieCommand();
static auto constexpr hit                        = PoisonRanger::HitCommand();
static auto constexpr open_fire  = PoisonRanger::OpenFireCommand();
static auto constexpr close_fire = PoisonRanger::CloseFireCommand();

PoisonRanger::~PoisonRanger() {
  release_target(m_target);
}

void PoisonRanger::_bind_methods() {
  BIND_PROPERTY(PoisonRanger, gravity, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, total_health, Variant::INT);
  BIND_PROPERTY(PoisonRanger, fire_rate, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, hit_bounce_velocity, Variant::VECTOR2);
  BIND_PROPERTY(PoisonRanger, hit_animation_time, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, walking_speed, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, running_speed, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, shooting_range, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, target_lost_distance, Variant::FLOAT);
  ClassDB::bind_method(D_METHOD("take_hit"), &PoisonRanger::take_hit);
  ClassDB::bind_method(D_METHOD("acquire_target"),
                       &PoisonRanger::acquire_target);
  ClassDB::bind_method(D_METHOD("release_target"),
                       &PoisonRanger::release_target);
}

static void go_patrolling(PoisonRanger& ranger) {
  ranger.update_bounds();
  close_fire(ranger);
  auto const as = ranger.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (as) {
    as->play("Run");
  }
}

static Node2D* player_is_visible(PoisonRanger& ranger, Vector2 const& target) {
  static uint32_t collision_mask = 65535 ^ ranger.get_collision_layer();
  auto world                     = ranger.get_world_2d();
  auto from                      = ranger.get_global_position();
  auto const cs = ranger.get_node<CollisionShape2D>("CollisionShape2D");
  if (cs) {
    from.y -= cs->get_shape()->get_rect().size.y;
  }
  auto hit_target = ray_hits(from, target, collision_mask, world);

  return (hit_target && hit_target->is_in_group("Player")) ? hit_target
                                                           : nullptr;
}

void PoisonRanger::_ready() {
  m_health        = m_total_health;
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_max(m_health);
    health_bar->set_value(m_health);
  }
  m_fire_timer.set_callback([&]() { fire(); });
  m_fire_timer.set_timeout(m_fire_rate);
  m_fire_timer.set_repeat(true);
  set_direction(right);
  go_patrolling(*this);
}

void PoisonRanger::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint())
    return;
  m_fire_timer.tick(delta);
}

void PoisonRanger::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  m_state->update(*this);
  auto velocity = get_velocity();
  velocity.y += m_gravity * static_cast<float>(delta);
  set_velocity(velocity);
  move_and_slide();
}

void PoisonRanger::take_hit(int damage, Vector2 const& from_direction) {
  hit(*this, from_direction);
  open_fire(*this);
  auto player = player_is_visible(*this, from_direction);
  if (player) {
    acquire_target(player);
  }
  set_state(&PoisonRanger::firing);

  m_health -= damage;
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_value(m_health);
  }
  if (m_health <= 0) {
    die(*this);
    set_state(&PoisonRanger::dying);
  }
}

void PoisonRanger::set_hit_animation_time(float t) const {
  auto animated_sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    Ref<ShaderMaterial> material = animated_sprite->get_material();
    material->set_shader_parameter("time", t);
  }
}

float PoisonRanger::get_hit_animation_time() const {
  auto animated_sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite == nullptr)
    return 0.0;
  Ref<ShaderMaterial> material = animated_sprite->get_material();
  if (material == nullptr)
    return 0.0;
  float const time = material->get_shader_parameter("time");
  return time;
}

void PoisonRanger::set_state(PoisonRangerState* state) {
  m_state = state;
}

void PoisonRanger::set_gravity(float gravity) {
  m_gravity = gravity;
}

float PoisonRanger::get_gravity() const {
  return m_gravity;
}

void PoisonRanger::set_total_health(int health) {
  m_total_health = health;
}

int PoisonRanger::get_total_health() const {
  return m_total_health;
}

void PoisonRanger::set_fire_rate(double rate) {
  m_fire_rate = rate;
}

double PoisonRanger::get_fire_rate() const {
  return m_fire_rate;
}

void PoisonRanger::fire() {
  auto const gun    = get_node<Gun>("Gun");
  auto const target = get_target();
  if (gun && target) {
    auto target_pos = target->get_global_position();
    auto const cs   = get_node<CollisionShape2D>("CollisionShape2D");
    if (cs) {
      target_pos.y -= cs->get_shape()->get_rect().size.height * 0.5f;
    }
    auto const direction = target_pos - gun->get_global_position();
    gun->fire(direction);
  }
  auto animated_sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Shoot");
  }
}

void PoisonRanger::set_hit_bounce_velocity(const Vector2& velocity) {
  m_hit_bounce_velocity = velocity;
}

Vector2 PoisonRanger::get_hit_bounce_velocity() const {
  return m_hit_bounce_velocity;
}

PoisonRanger::Direction PoisonRanger::get_direction() const {
  return m_direction;
}

void PoisonRanger::set_walking_speed(float speed) {
  m_walking_speed = speed;
}

float PoisonRanger::get_walking_speed() const {
  return m_walking_speed;
}

void PoisonRanger::set_running_speed(float speed) {
  m_running_speed = speed;
}

float PoisonRanger::get_running_speed() const {
  return m_running_speed;
}

void PoisonRanger::set_shooting_range(float distance) {
  m_shooting_range = distance;
}

float PoisonRanger::get_shooting_range() const {
  return m_shooting_range;
}

void PoisonRanger::set_target_lost_distance(float distance) {
  m_target_lost_distance = distance;
}

float PoisonRanger::get_target_lost_distance() const {
  return m_target_lost_distance;
}

void PoisonRanger::set_direction(PoisonRanger::Direction direction) {
  auto as = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (as) {
    as->set_flip_h(direction == left);
  }
  auto const direction_changed = direction != m_direction;
  if (direction_changed) {
    auto gun = get_node<Gun>("Gun");
    if (gun) {
      auto gun_position = gun->get_position();
      gun_position.x *= -1;
      gun->set_position(gun_position);
    }
  }
  m_direction = direction;
}

Node2D* PoisonRanger::get_target() const {
  return m_target;
}

void PoisonRanger::acquire_target(Node2D* target) {
  m_target = target;
}

void PoisonRanger::release_target(Node2D* target) {
  if (target == m_target) {
    m_target = nullptr;
  }
}

Vector2& PoisonRanger::get_bounds() {
  return m_bounds;
}

void PoisonRanger::update_bounds() {
  auto const cs = get_node<CollisionShape2D>("PatrolArea/CollisionShape2D");
  if (cs) {
    auto const rect = cs->get_shape()->get_rect();
    auto const pos  = get_position();
    m_bounds        = {pos.x - rect.size.x / 2, pos.x + rect.size.x / 2};
  }
}

static void look_at(PoisonRanger& ranger, Node2D* target) {
  auto ranger_position = ranger.get_global_position();
  auto target_position = target->get_global_position();
  auto const direction = target_position.x < ranger_position.x
                           ? PoisonRanger::left
                           : PoisonRanger::right;
  ranger.set_direction(direction);
}

static void flip(PoisonRanger& ranger) {
  auto position     = ranger.get_position();
  auto velocity     = ranger.get_velocity();
  const auto offset = 1.0f;
  PoisonRanger::Direction new_direction;
  if (ranger.get_direction() == PoisonRanger::right) {
    new_direction = PoisonRanger::left;
    position.x -= offset;
  } else {
    new_direction = PoisonRanger::right;
    position.x += offset;
  }
  velocity.x *= -1;
  ranger.set_direction(new_direction);
  ranger.set_position(position);
  ranger.set_velocity(velocity);
}

static void walk(PoisonRanger& ranger) {
  auto velocity    = ranger.get_velocity();
  const auto speed = ranger.get_walking_speed();
  velocity.x = ranger.get_direction() == PoisonRanger::right ? speed : -speed;
  ranger.set_velocity(velocity);
  // Keep in patrol area
  auto const x       = ranger.get_position().x;
  auto const& bounds = ranger.get_bounds();
  if (x < bounds.x || x > bounds.y || ranger.is_on_wall()) {
    flip(ranger);
    return;
  }
}

// States
void PoisonRanger::PatrollingState::update(PoisonRanger& ranger) const {
  auto const ray_length = ranger.get_shooting_range();
  auto target           = ranger.get_global_position();
  target.x += ranger.get_direction() == right ? ray_length : -ray_length;
  auto const player = player_is_visible(ranger, target);
  if (player) {
    ranger.acquire_target(player);
    open_fire(ranger);
    ranger.set_state(&PoisonRanger::firing);
    return;
  }
  if (ranger.is_on_floor()) {
    walk(ranger);
  }
}

void PoisonRanger::FiringState::update(PoisonRanger& ranger) const {
  auto target = ranger.get_target();
  if (target == nullptr) {
    go_patrolling(ranger);
    ranger.set_state(&PoisonRanger::patrolling);
    return;
  }
  ::look_at(ranger, target);

  auto velocity = ranger.get_velocity();
  if (velocity.y > 0) {
    velocity.x = 0;
    ranger.set_velocity(velocity);
  }

  auto const player = player_is_visible(ranger, target->get_global_position());
  if (player == nullptr) {
    ranger.release_target(target);
    return;
  }

  auto const distance = std::abs(ranger.get_global_position().x
                                 - player->get_global_position().x);

  if (distance > ranger.get_shooting_range()) {
    ranger.set_state(&PoisonRanger::chasing);
    close_fire(ranger);
    return;
  }
  if (distance > ranger.get_target_lost_distance()) {
    ranger.release_target(target);
    go_patrolling(ranger);
    ranger.set_state(&PoisonRanger::patrolling);
  }
}

void PoisonRanger::ChasingState::update(PoisonRanger& ranger) const {
  auto target       = ranger.m_target;
  auto const player = player_is_visible(ranger, target->get_global_position());
  if (!player) {
    go_patrolling(ranger);
    ranger.set_state(&PoisonRanger::patrolling);
    return;
  }
  ::look_at(ranger, ranger.m_target);

  auto const target_distance = std::abs(ranger.get_global_position().x
                                        - player->get_global_position().x);
  auto const lost_distance   = ranger.get_target_lost_distance();

  if (target_distance < ranger.get_shooting_range()) {
    open_fire(ranger);
    ranger.set_state(&PoisonRanger::firing);
    return;
  }

  if (target_distance > lost_distance) {
    go_patrolling(ranger);
    ranger.set_state(&PoisonRanger::patrolling);
    return;
  }

  auto animated_sprite = ranger.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Run");
  }
  auto const running_speed = ranger.get_running_speed();
  auto velocity            = ranger.get_velocity();
  velocity.x = ranger.get_direction() == PoisonRanger::left ? -running_speed
                                                            : running_speed;
  ranger.set_velocity(velocity);
}

void PoisonRanger::DyingState::update(PoisonRanger& ranger) const {
  auto animated_sprite = ranger.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite && animated_sprite->is_playing()) {
    return;
  }
  ranger.queue_free();
}

// Commands
void PoisonRanger::DieCommand::operator()(PoisonRanger& ranger) const {
  auto animated_sprite = ranger.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Die");
  }
  ranger.set_velocity({});
}

void PoisonRanger::HitCommand::operator()(PoisonRanger& ranger,
                                          Vector2 const& from_direction) const {
  auto const vfx = ranger.get_node<AnimationPlayer>("VFX");
  if (vfx) {
    vfx->play("Hit");
  }
  auto position        = ranger.get_global_position();
  auto bounce_velocity = ranger.get_hit_bounce_velocity();
  if (from_direction.x > position.x) {
    bounce_velocity *= -1;
  }
  auto velocity = ranger.get_velocity();
  velocity += bounce_velocity;
  ranger.set_position(position);
  ranger.set_velocity(velocity);
}

void PoisonRanger::OpenFireCommand::operator()(PoisonRanger& ranger) const {
  ranger.m_fire_timer.start();
  ranger.set_velocity({});
  auto const as = ranger.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (as) {
    as->play("Idle");
  }
}

void PoisonRanger::CloseFireCommand::operator()(PoisonRanger& ranger) const {
  ranger.m_fire_timer.stop();
}
