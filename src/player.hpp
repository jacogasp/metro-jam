#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "player_state.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <memory>

using namespace godot;

class Player : public CharacterBody2D {
  GDCLASS(Player, CharacterBody2D);
  float m_gravity                      = 500;
  float m_jump_force                   = 350;
  float m_air_jump_force               = 350;
  float m_speed                        = 0;
  bool m_can_attack                    = true;
  AnimatedSprite2D* m_animatedSprite2D = nullptr;
  Weapon* m_weapon                     = nullptr;
  PlayerState* m_state{&Player::standing};

  static void _bind_methods();

 public:
  static StandingState standing;
  static RunningState running;
  static JumpingState jumping;
  static AirJumpingState air_jumping;
  static AttackState attacking;

  friend class PlayerState;

  void _ready() override;
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
  void set_animation(const char* animation) const;
  void set_weapon_monitoring(bool can_monitor) const;
  [[nodiscard]] Vector2 get_h_direction() const;
};

#endif