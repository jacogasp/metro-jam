#ifndef COIN_SPAWNER_HPP
#define COIN_SPAWNER_HPP

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include "macros.hpp"

using namespace godot;

class CoinSpawner : public Node2D {
    GDCLASS_V2(CoinSpawner, Node2D)
    Ref<PackedScene> m_packed_scene{nullptr};
    int m_quantity             = 0;
    static void _bind_methods();

public:
    void _ready() override;
    void set_quantity(int quantity);
    [[nodiscard]] int get_quantity() const;
    void set_packed_scene(const Ref<PackedScene>& packed_scene);
    Ref<PackedScene> get_packed_scene();
    void spawn();
};

#endif // COIN_SPAWNER_HPP
