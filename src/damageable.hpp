#ifndef DAMAGEABLE_HPP
#define DAMAGEABLE_HPP

#include <godot_cpp/classes/node2d.hpp>

class Damageable {
 public:
  virtual ~Damageable()             = default;
  virtual void take_hit(int damage, const godot::Vector2& from_direction) = 0;
};

#endif // DAMAGEABLE_HPP