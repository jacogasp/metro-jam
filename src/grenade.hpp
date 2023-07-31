#ifndef COREGAME_GRENADE_HPP
#define COREGAME_GRENADE_HPP

#include "macros.hpp"
#include "timer.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>

using namespace godot;

class Grenade : public RigidBody2D {
  GDCLASS_V2(Grenade, RigidBody2D)

 private:
  Timer m_timer{};
  TimeDelta m_timeout{};
  int m_damages               = 50;
  bool m_exploding            = false;
  bool m_explode_on_collision = false;
  static void _bind_methods();

 public:
  void _ready() override;
  void _physics_process(double delta) override;
  void explode();
  void set_timeout(TimeDelta timeout);
  [[nodiscard]] TimeDelta get_timeout() const;
  void set_damages(int damages);
  [[nodiscard]] int get_damages() const;
  void set_explode_on_collision(bool explode_on_collision);
  [[nodiscard]] bool get_explode_on_collision() const;
  void body_entered(Node*);
};

#endif // COREGAME_GRENADE_HPP
