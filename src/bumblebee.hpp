#ifndef COREGAME_BUMBLEBEE_HPP
#define COREGAME_BUMBLEBEE_HPP

#include "timer.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <variant>

using namespace godot;

class BumbleBee;

// States
class BumbleBeeState {
 public:
  virtual ~BumbleBeeState() = default;
  virtual void handle_input(BumbleBee& bumble_bee, Input* input) const {};
  virtual void update(BumbleBee& bumble_bee) const {};
};

class IdleState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class JumpState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class OnWallState : public BumbleBeeState {
  void update(BumbleBee& bumble_bee) const override;
};

class BumbleBee final : public CharacterBody2D {
  GDCLASS(BumbleBee, CharacterBody2D);
  struct TimerIntervalRange {
    int min;
    int max;
  };

  enum Direction { left = -1, right = 1 };

  static IdleState idle;
  static JumpState jumping;
  static OnWallState on_wall;

  BumbleBeeState* m_state{&BumbleBee::idle};
  AnimatedSprite2D* m_animated_sprite2D{nullptr};
  RayCast2D* m_front_ray{nullptr};
  TimerIntervalRange m_jump_timer_interval_ms{1000, 2000};
  Timer m_timer{};
  Direction m_direction{left};

  static void _bind_methods();
  void _ready() override;
  void _physics_process(float delta);
  void set_state(BumbleBeeState* state);
};

// Commands

struct IdleCommand {
  void operator()(BumbleBee& bumble_bee) const;
};


struct JumpCommand {
  void operator()(BumbleBee& bumble_bee) const;
};

struct FlipCommand  {
  void operator()(BumbleBee& bumble_bee) const;
};

using BumbleBeeCommand = std::variant<IdleCommand, JumpCommand, FlipCommand>;

#endif // COREGAME_BUMBLEBEE_HPP
