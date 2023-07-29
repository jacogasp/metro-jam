#ifndef COREGAME_ENEMY_SOLDIER_HPP
#define COREGAME_ENEMY_SOLDIER_HPP

#include <godot_cpp/classes/character_body2d.hpp>

#include "damageable.hpp"
#include "macros.hpp"
#include "state.hpp"
#include "timer.hpp"

using namespace godot;

class EnemySoldier;
using EnemySoldierState = State<EnemySoldier>;

class EnemySoldier
    : public CharacterBody2D
    , public Damageable {
  GDCLASS_V2(EnemySoldier, CharacterBody2D)

  enum Direction { left = -1, right = 1 };

 private:
  float m_gravity               = 980;
  int m_total_health            = 100;
  int m_health                  = 100;
  EnemySoldierState* m_state    = &EnemySoldier::idle;
  TimeDelta m_fire_rate         = 1;
  Vector2 m_hit_bounce_velocity = {100, -100};
  Direction m_direction         = right;
  Timer m_fire_timer;

  static void _bind_methods();

 public:
  void _ready() override;
  void _process(double) override;
  void _physics_process(double) override;
  void set_state(EnemySoldierState* state);
  void take_hit(int damage, Vector2 const& from_direction) override;
  void set_gravity(float gravity);
  [[nodiscard]] float get_gravity() const;
  void set_total_health(int health);
  [[nodiscard]] int get_total_health() const;
  void set_fire_rate(double rate);
  [[nodiscard]] double get_fire_rate() const;
  void set_hit_bounce_velocity(Vector2 const& velocity);
  [[nodiscard]] Vector2 get_hit_bounce_velocity() const;
  [[nodiscard]] Direction get_direction() const;

  void fire();

  // States
  struct IdleState : EnemySoldierState {
    void update(EnemySoldier& enemy) const override;
  };

  struct FallingState : EnemySoldierState {
    void update(EnemySoldier& enemy) const override;
  };

  struct DyingState : EnemySoldierState {
    void update(EnemySoldier& enemy) const override;
  };

 public:
  static IdleState idle;
  static DyingState dying;
  static FallingState falling;

  // Commands
  struct IdleCommand {
    void operator()(EnemySoldier& enemy) const;
  };

  struct DieCommand {
    void operator()(EnemySoldier& enemy) const;
  };

  struct HitCommand {
    void operator()(EnemySoldier& enemy, Vector2 const& from_direction) const;
  };
};

#endif // COREGAME_ENEMY_SOLDIER_HPP
