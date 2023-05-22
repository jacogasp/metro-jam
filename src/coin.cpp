#include "coin.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void Coin::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_body_entered"), &Coin::_on_body_entered);
  ClassDB::bind_method(D_METHOD("_on_area_entered", "area"),
                       &Coin::_on_area_entered);
  ClassDB::bind_method(D_METHOD("test"), &Coin::test);
}

void Coin::_ready() {
  m_logger = core_game::Locator<core_game::Logger>::getService();
  m_logger->info("I'm Coin");
  m_collisionShape2D = get_node<CollisionShape2D>("CollisionShape2D");
}

void Coin::_on_body_entered(Node2D* node) {
  m_logger->debug("[Body Entered] Interaction with Coin");
}

void Coin::_on_area_entered(Area2D* area) {
  m_logger->debug("[Area Entered] Interaction with Coin");
}

void Coin::test() {
  m_logger->debug("test");
}
