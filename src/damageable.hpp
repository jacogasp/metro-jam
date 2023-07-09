#ifndef IDAMAGEABLE_HPP
#define IDAMAGEABLE_HPP

#include <godot_cpp/classes/node2d.hpp>

class Damageable {
 public:
  virtual ~Damageable()             = default;
  virtual void take_hit(int damage) = 0;
};

#endif // DAMAGEABLE_HPP