#include "poison_ranger.hpp"
#include "gun.hpp"
#include "health_bar.hpp"
#include "io.hpp"
#include "ray_cast.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader_material.hpp>

using core_game::to_str;

PoisonRanger::IdleState PoisonRanger::idle       = PoisonRanger::IdleState();
PoisonRanger::FallingState PoisonRanger::falling = PoisonRanger::FallingState();
PoisonRanger::AlertState PoisonRanger::in_alert  = PoisonRanger::AlertState();
PoisonRanger::FiringState PoisonRanger::firing   = PoisonRanger::FiringState();
PoisonRanger::ChasingState PoisonRanger::chasing = PoisonRanger::ChasingState();
PoisonRanger::DyingState PoisonRanger::dying     = PoisonRanger::DyingState();
static auto constexpr go_idle                    = PoisonRanger::IdleCommand();
static auto constexpr fall                       = PoisonRanger::FallCommand();
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
  BIND_PROPERTY(PoisonRanger, running_speed, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, shooting_range, Variant::FLOAT);
  BIND_PROPERTY(PoisonRanger, target_lost_distance, Variant::FLOAT);
  ClassDB::bind_method(D_METHOD("take_hit"), &PoisonRanger::take_hit);
  ClassDB::bind_method(D_METHOD("acquire_target"),
                       &PoisonRanger::acquire_target);
  ClassDB::bind_method(D_METHOD("release_target"),
                       &PoisonRanger::release_target);
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
  go_idle(*this);
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
  m_health -= damage;
  hit(*this, from_direction);
  auto health_bar = get_node<HealthBar>("HealthBar");
  if (health_bar) {
    health_bar->set_value(m_health);
  }
  if (m_health <= 0) {
    die(*this);
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
  auto gun = get_node<Gun>("Gun");
  if (gun) {
    gun->fire({});
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
  m_direction = direction;
}

void PoisonRanger::acquire_target(Node2D* target) {
  m_target = target;
}

void PoisonRanger::release_target(Node2D* target) {
  if (target == m_target) {
    m_target = nullptr;
  }
}

static Node2D* player_is_visible(PoisonRanger& enemy, Vector2 const& target) {
  static uint32_t collision_mask = 65535 ^ enemy.get_collision_layer();
  auto world                     = enemy.get_world_2d();
  auto from                      = enemy.get_global_position();
  auto hit_target = ray_hits(from, target, collision_mask, world);

  return (hit_target && hit_target->is_in_group("Player")) ? hit_target
                                                           : nullptr;
}

static void look_at(PoisonRanger& enemy, Node2D* target) {
  auto enemy_position          = enemy.get_global_position();
  auto target_position         = target->get_global_position();
  auto const direction         = target_position.x < enemy_position.x
                                   ? PoisonRanger::left
                                   : PoisonRanger::right;
  auto const direction_changed = direction != enemy.get_direction();
  enemy.set_direction(direction);

  if (direction_changed) {
    auto scale = enemy.get_scale();
    scale.x *= -1;
    enemy.set_scale(scale);

    auto gun = enemy.get_node<Gun>("Gun");
    if (gun) {
      auto bullet_impulse = gun->get_bullet_impulse();
      bullet_impulse.x *= -1;
      std::cout << direction << ' ' << bullet_impulse.x << '\n';
      gun->set_bullet_impulse(bullet_impulse);
    }
  }
}

// States
void PoisonRanger::IdleState::update(PoisonRanger& enemy) const {
  std::cout << "idle\n";
  auto const enemy_pos = enemy.get_global_position();
  auto target          = enemy_pos;
  target.x += enemy.get_shooting_range() * enemy.get_scale().x;
  auto player = player_is_visible(enemy, target);
  enemy.acquire_target(player);
  if (!player) {
    return;
  }
  auto const player_pos = player->get_global_position();
  auto const distance   = std::abs(player_pos.x - enemy_pos.x);
  if (distance < enemy.get_shooting_range()) {
    open_fire(enemy);
  }
}

void PoisonRanger::FallingState::update(PoisonRanger& enemy) const {
  auto const v_y = enemy.get_velocity().y;
  if (v_y >= 0 && enemy.is_on_floor()) {
    go_idle(enemy);
  } else {
    fall(enemy);
  }
}

void PoisonRanger::AlertState::update(PoisonRanger& enemy) const {
  std::cout << "alert\n";
  auto const v_y = enemy.get_velocity().y;
  if (!(v_y >= 0 && enemy.is_on_floor())) {
    fall(enemy);
    return;
  }
  auto target = enemy.get_global_position();
  target.x += enemy.get_shooting_range();
  auto player = player_is_visible(enemy, target);
  if (player) {
    enemy.m_target = player;
    open_fire(enemy);
    return;
  }

  target = enemy.get_global_position();
  target.x -= enemy.get_shooting_range();
  player = player_is_visible(enemy, target);
  if (player) {
    enemy.m_target = player;
    open_fire(enemy);
    return;
  }

  enemy.set_velocity({0, 0});
}

void PoisonRanger::FiringState::update(PoisonRanger& enemy) const {
  std::cout << "firing\n";
  auto target = enemy.m_target;
  ::look_at(enemy, target);

  auto velocity = enemy.get_velocity();
  velocity.x    = 0;
  enemy.set_velocity(velocity);

  auto const player = player_is_visible(enemy, target->get_global_position());
  if (player == nullptr) {
    return;
  }

  auto const distance =
      std::abs(enemy.get_global_position().x - player->get_global_position().x);

  if (distance > enemy.get_shooting_range()) {
    enemy.set_state(&PoisonRanger::chasing);
    close_fire(enemy);
    return;
  }
  if (distance > enemy.get_target_lost_distance()) {
    go_idle(enemy);
  }
}

void PoisonRanger::ChasingState::update(PoisonRanger& enemy) const {
  std::cout << "chasing\n";
  auto target       = enemy.m_target;
  auto const player = player_is_visible(enemy, target->get_global_position());
  if (!player) {
    go_idle(enemy);
    return;
  }
  ::look_at(enemy, enemy.m_target);

  auto const target_distance =
      std::abs(enemy.get_global_position().x - player->get_global_position().x);
  auto const lost_distance = enemy.get_target_lost_distance();

  if (target_distance < enemy.get_shooting_range()) {
    open_fire(enemy);
    return;
  }

  if (target_distance > lost_distance) {
    enemy.release_target(player);
    go_idle(enemy);
    return;
  }

  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Run");
  }
  auto const running_speed = enemy.get_running_speed();
  auto velocity            = enemy.get_velocity();
  velocity.x = enemy.get_direction() == PoisonRanger::left ? -running_speed
                                                           : running_speed;
  enemy.set_velocity(velocity);
}

