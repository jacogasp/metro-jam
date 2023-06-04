#include "chest.hpp"
#include <godot_cpp/classes/engine.hpp>

void Chest::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_life", "life"), &Chest::set_life);
    ClassDB::bind_method(D_METHOD("get_life"), &Chest::get_life);
    ClassDB::bind_method(D_METHOD("take_hit", "damage"), &Chest::take_hit);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "life"), "set_life", "get_life");
}

void Chest::set_life(int life) {
    m_life = life;
}

int Chest::get_life() {
    return m_life;
}

void Chest::take_hit(int damage) {
    m_life -= damage;
}
