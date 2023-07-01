#ifndef COREGAME_BUMBLEBEE_HPP
#define COREGAME_BUMBLEBEE_HPP

#include "commands.hpp"
#include "timer.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <future>

using namespace godot;

class BumbleBee;
class IdleState;
class JumpState;

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

class BumbleBee final : public CharacterBody2D {
  GDCLASS(BumbleBee, CharacterBody2D);
  struct TimerIntervalRange {
    int min;
    int max;
  };

  static IdleState idle;
  static JumpState jumping;

  BumbleBeeState* m_state{&BumbleBee::idle};
  AnimatedSprite2D* m_animated_sprite2D{nullptr};
  TimerIntervalRange m_jump_timer_interval_ms{1000, 2000};
  Timer m_timer{};

  static void _bind_methods();
  void _ready() override;
  void _physics_process(float delta);
  void set_state(BumbleBeeState* state);
};

// Commands
template<>
void IdleCommand::execute(BumbleBee& game_actor) const;

template<>
void JumpCommand::execute(BumbleBee& game_actor) const;

template<>
void JumpOutCommand::execute(BumbleBee& game_actor) const;

#endif // COREGAME_BUMBLEBEE_HPP
