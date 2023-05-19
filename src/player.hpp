#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "player_state.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <memory>

using namespace godot;

class Player : public CharacterBody2D {
  GDCLASS(Player, CharacterBody2D);
  float m_gravity                      = 500;
  float m_jump                         = 350;
  float m_speed                        = 0;
  AnimatedSprite2D* m_animatedSprite2D = nullptr;
  PlayerState* m_state{&Player::standing};

  static void _bind_methods();

 public:
  static StandingState standing;
  static WalkingState walking;
  static JumpingState jumping;
  static AirJumpingState air_jumping;

  void _ready() override;
  void _physics_process(float delta);
  [[nodiscard]] float get_speed() const;
  void set_speed(float speed);
  void set_state(PlayerState* state);
  void set_animation(const char* animation) const;
};

#endif