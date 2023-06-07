#ifndef CHEST_HPP
#define CHEST_HPP

#include "coin_spawner.hpp"
#include "damageable.hpp"
#include <godot_cpp/classes/static_body2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <memory>

using namespace godot;

class Chest : public StaticBody2D, public IDamageable {
    GDCLASS(Chest, StaticBody2D);
    int m_life{20};
    Ref<PackedScene> m_packed_scene{nullptr};
    int m_quantity{0};
    CollisionShape2D* m_collisionShape2D{nullptr};
    CoinSpawner* m_coin_spawner{nullptr};

    static void _bind_methods();

public:
    void _ready() override;
    void set_life(int life);
    int get_life();
    void set_quantity(int quantity);
    [[nodiscard]] int get_quantity() const;
    void set_packed_scene(const Ref<PackedScene>& packed_scene);
    Ref<PackedScene> get_packed_scene();

    void take_hit(int damage) override;
};

#endif // CHEST_HPP
