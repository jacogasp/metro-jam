#ifndef COREGAME_GRENADE_HPP
#define COREGAME_GRENADE_HPP

#include "timer.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>

using namespace godot;

class Grenade : public RigidBody2D {
  GDCLASS(Grenade, RigidBody2D);
 private:
  Timer m_timer{};
  static void _bind_methods();
 public:
  void _ready() override;
  void explode();
};

#endif // COREGAME_GRENADE_HPP
