#ifndef COREGAME_GRENADE_LAUNCHER_HPP
#define COREGAME_GRENADE_LAUNCHER_HPP

#include "macros.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

using namespace godot;

class GrenadeLauncher
    : public Node2D
    , public Weapon {
  GDCLASS_V2(GrenadeLauncher, Node2D)

 private:
  Ref<PackedScene> m_grenade = nullptr;
  Vector2 m_impulse{};
  Vector2 m_velocity{};
  Vector2 m_prev_position{};

  static void _bind_methods();

 public:
  void _process(double) override;
  void fire(Vector2 const&) override;
  void set_grenade(const Ref<PackedScene>& scene);
  [[nodiscard]] Ref<PackedScene> get_grenade() const;
  void set_impulse(Vector2 const& impulse);
  [[nodiscard]] Vector2 get_impulse() const;
};

#endif // COREGAME_GRENADE_LAUNCHER_HPP
