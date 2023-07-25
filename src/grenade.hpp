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
  int m_damages = 50;
  static void _bind_methods();

 public:
  Grenade();
  void _ready() override;
  void _process(double delta) override;
  void explode();
  void set_timeout(TimeDelta timeout);
  [[nodiscard]] TimeDelta get_timeout() const;
  void set_damages(int damages);
  [[nodiscard]] int get_damages() const;
};

#endif // COREGAME_GRENADE_HPP
