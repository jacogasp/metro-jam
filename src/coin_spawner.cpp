#include "coin_spawner.hpp"
#include "coin.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>

void CoinSpawner::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_packed_scene", "scene"), &CoinSpawner::set_packed_scene);
    ClassDB::bind_method(D_METHOD("get_packed_scene"), &CoinSpawner::get_packed_scene);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "coin"), "set_packed_scene", "get_packed_scene");

    ClassDB::bind_method(D_METHOD("set_quantity", "quantity"), &CoinSpawner::set_quantity);
    ClassDB::bind_method(D_METHOD("get_quantity"), &CoinSpawner::get_quantity);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "quantity"), "set_quantity", "get_quantity");

    // ClassDB::bind_method(D_METHOD("spawn"), &CoinSpawner::spawn);
}

void CoinSpawner::_ready() {}

void CoinSpawner::set_quantity(int quantity) {
    m_quantity = quantity;
}

int CoinSpawner::get_quantity() const {
    return m_quantity;
}

void CoinSpawner::set_packed_scene(const Ref<PackedScene>& packed_scene) {
    m_packed_scene = packed_scene;
}

Ref<PackedScene> CoinSpawner::get_packed_scene() {
    return m_packed_scene;
}

void CoinSpawner::spawn() {
    auto random = godot::RandomNumberGenerator();

    for (int i = 0; i < m_quantity; i++)
    {
        auto coin_scene = m_packed_scene->instantiate();
        auto room_scene = get_parent()->get_parent()->call_deferred("add_child", coin_scene);
        auto coin = Node::cast_to<Coin>(coin_scene);
        if (coin) {
            int velocity_x = random.randi_range(-250, 250);
            coin->set("position", get_parent()->get("position"));
            coin->set("linear_velocity", Vector2(velocity_x, -300));
        }
    }
}