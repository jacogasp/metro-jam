#include "player.hpp"
#include "constants.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <cassert>

StandingState Player::standing      = StandingState();
RunningState Player::running        = RunningState();
JumpingState Player::jumping        = JumpingState();
AirJumpingState Player::air_jumping = AirJumpingState();
AttackState Player::attacking       = AttackState();

void Player::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_speed"), &Player::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
  ClassDB::bind_method(D_METHOD("set_gravity"), &Player::set_gravity);
  ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
  ClassDB::bind_method(D_METHOD("get_jump_force"), &Player::get_jump_force);
  ClassDB::bind_method(D_METHOD("set_jump_force"), &Player::set_jump_force);
  ClassDB::bind_method(D_METHOD("get_air_jump_force"),
                       &Player::get_air_jump_force);
  ClassDB::bind_method(D_METHOD("set_air_jump_force"),
                       &Player::set_air_jump_force);

  // clang-format off
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"),
               "set_gravity","get_gravity");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_force"),
               "set_jump_force","get_jump_force");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "air_jump_force"),
               "set_air_jump_force", "get_air_jump_force");
  // clang-format on
}

void Player::_ready() {
  PROFILE_FUNCTION()
  m_logger = core_game::LoggerLocator::getService();

  m_animatedSprite2D = get_node<AnimatedSprite2D>("AnimatedSprite2D");
  m_animatedSprite2D->play("Idle");
  m_weapon = get_node<Weapon>("Weapon");
  m_weapon->set_monitoring(false);
  load();
  m_logger->info("Player ready.");
}

void Player::_process(float delta) {
  PROFILE_FUNCTION()
   if (Engine::get_singleton()->is_editor_hint())
    return;
  auto input = Input::get_singleton();
  if (input->is_action_just_pressed("save")) {
    save();
  } else if (input->is_action_just_pressed("load")) {
    load();
  }
}

void Player::_physics_process(float delta) {
  PROFILE_FUNCTION()
  assert(m_state);
  if (Engine::get_singleton()->is_editor_hint())
    return;
  auto input = Input::get_singleton();
  m_state->handleInput(*this, *input);
  m_state->update(*this);

  auto const g = get_velocity().y + m_gravity * delta;
  Vector2 velocity{0, g};

  if (input->is_action_pressed("move_left")) {
    velocity.x = -m_speed;
  } else if (input->is_action_pressed("move_right")) {
    velocity.x = m_speed;
  }

  auto is_flipped = m_animatedSprite2D->is_flipped_h() && velocity.x == 0;
  m_animatedSprite2D->set_flip_h(velocity.x < 0 || is_flipped);
  auto is_flipped_weapon = m_weapon->is_flipped_h() && velocity.x == 0;
  m_weapon->set_flip_h(velocity.x < 0 || is_flipped_weapon);
  set_velocity(velocity);
  move_and_slide();
}

void Player::save() {
  PROFILE_FUNCTION()
  try {
    Dictionary d{};
    d["pos.x"] = get_position().x;
    d["pos.y"] = get_position().y;
    auto path  = core_game::SAVINGS_DIRECTORY + "/player.json";
    core_game::FileWriter file{path};
    file.write(core_game::dict_to_json(d));
    m_logger->info("Player state saved.");
  } catch (const std::exception& e) {
    m_logger->error(e.what());
  }
}

void Player::load() {
  PROFILE_FUNCTION()
  const auto path = core_game::SAVINGS_DIRECTORY + "/player.json";
  if (!std::filesystem::exists(path)) {
    m_logger->error("Save file not found");
    return;
  }
  try {
    core_game::FileReader file{path};
    const auto body         = file.get();
    const auto player_state = core_game::json_to_dict(body);
    if (!player_state.has("pos.x")) {
      m_logger->error("Player saving data corrupted");
      return;
    }
    const float x = static_cast<float>(player_state["pos.x"]);
    const float y = static_cast<float>(player_state["pos.y"]);
    set_position({x, y});
    m_logger->info("Loaded saved player state");
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

void Player::set_state(PlayerState* state) {
  m_state = state;
}

float Player::get_gravity() const {
  return m_gravity;
}

void Player::set_gravity(float gravity) {
  m_gravity = gravity;
}

Vector2 Player::get_h_direction() const {
  return m_animatedSprite2D->is_flipped_h() ? Vector2{-1, 0} : Vector2{1, 0};
}

void Player::set_weapon_monitoring(bool can_monitor) const {
  m_weapon->set_monitoring(can_monitor);
}