void PoisonRanger::DyingState::update(PoisonRanger& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite && animated_sprite->is_playing()) {
    return;
  }
  enemy.queue_free();
}

// Commands

void PoisonRanger::IdleCommand::operator()(PoisonRanger& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Idle");
  }
  enemy.set_velocity({0, 0});
  enemy.set_state(&PoisonRanger::idle);
  enemy.release_target(enemy.m_target);
  close_fire(enemy);
}

void PoisonRanger::FallCommand::operator()(PoisonRanger& enemy) const {
}

void PoisonRanger::DieCommand::operator()(PoisonRanger& enemy) const {
  auto animated_sprite = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (animated_sprite) {
    animated_sprite->play("Die");
  }
  enemy.set_state(&PoisonRanger::dying);
}

void PoisonRanger::HitCommand::operator()(PoisonRanger& enemy,
                                          Vector2 const& from_direction) const {
  auto const vfx = enemy.get_node<AnimationPlayer>("VFX");
  if (vfx) {
    vfx->play("Hit");
  }
  auto const position      = enemy.get_global_position();
  auto const hit_direction = position.x < from_direction.x ? left : right;
  auto bounce_velocity     = enemy.get_hit_bounce_velocity();
  bounce_velocity.x *= static_cast<float>(hit_direction);
  auto velocity = enemy.get_velocity();
  velocity += bounce_velocity;
  enemy.set_velocity(velocity);
  enemy.set_state(&PoisonRanger::in_alert);
}

void PoisonRanger::OpenFireCommand::operator()(PoisonRanger& enemy) const {
  enemy.m_fire_timer.start();
  enemy.set_state(&PoisonRanger::firing);
  auto const as = enemy.get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (as) {
    as->play("Idle");
  }
}

void PoisonRanger::CloseFireCommand::operator()(PoisonRanger& enemy) const {
  enemy.m_fire_timer.stop();
}
