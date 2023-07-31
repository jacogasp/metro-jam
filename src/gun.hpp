#ifndef COREGAME_GUN_HPP
#define COREGAME_GUN_HPP

#include "macros.hpp"
#include "timer.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

using namespace godot;

class Gun
    : public Node2D
    , public Weapon {
  GDCLASS_V2(Gun, Node2D)
 private:
  Ref<PackedScene> m_bullet{nullptr};
  Vector2 m_bullet_impulse{100, 0};
  Timer m_cooldown_timer;
  bool m_cooling_down = false;

  static void _bind_methods();

 public:
  void _ready() override;
  void _process(double t) override;
  void set_bullet(const Ref<PackedScene>& scene);
  [[nodiscard]] Ref<PackedScene> get_bullet() const;
  void fire(Vector2 const& target) override;
  void set_bullet_impulse(Vector2 const& impulse);
  [[nodiscard]] Vector2 get_bullet_impulse() const;
  void set_cooldown(double cooldown);
  [[nodiscard]] double get_cooldown() const;
  [[nodiscard]] bool cooling_down() const;
};

#endif // COREGAME_GUN_HPP
