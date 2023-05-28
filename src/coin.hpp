#ifndef COIN_HPP
#define COIN_HPP

#include "logger.hpp"
#include <godot_cpp/classes/rigid_body2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>

using namespace godot;

class Coin : public RigidBody2D {
  GDCLASS(Coin, RigidBody2D);
  CollisionShape2D* m_collisionShape2D = nullptr;
  
  core_game::LoggerService* m_logger   = nullptr;

  static void _bind_methods();

 public:
  void _ready() override;
  void _on_coin_area_body_entered(Node2D* body);
};

#endif // COIN_HPP
