#ifndef COIN_HPP
#define COIN_HPP

#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/area2d.hpp>

using namespace godot;
class Node2D;

class Coin : public Area2D {
    GDCLASS(Coin, Area2D);
    CollisionShape2D* m_collisionShape2D = nullptr;
    static void _bind_methods();

public:
    void _ready() override;
    void _on_body_entered(Node2D* node);
    void _on_area_entered(Area2D* area);
    void test();
};

#endif
