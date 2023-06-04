#ifndef CHEST_HPP
#define CHEST_HPP

#include "damageable.hpp"
#include <godot_cpp/classes/static_body2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <memory>

using namespace godot;

class Chest : public StaticBody2D, public IDamageable {
    GDCLASS(Chest, StaticBody2D);
    int m_life                         = 20;
    CollisionShape2D* m_collisionShape2D = nullptr;

    static void _bind_methods();

public:
    void set_life(int life);
    int get_life();
    void take_hit(int damage) override;
};

#endif // CHEST_HPP
