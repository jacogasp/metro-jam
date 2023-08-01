#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class Weapon {
 public:
  virtual ~Weapon()                        = default;
  virtual void fire(Vector2 const& direction) = 0;
};

#endif // WEAPON_HPP