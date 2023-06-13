#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "persist.hpp"
#include "player_state.hpp"
#include "types.hpp"
#include "weapon.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <memory>

using namespace godot;
namespace core_game {
class LoggerService;
}

class Player
    : public CharacterBody2D
    , public Persist {
  GDCLASS(Player, CharacterBody2D)
  float m_gravity                      = 500;
  float m_jump_force                   = 350;
  float m_air_jump_force               = 350;
  float m_speed                        = 0;
  float m_landing_threshold            = 10;
  core_game::LoggerService* m_logger   = nullptr;
  AnimatedSprite2D* m_animatedSprite2D = nullptr;
  Weapon* m_weapon                     = nullptr;
  RayCast2D* m_landing_ray             = nullptr;
  PlayerState* m_state{&Player::standing};

  static void _bind_methods();

 public:
  static StandingState standing;
  static RunningState running;
  static JumpingState jumping;
  static AirJumpingState air_jumping;
  static AttackState attacking;
  static Path savings_path;

  friend class PlayerState;

  void _ready() override;
  void _process(float delta);
  void _physics_process(float delta);
  [[nodiscard]] float get_speed() const;
  void set_speed(float speed);
  [[nodiscard]] float get_gravity() const;
  void set_gravity(float gravity);
  [[nodiscard]] float get_jump_force() const;
  void set_jump_force(float force);
  [[nodiscard]] float get_air_jump_force() const;
  void set_air_jump_force(float force);
  void set_state(PlayerState* state);
  void set_weapon_monitoring(bool can_monitor) const;
  void hit();
  [[nodiscard]] Vector2 get_h_direction() const;
  void save() override;
  void load() override;
};

#endif