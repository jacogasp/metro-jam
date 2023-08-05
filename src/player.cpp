#include "player.hpp"
#include "constants.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "profiler.hpp"
#include "ray_cast.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/shape2d.hpp>
#include <cassert>
#include <memory>

StandingState Player::standing      = StandingState();
RunningState Player::running        = RunningState();
JumpingState Player::jumping        = JumpingState();
FallingState Player::falling        = FallingState();
AirJumpingState Player::air_jumping = AirJumpingState();
AttackState Player::attacking       = AttackState();
SlideState Player::sliding          = SlideState();

Path Player::savings_path = []() {
  auto path = core_game::SAVINGS_DIRECTORY;
  path.append("player.json");
  return path;
}();

static void flip_h(RayCast2D& ray) {
  auto position        = ray.get_position();
  auto target_position = ray.get_target_position();
  position.x *= -1;
  target_position.x *= -1;
  ray.set_position(position);
  ray.set_target_position(target_position);
}

void Player::_bind_methods() {
  BIND_PROPERTY(Player, max_lives, Variant::INT);
  BIND_PROPERTY(Player, speed, Variant::FLOAT);
  BIND_PROPERTY(Player, gravity, Variant::FLOAT);
  BIND_PROPERTY(Player, jump_force, Variant::FLOAT);
  BIND_PROPERTY(Player, air_jump_force, Variant::FLOAT);
  BIND_PROPERTY(Player, slide_speed, Variant::FLOAT);
  BIND_PROPERTY(Player, attack_range, Variant::FLOAT);
  BIND_PROPERTY(Player, attack_strength, Variant::INT);
  BIND_PROPERTY(Player, skin_depth, Variant::FLOAT);

  ClassDB::bind_method(D_METHOD("pick"), &Player::pick);
  ClassDB::bind_method(D_METHOD("set_hit_animation_time"),
                       &Player::set_hit_animation_time);
  ClassDB::bind_method(D_METHOD("get_hit_animation_time"),
                       &Player::get_hit_animation_time);

  ClassDB::bind_method(D_METHOD("hit"), &Player::hit);
  ClassDB::bind_method(D_METHOD("is_life_full"), &Player::is_life_full);
  ClassDB::bind_method(D_METHOD("add_life"), &Player::add_life);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hit_animation_time",
                            PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_hit_animation_time", "get_hit_animation_time");

  ADD_SIGNAL(MethodInfo("player_hit"));
  ADD_SIGNAL(MethodInfo("player_gains_life"));
}

Player::Player() {
  Immunity* immunity = memnew(Immunity);
  immunity->set_name("Immunity");
  immunity->set_process(true);
  call_deferred("add_child", immunity);
}

void Player::_ready() {
  PROFILE_FUNCTION();
  m_logger           = core_game::LoggerLocator::getService();
  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animatedSprite2D->play("Idle");
  m_material = m_animatedSprite2D->get_material();
  m_animatedSprite2D->set_material(m_material);
  m_interaction_ray = get_node<RayCast2D>("InteractionRay");
  m_vfx             = get_node<AnimationPlayer>("VFX");
  m_audio_footsteps = get_node<AudioStreamPlayer>("Audio/Footsteps");
  m_audio_jump      = get_node<AudioStreamPlayer>("Audio/Jump");
  auto shape = get_node<CollisionShape2D>("CollisionShape2D")->get_shape();
  m_bounds   = shape->get_rect();
  add_child(&wrench_weapon);
  load();
  m_logger->info("Player ready.");
}

static void maybe_save(Player& player, Input& input) {
  if (input.is_action_just_pressed("save")) {
    player.save();
  } else if (input.is_action_just_pressed("load")) {
    player.load();
  }
}

static bool check_interaction(Input& input, RayCast2D& ray) {
  PROFILE_FUNCTION();
  if (input.is_action_just_pressed("interact") && ray.is_colliding()) {
    auto item = ray.get_collider();
    if (item) {
      item->call("interact");
      auto parent = Object::cast_to<Area2D>(item)->get_parent();
      if (parent && parent->get_parent()) {
        auto node = parent->get_parent();
        node->call("interact");
      }
      return true;
    }
  }
  return false;
}

void Player::_process(double) {
  PROFILE_FUNCTION();
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto input = Input::get_singleton();
  maybe_save(*this, *input);
  check_interaction(*input, *m_interaction_ray);
}

void Player::_physics_process(double delta) {
  PROFILE_FUNCTION();
  assert(m_state);
  if (Engine::get_singleton()->is_editor_hint())
    return;

  auto const g = get_velocity().y + m_gravity * static_cast<float>(delta);
  Vector2 velocity{0, g};

  auto input = Input::get_singleton();

  if (input->is_action_pressed("move_left")) {
    velocity.x = -m_speed;
    set_direction(left);
  } else if (input->is_action_pressed("move_right")) {
    velocity.x = m_speed;
    set_direction(right);
  }

  set_velocity(velocity);
  m_state->handleInput(*this, *input);
  m_state->update(*this);
  move_and_slide();
}

