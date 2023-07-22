#ifndef COREGAME_GUN_HPP
#define COREGAME_GUN_HPP

#include "macros.hpp"
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

  static void _bind_methods();

 public:
  void set_bullet(const Ref<PackedScene>& scene);
  [[nodiscard]] Ref<PackedScene> get_bullet() const;
  void fire(Vector2 const& target) override;
};

#endif // COREGAME_GUN_HPP
