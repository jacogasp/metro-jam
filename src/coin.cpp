#include "coin.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/engine.hpp>


void Coin::_bind_methods() {
    // ClassDB::bind_method(D_METHOD("_on_body_entered"), &Coin::_on_body_entered);
    // ClassDB::bind_method(D_METHOD("_on_area_entered", "area"), &Coin::_on_area_entered);
    ClassDB::bind_method(D_METHOD("test"), &Coin::test);
}

void Coin::_ready() {
    UtilityFunctions::print("I'm Coin");
    m_collisionShape2D = get_node<CollisionShape2D>("CollisionShape2D");
}

// void Coin::_on_body_entered(Node2D node) {
//     UtilityFunctions::print("[Body Entered] Interaction with Coin");
// }

void Coin::_on_area_entered(Area2D* area) {
    UtilityFunctions::print("[Area Entered] Interaction with Coin");
}

void Coin::test() {
    UtilityFunctions::print("Test");
}
