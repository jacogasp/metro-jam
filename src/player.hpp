#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "gun.hpp"
#include "macros.hpp"
#include "persist.hpp"
#include "player_state.hpp"
#include "superpowers.hpp"
#include "types.hpp"
#include "wrench.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <cinttypes>

using namespace godot;
namespace core_game {
class LoggerService;
}

class Player
    : public CharacterBody2D
    , public Persist {
  GDCLASS_V2(Player, CharacterBody2D)

  enum Direction { left, right };
  static constexpr uint32_t block_collision_mask = 1 << 4;
  int m_max_lives                                = 5;
  int m_current_life                             = m_max_lives;
  float m_gravity                                = 500;
  float m_jump_force                             = 350;
  float m_air_jump_force                         = 350;
  float m_speed                                  = 0;
  float m_slide_speed                            = 400;
  float m_attack_range                           = 50;
  float m_skin_depth                             = 5;
  Direction m_direction                          = right;
  core_game::LoggerService* m_logger             = nullptr;
  AnimatedSprite2D* m_animatedSprite2D           = nullptr;
  RayCast2D* m_interaction_ray                   = nullptr;
  AnimationPlayer* m_vfx                         = nullptr;
  Ref<ShaderMaterial> m_material                 = nullptr;
  AudioStreamPlayer* m_audio_footsteps           = nullptr;
  AudioStreamPlayer* m_audio_jump                = nullptr;

  PlayerState* m_state{&Player::standing};

  Rect2 m_bounds{};

  Wrench wrench_weapon{};

  friend struct IdleCommand;
  friend struct JumpCommand;
  friend struct FallCommand;
  friend struct AirJumpCommand;
  friend struct RunCommand;
  friend struct AttackCommand;
  friend struct GrenadeCommand;
  friend struct SlideCommand;
  friend struct AttackState;
  friend struct SlideState;

  static void _bind_methods();

 public:
  static StandingState standing;
  static RunningState running;
  static JumpingState jumping;
  static FallingState falling;
  static AirJumpingState air_jumping;
  static AttackState attacking;
  static SlideState sliding;
  static Path savings_path;

  friend struct PlayerState;
  Player();
  void _ready() override;
  void _process(double delta) override;
  void _physics_process(double delta) override;
  void set_direction(Direction direction);
  [[nodiscard]] Direction get_direction() const;
  void set_max_lives(int lives);
  [[nodiscard]] int get_max_lives() const;
  [[nodiscard]] int get_current_life() const;
  void add_life();
  void loose_life();
  [[nodiscard]] bool is_life_full() const;
  [[nodiscard]] float get_speed() const;
  void set_speed(float speed);
  [[nodiscard]] float get_gravity() const;
  void set_gravity(float gravity);
  [[nodiscard]] float get_jump_force() const;
  void set_jump_force(float force);
  [[nodiscard]] float get_air_jump_force() const;
  void set_air_jump_force(float force);
  [[nodiscard]] float get_slide_speed() const;
  void set_slide_speed(float slide_speed);
  [[nodiscard]] float get_attack_range() const;
  void set_attack_range(float attack_range);
  [[nodiscard]] int get_attack_strength() const;
  void set_attack_strength(int attack_strength);
  [[nodiscard]] float get_skin_depth() const;
  void set_skin_depth(float depth);
  void set_state(PlayerState* state);
  void set_hit_animation_time(float t);
  [[nodiscard]] float get_hit_animation_time() const;
  [[nodiscard]] float get_ground_position() const;
  [[nodiscard]] bool is_on_ground();
  void pick(Node2D *node);
  void hit();
  void save() override;
  void load() override;
};

#endif