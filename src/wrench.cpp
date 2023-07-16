#include "wrench.hpp"
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>
#include <godot_cpp/classes/world2d.hpp>

void Wrench::_bind_methods() {
}

void Wrench::fire(Vector2 const& target) {
  auto query = PhysicsRayQueryParameters2D::create(get_global_position(),
                                                   target, enemy_mask);
  query->set_hit_from_inside(true);
  auto space_state = get_world_2d()->get_direct_space_state();
  auto result      = space_state->intersect_ray(query);
  if (!result.is_empty()) {
    Node2D* collider   = cast_to<Node2D>(result["collider"]);
    collider->call("take_hit", m_damages);
  }
}

void Wrench::set_damages(int damages) {
  m_damages = damages;
}

int Wrench::get_damages() const {
  return m_damages;
}