void Player::set_direction(Player::Direction direction) {
  if (direction == m_direction) {
    return;
  }
  m_direction = direction;
  auto scale  = get_scale();
  scale.x *= -1;
  set_scale(scale);
  ::flip_h(*m_interaction_ray);

  auto grenade_launcher = get_node<Gun>("GrenadeLauncher");
  if (grenade_launcher) {
    auto position = grenade_launcher->get_position();
    position.x *= -1;
    grenade_launcher->set_position(position);
  }
}

Player::Direction Player::get_direction() const {
  return m_direction;
}

void Player::set_max_lives(int lives) {
  m_max_lives = lives;
}

int Player::get_max_lives() const {
  return m_max_lives;
}

int Player::get_current_life() const {
  return m_current_life;
}

void Player::add_life() {
  ++m_current_life;
  if (m_current_life > m_max_lives) {
    m_current_life = m_max_lives;
  }
  emit_signal("player_gains_life");
}

void Player::loose_life() {
  --m_current_life;
  if (m_current_life < 0) {
    m_current_life = 0;
  }
}

bool Player::is_life_full() const {
  return m_current_life == m_max_lives;
}

void Player::save() {
  PROFILE_FUNCTION();
  try {
    Dictionary d{};
    d["pos.x"] = get_position().x;
    d["pos.y"] = get_position().y;
    core_game::FileWriter file{savings_path};
    file.write(core_game::dict_to_json(d));
    static const auto msg{std::string{"Player saved player state to "}
                          + savings_path.string()};
    m_logger->info(msg);
  } catch (const std::exception& e) {
    m_logger->error(e.what());
  }
}

void Player::load() {
  PROFILE_FUNCTION();
  if (!std::filesystem::exists(savings_path)) {
    m_logger->warn("Save file not found");
    return;
  }
  try {
    core_game::FileReader file{savings_path};
    const auto body         = file.get();
    const auto player_state = core_game::json_to_dict(body);
    if (!player_state.has("pos.x")) {
      m_logger->error("Player saving data corrupted");
      return;
    }
    const float x = static_cast<float>(player_state["pos.x"]);
    const float y = static_cast<float>(player_state["pos.y"]);
    set_position({x, y});
    static const auto msg{std::string{"Loaded saved player state from "}
                          + savings_path.string()};
    m_logger->info(msg);
  } catch (const std::exception& e) {
    m_logger->error(e.what());
  }
}

// Setters and getters
float Player::get_speed() const {
  return m_speed;
}

void Player::set_speed(float speed) {
  m_speed = speed;
}

float Player::get_jump_force() const {
  return m_jump_force;
}

void Player::set_jump_force(float force) {
  m_jump_force = force;
}

float Player::get_air_jump_force() const {
  return m_air_jump_force;
}

void Player::set_air_jump_force(float force) {
  m_air_jump_force = force;
}

float Player::get_attack_range() const {
  return m_attack_range;
}

void Player::set_attack_range(float attack_range) {
  m_attack_range = attack_range;
}

int Player::get_attack_strength() const {
  return Player::wrench_weapon.get_damages();
}

void Player::set_attack_strength(int strength) {
  Player::wrench_weapon.set_damages(strength);
}

float Player::get_skin_depth() const {
  return m_skin_depth;
}

void Player::set_skin_depth(float depth) {
  m_skin_depth = depth;
}

void Player::set_state(PlayerState* state) {
  m_state = state;
}

float Player::get_gravity() const {
  return m_gravity;
}

void Player::set_gravity(float gravity) {
  m_gravity = gravity;
}

void Player::pick(Node2D* node) {
  auto const position = node->get_global_position();
  auto old_parent = node->get_parent();
  old_parent->remove_child(node);
  add_child(node);
  node->set_global_position(position);
}

void Player::hit() {
  auto immunity = get_node<Immunity>("Immunity");
  if (immunity->is_active()) {
    return;
  }
  loose_life();
  emit_signal("player_hit");
  get_node<AudioStreamPlayer>("Audio/Ouch")->play();
  m_vfx->play("Hit");
}

void Player::set_hit_animation_time(float t) {
  if (m_animatedSprite2D == nullptr)
    return;
  m_material->set_shader_parameter("time", t);
}

float Player::get_hit_animation_time() const {
  if (m_material == nullptr)
    return 0.0f;
  auto const time = m_material->get_shader_parameter("time");
  return static_cast<float>(time);
}

float Player::get_slide_speed() const {
  return m_slide_speed;
}

void Player::set_slide_speed(float slide_speed) {
  m_slide_speed = slide_speed;
}

float Player::get_ground_position() const {
  auto cs          = get_node<CollisionShape2D>("CollisionShape2D");
  auto const shape = cs->get_shape();
  auto const rect  = shape->get_rect();
  return rect.position.y + rect.size.height / 2;
}

bool Player::is_on_ground() {
  PROFILE_FUNCTION();
  auto const pos        = get_global_position();
  auto world            = get_world_2d();
  auto const half_width = m_bounds.size.x / 2;
  Vector2 target_left{pos.x - half_width + m_skin_depth, pos.y + m_skin_depth};
  Vector2 target_right{pos.x + half_width - m_skin_depth, pos.y + m_skin_depth};
  auto hit_left  = ray_hits(pos, target_left, block_collision_mask, world);
  auto hit_right = ray_hits(pos, target_right, block_collision_mask, world);
  return hit_left || hit_right;
}
