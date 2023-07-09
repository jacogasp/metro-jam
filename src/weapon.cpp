#include "coin_spawner.hpp"
#include "weapon.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void Weapon::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_damage", "damage"), &Weapon::set_damage);
  ClassDB::bind_method(D_METHOD("get_damage"), &Weapon::get_damage);
  ClassDB::bind_method(D_METHOD("_on_body_entered", "body"),
                       &Weapon::_on_body_entered);

  ADD_PROPERTY(PropertyInfo(Variant::INT, "damage"), "set_damage",
               "get_damage");
}

void Weapon::_ready() {
  m_collisionShape2D = get_node<CollisionShape2D>("CollisionShape2D");
}


void Weapon::_physics_process(float delta) {
  auto collision_shape_position = m_collisionShape2D->get_position();
  float new_collider_pos_x;

  if (m_flip_h && collision_shape_position.x > 0
      || !m_flip_h && collision_shape_position.x < 0) {
    new_collider_pos_x = -collision_shape_position.x;
  } else {
    new_collider_pos_x = collision_shape_position.x;
  }

  m_collisionShape2D->set_position(
      Vector2(new_collider_pos_x, collision_shape_position.y));
}

int Weapon::get_damage() const {
  return m_damage;
}

void Weapon::set_damage(int damage) {
  m_damage = damage;
}

void Weapon::set_flip_h(bool value) {
  m_flip_h = value;
}

bool Weapon::is_flipped_h() const {
  return m_flip_h;
}

void Weapon::_on_body_entered(Node2D* body) {
  const auto groups = body->get_groups();
  if (groups.has("Damageable")) {
    auto coin_spawner = body->get_node<CoinSpawner>("CoinSpawner");
    coin_spawner->spawn();
    body->queue_free();
  }
}