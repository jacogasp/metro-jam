#include "coin.hpp"
#include <godot_cpp/classes/engine.hpp>

void Coin::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_coin_area_body_entered", "body"),
                       &Coin::_on_coin_area_body_entered);
}

void Coin::_ready() {
  m_collisionShape2D = get_node<CollisionShape2D>("CollisionShape2D");
}

void Coin::_on_coin_area_body_entered(Node2D* body) {
  if (body->is_in_group("Player")) {
    queue_free();
  }
}
