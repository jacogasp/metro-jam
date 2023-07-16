#include "player.hpp"
#include "constants.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>
#include <godot_cpp/classes/world2d.hpp>
#include <cassert>

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

static void set_ray_h_length(RayCast2D& ray, float strength) {
  auto position = ray.get_position();
  Vector2 target_position;
  target_position.x =
      position.x > 0 ? position.x + strength : position.x - strength;
  target_position.y = position.y;
  ray.set_target_position(target_position);
}

static void flip_h(RayCast2D& ray) {
  auto position        = ray.get_position();
  auto target_position = ray.get_target_position();
  position.x *= -1;
  target_position.x *= -1;
  ray.set_position(position);
  ray.set_target_position(target_position);
}

void Player::_bind_methods() {
  BIND_PROPERTY(Player, speed, Variant::FLOAT);
  BIND_PROPERTY(Player, gravity, Variant::FLOAT);
  BIND_PROPERTY(Player, jump_force, Variant::FLOAT);
  BIND_PROPERTY(Player, air_jump_force, Variant::FLOAT);
  BIND_PROPERTY(Player, slide_speed, Variant::FLOAT);
  BIND_PROPERTY(Player, attack_range, Variant::FLOAT);
  BIND_PROPERTY(Player, attack_strength, Variant::INT);

  ClassDB::bind_method(D_METHOD("set_hit_animation_time"),
                       &Player::set_hit_animation_time);
  ClassDB::bind_method(D_METHOD("get_hit_animation_time"),
                       &Player::get_hit_animation_time);

  ClassDB::bind_method(D_METHOD("hit"), &Player::hit);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hit_animation_time",
                            PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_hit_animation_time", "get_hit_animation_time");

  ADD_SIGNAL(MethodInfo("player_hit"));
}

void Player::_ready() {
  PROFILE_FUNCTION()
  m_logger           = core_game::LoggerLocator::getService();
  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animatedSprite2D->play("Idle");
  m_material = m_animatedSprite2D->get_material()->duplicate();
  m_animatedSprite2D->set_material(m_material);
  m_grenade_launcher = get_node<GrenadeLauncher>("GrenadeLauncher");
  m_interaction_ray  = get_node<RayCast2D>("InteractionRay");
  m_vfx              = get_node<AnimationPlayer>("VFX");
  m_audio_footsteps  = get_node<AudioStreamPlayer>("Audio/Footsteps");
  m_audio_jump       = get_node<AudioStreamPlayer>("Audio/Jump");
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

void Player::_process(float delta) {
  PROFILE_FUNCTION()
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto input                = Input::get_singleton();
  auto const x_velocity     = get_velocity().x;
  auto const prev_direction = m_direction;

  maybe_save(*this, *input);
  check_interaction(*input, *m_interaction_ray);

  if (x_velocity > 0) {
    m_direction = right;
  } else if (x_velocity < 0) {
    m_direction = left;
  }

  if (prev_direction != m_direction) {
    flip_h();
  }
}

void Player::_physics_process(float delta) {
  PROFILE_FUNCTION()
  assert(m_state);
  if (Engine::get_singleton()->is_editor_hint())
    return;

  auto const g = get_velocity().y + m_gravity * delta;
  Vector2 velocity{0, g};

  auto input = Input::get_singleton();

  if (input->is_action_pressed("move_left")) {
    velocity.x = -m_speed;
  } else if (input->is_action_pressed("move_right")) {
    velocity.x = m_speed;
  }
  set_velocity(velocity);

  m_state->handleInput(*this, *input);
  m_state->update(*this);

  auto is_flipped = m_animatedSprite2D->is_flipped_h() && velocity.x == 0;
  m_animatedSprite2D->set_flip_h(velocity.x < 0 || is_flipped);
  move_and_slide();
}

void Player::save() {
  PROFILE_FUNCTION()
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
  PROFILE_FUNCTION()
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

void Player::set_state(PlayerState* state) {
  m_state = state;
}

float Player::get_gravity() const {
  return m_gravity;
}

void Player::set_gravity(float gravity) {
  m_gravity = gravity;
}

void Player::hit() {
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
  return (float)time;
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

void Player::flip_h() const {
  ::flip_h(*m_interaction_ray);
  if (m_grenade_launcher) {
    auto impulse = m_grenade_launcher->get_impulse();
    impulse.x *= -1;
    m_grenade_launcher->set_impulse(impulse);
  }
}

static bool ray_hits(Vector2 position, Vector2 target, Ref<World2D> world) {
  auto query = PhysicsRayQueryParameters2D::create(
      position, target, Player::block_collision_mask);
  query->set_hit_from_inside(true);
  auto space_state = world->get_direct_space_state();
  auto result      = space_state->intersect_ray(query);
  auto collider    = Node::cast_to<Node2D>(result["collider"]);
  return collider != nullptr;
}

bool Player::is_on_ground() const {
  auto pos              = get_global_position();
  auto world            = get_world_2d();
  auto const half_width = m_bounds.size.x / 2;
  Vector2 target_left{pos.x - half_width, pos.y + ground_skin_depth};
  Vector2 target_right{pos.x + half_width, pos.y + ground_skin_depth};
  auto hit_left  = ray_hits(pos, target_left, world);
  auto hit_right = ray_hits(pos, target_right, world);
  return hit_left || hit_right;
}
