#include "chest.hpp"
#include <godot_cpp/classes/engine.hpp>

void Chest::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_life", "life"), &Chest::set_life);
    ClassDB::bind_method(D_METHOD("get_life"), &Chest::get_life);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "life"), "set_life", "get_life");

    ClassDB::bind_method(D_METHOD("set_packed_scene", "scene"), &Chest::set_packed_scene);
    ClassDB::bind_method(D_METHOD("get_packed_scene"), &Chest::get_packed_scene);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "coin"), "set_packed_scene", "get_packed_scene");

    ClassDB::bind_method(D_METHOD("set_quantity", "quantity"), &Chest::set_quantity);
    ClassDB::bind_method(D_METHOD("get_quantity"), &Chest::get_quantity);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "quantity"), "set_quantity", "get_quantity");

    // ClassDB::bind_method(D_METHOD("take_hit", "damage"), &Chest::take_hit);
}

void Chest::_ready() {
    m_coin_spawner = get_node<CoinSpawner>("CoinSpawner");
    m_coin_spawner->set_packed_scene(m_packed_scene);
    m_coin_spawner->set_quantity(m_quantity);
}

void Chest::set_life(int life) {
    m_life = life;
}

int Chest::get_life() {
    return m_life;
}

void Chest::set_quantity(int quantity) {
    m_quantity = quantity;
}

int Chest::get_quantity() const {
    return m_quantity;
}

void Chest::set_packed_scene(const Ref<PackedScene>& packed_scene) {
    m_packed_scene = packed_scene;
}

Ref<PackedScene> Chest::get_packed_scene() {
    return m_packed_scene;
}

void Chest::take_hit(int damage, Vector2 const&) {
    m_life -= damage;
}
