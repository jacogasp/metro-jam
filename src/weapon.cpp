#include "weapon.hpp"
#include <godot_cpp/classes/engine.hpp>

void Weapon::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_damage", "damage"), &Weapon::set_damage);
    ClassDB::bind_method(D_METHOD("get_damage"), &Weapon::get_damage);
    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), 
                       &Weapon::_on_body_entered);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "damage"), "set_damage", "get_damage");
}

void Weapon::_ready() {
    m_collisionShape2D = get_node<CollisionShape2D>("CollisionShape2D");
}

float Weapon::get_damage() const {
    return m_damage;
}

void Weapon::set_damage(float damage) {
    m_damage = damage;
}

void Weapon::_on_body_entered(Node2D* body) {
    
    godot::TypedArray<Node> children = body->get_children();

    for (int i = 0; i < children.size(); i++) {
        auto child = children[i];
        if (child.stringify().contains("Damageable")) {
            // child.hit(m_damage);
            // body->get_node<Damageable>().hit(m_damage);
            body->queue_free();
            break;
        }
    }
}