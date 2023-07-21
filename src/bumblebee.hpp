#ifndef COREGAME_BUMBLEBEE_HPP
#define COREGAME_BUMBLEBEE_HPP

#include "damageable.hpp"
#include "macros.hpp"
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
using HealthBar = TextureProgressBar;

// States
class BumbleBeeState {
 public:
  virtual ~BumbleBeeState() = default;
  virtual void handle_input(BumbleBee&, Input*) const {
  }
  virtual void update(BumbleBee&) const {
  }
};

class IdleState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class WalkingState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class JumpState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class OnWallState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class DyingState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class BumbleBee final
    : public CharacterBody2D
    , public Damageable {
  GDCLASS_V2(BumbleBee, CharacterBody2D)

  enum Direction { left = -1, right = 1 };

  static IdleState idle_state;
  static WalkingState walking_state;
  static JumpState jumping;
  static OnWallState on_wall;
  static DyingState dying;

  BumbleBeeState* m_state{&BumbleBee::walking_state};
  AnimatedSprite2D* m_animated_sprite2D{nullptr};
  Area2D* m_aggro_area{nullptr};
  AnimationPlayer* m_vfx{nullptr};
  HealthBar* m_health_bar{nullptr};
  Node2D* m_target{nullptr};

  Timer m_timer{};
  Direction m_direction        = left;
  Vector2 m_jump_velocity      = Vector2{200, -200};
  TimeDelta m_jump_interval_s  = 2;
  Vector2 m_bounds             = Vector2{};
  int m_total_health           = 100;
  int m_health                 = 100;
  Timer m_health_bar_timer{};
  TimeDelta m_health_bar_show_time = 2;

  static void _bind_methods();
  void _ready() override;
  void _process(double) override;
  void _physics_process(double delta) override;
  void set_state(BumbleBeeState* state);
  void take_hit(int damage) override;
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
};

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

struct FlipCommand {
  void operator()(BumbleBee& bumble_bee) const;
};

struct DieCommand {
  void operator()(BumbleBee& bumble_bee) const;
};

#endif // COREGAME_BUMBLEBEE_HPP
