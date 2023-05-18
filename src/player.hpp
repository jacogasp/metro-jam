#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class Player : public CharacterBody2D {
  GDCLASS(Player, CharacterBody2D);
  enum State { STANDING, WALKING, JUMPING, AIR_JUMPING };

  float m_gravity                      = 500;
  float m_jump                         = 350;
  float m_speed                        = 0;
  AnimatedSprite2D* m_animatedSprite2D = nullptr;
  State m_state{STANDING};

  static void _bind_methods();

 public:
  void _ready() override;
  void _physics_process(float delta);
  [[nodiscard]] float get_speed() const;
  void set_speed(float speed);
};

#endif