#ifndef COREGAME_GRENADE_HPP
#define COREGAME_GRENADE_HPP

#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>

using namespace godot;

class Grenade : public RigidBody2D {
  GDCLASS(Grenade, RigidBody2D);
  static void _bind_methods(){};
};

class GrenadeLauncher
    : public Node2D
    , public Weapon {
  GDCLASS(GrenadeLauncher, Node2D);
 private:
  Ref<PackedScene> m_grenade = nullptr;
  Vector2 m_impulse{};
  Vector2 m_velocity{};
  Vector2 m_prev_position{};

  static void _bind_methods();
 public:
  void _process(float);
  void fire(Vector2 const&) override;
  void set_grenade(const Ref<PackedScene>& scene);
  [[nodiscard]] Ref<PackedScene> get_grenade() const;
  void set_impulse(Vector2 const& impulse);
  [[nodiscard]] Vector2 get_impulse() const;
};

#endif // COREGAME_GRENADE_HPP
