#ifndef COREGAME_WRENCH_HPP
#define COREGAME_WRENCH_HPP

#include "weapon.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>

using namespace godot;

class Wrench
    : public Node2D
//    , public Weapon {

{GDCLASS(Wrench, Node2D);
  static const uint32_t enemy_mask = 1 << 12;

 private:
  int m_damages    = 0;
  RayCast2D* m_ray = nullptr;
  static void _bind_methods();

 public:
  void fire(Vector2 const& target);
  void set_damages(int damages);
  [[nodiscard]] int get_damages() const;
};

#endif // COREGAME_WRENCH_HPP