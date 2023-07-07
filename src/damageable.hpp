#ifndef IDAMAGEABLE_HPP
#define IDAMAGEABLE_HPP

#include <godot_cpp/classes/node2d.hpp>

class IDamageable {
    int life;
public:
    virtual void take_hit(int damage) = 0;
};

#endif // DAMAGEABLE_HPP