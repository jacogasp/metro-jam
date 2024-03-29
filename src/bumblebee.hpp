#ifndef COREGAME_BUMBLEBEE_HPP
#define COREGAME_BUMBLEBEE_HPP

#include "damageable.hpp"
#include "health_bar.hpp"
#include "macros.hpp"
#include "state.hpp"
#include "timer.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <variant>

using namespace godot;

class BumbleBee;
using BumbleBeeState = State<BumbleBee>;

class BumbleBee final
    : public CharacterBody2D
    , public Damageable {
  GDCLASS_V2(BumbleBee, CharacterBody2D)

  enum Direction { left = -1, right = 1 };

 private:
  BumbleBeeState* m_state{&BumbleBee::walking_state};
  AnimatedSprite2D* m_animated_sprite2D{nullptr};
  Area2D* m_aggro_area{nullptr};
  AnimationPlayer* m_vfx{nullptr};
  HealthBar* m_health_bar{nullptr};
  Node2D* m_target{nullptr};

  Timer m_timer{};
  Timer m_health_bar_timer{};
  Direction m_direction       = right;
  float m_speed               = 25;
  Vector2 m_jump_velocity     = Vector2{200, -200};
  TimeDelta m_jump_interval_s = 2;
  Vector2 m_bounds            = Vector2{};
  int m_total_health          = 100;
  int m_health                = 100;
  float m_hit_bounce_factor   = 1.0;

  static void _bind_methods();

 public:
  void _ready() override;
  void _physics_process(double delta) override;
  void set_state(BumbleBeeState* state);
  void take_hit(int damage, Vector2 const& from_direction) override;
  void set_speed(float speed);
  [[nodiscard]] float get_speed() const;
  void set_jump_velocity(Vector2 const& velocity);
  [[nodiscard]] Vector2 get_jump_velocity() const;
  void set_jump_interval(TimeDelta interval);
  [[nodiscard]] TimeDelta get_jump_interval() const;
  void on_body_entered(Node* node);
  void set_hit_animation_time(float t) const;
  [[nodiscard]] float get_hit_animation_time() const;
  void look_at(Node2D* node);
  void acquire_target(Node2D* node);
  void release_target(Node2D* node);
  void update_bounds();
  [[nodiscard]] AnimatedSprite2D* get_animated_sprite() const;
  void set_direction(Direction const& direction);
  [[nodiscard]] Direction const& get_direction() const;
  void set_hit_bounce_factor(float f);
  [[nodiscard]] float get_hit_bounce_factor() const;
  [[nodiscard]] Node2D* get_target();
  [[nodiscard]] Timer& get_attack_timer();
  [[nodiscard]] Vector2& get_bounds();

 private:
  struct IdleState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

  class WalkingState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

  struct AlertState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

  class AttackState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

  class OnWallState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

  class DyingState : public BumbleBeeState {
    void update(BumbleBee& bumble_bee) const override;
  };

 public:
  static IdleState idle_state;
  static WalkingState walking_state;
  static AttackState attack_state;
  static OnWallState on_wall;
  static DyingState dying;

  // Commands
  struct IdleCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };

  struct WalkCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };

  struct JumpCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };

  struct AlertCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };

  struct AttackCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };

  struct HitCommand {
    void operator()(BumbleBee& bumble_bee, int damage) const;
  };

  struct DieCommand {
    void operator()(BumbleBee& bumble_bee) const;
  };
};

#endif // COREGAME_BUMBLEBEE_HPP
