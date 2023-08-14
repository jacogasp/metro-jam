#ifndef COREGAME_POISON_RANGER_HPP
#define COREGAME_POISON_RANGER_HPP

#include <godot_cpp/classes/character_body2d.hpp>

#include "damageable.hpp"
#include "macros.hpp"
#include "state.hpp"
#include "timer.hpp"

using namespace godot;

class PoisonRanger;
using PoisonRangerState = State<PoisonRanger>;

class PoisonRanger
    : public CharacterBody2D
    , public Damageable {
  GDCLASS_V2(PoisonRanger, CharacterBody2D)

  enum Direction { left = -1, right = 1 };

 private:
  float m_gravity               = 980;
  int m_total_health            = 100;
  int m_health                  = 100;
  float m_shooting_range        = 100;
  float m_target_lost_distance  = 300;
  float m_walking_speed         = 50;
  float m_running_speed         = 100;
  PoisonRangerState* m_state    = &PoisonRanger::patrolling;
  TimeDelta m_fire_rate         = 1;
  Vector2 m_hit_bounce_velocity = {100, -100};
  Direction m_direction         = right;
  Node2D* m_target              = nullptr;
  Timer m_fire_timer;
  Vector2 m_bounds;

  static void _bind_methods();

 public:
  ~PoisonRanger() override;
  void _ready() override;
  void _process(double) override;
  void _physics_process(double) override;
  void set_state(PoisonRangerState* state);
  void take_hit(int damage, Vector2 const& from_direction) override;
  void on_body_entered(Node2D *body);
  void set_gravity(float gravity);
  [[nodiscard]] float get_gravity() const;
  void set_total_health(int health);
  [[nodiscard]] int get_total_health() const;
  void set_fire_rate(double rate);
  [[nodiscard]] double get_fire_rate() const;
  void set_hit_bounce_velocity(Vector2 const& velocity);
  [[nodiscard]] Vector2 get_hit_bounce_velocity() const;
  void set_hit_animation_time(float t) const;
  [[nodiscard]] float get_hit_animation_time() const;
  [[nodiscard]] Direction get_direction() const;
  void set_walking_speed(float speed);
  [[nodiscard]] float get_walking_speed() const;
  void set_running_speed(float speed);
  [[nodiscard]] float get_running_speed() const;
  void set_shooting_range(float distance);
  [[nodiscard]] float get_shooting_range() const;
  void set_target_lost_distance(float distance);
  [[nodiscard]] float get_target_lost_distance() const;
  void set_direction(PoisonRanger::Direction direction);
  Node2D* get_target() const;
  void acquire_target(Node2D* target);
  void release_target(Node2D* target);
  Vector2& get_bounds();
  void update_bounds();

  void fire();

  // States
  struct PatrollingState : PoisonRangerState {
    void update(PoisonRanger& ranger) const override;
  };

  struct FiringState : PoisonRangerState {
    void update(PoisonRanger& ranger) const override;
  };

  struct ChasingState : PoisonRangerState {
    void update(PoisonRanger& ranger) const override;
  };

  struct DyingState : PoisonRangerState {
    void update(PoisonRanger& ranger) const override;
  };

 public:
  static PatrollingState patrolling;
  static FiringState firing;
  static ChasingState chasing;
  static DyingState dying;

  friend struct OpenFireCommand;
  friend struct FiringState;

  // Commands
  struct DieCommand {
    void operator()(PoisonRanger& ranger) const;
  };

  struct HitCommand {
    void operator()(PoisonRanger& ranger, Vector2 const& from_direction) const;
  };

  struct OpenFireCommand {
    void operator()(PoisonRanger& ranger) const;
  };

  struct CloseFireCommand {
    void operator()(PoisonRanger& ranger) const;
  };
};

#endif // COREGAME_POISON_RANGER_HPP
