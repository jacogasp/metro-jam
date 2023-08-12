#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

class Weapon {
 public:
  virtual ~Weapon()                                  = default;
  virtual void fire(godot::Vector2 const& direction) = 0;
};

#endif // WEAPON_HPP