#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <memory>

using namespace godot;

class Weapon : public Area2D {
    GDCLASS(Weapon, Area2D);
    int m_damage                         = 10;
    bool m_flip_h                        = false;
    CollisionShape2D* m_collisionShape2D = nullptr;

    static void _bind_methods();

public:
    void _ready() override;
    void _physics_process(float delta);
    [[nodiscard]] float get_damage() const;
    void set_damage(float damage);
    void set_flip_h(bool value);
    bool is_flipped_h();
    void _on_body_entered(Node2D* body);
};

#endif // WEAPON_HPP