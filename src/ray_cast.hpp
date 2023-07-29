#ifndef COREGAME_RAY_CAST_HPP
#define COREGAME_RAY_CAST_HPP

#include "profiler.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>
#include <godot_cpp/classes/world2d.hpp>
#include <godot_cpp/godot.hpp>
#include <cinttypes>

using namespace godot;

static inline bool ray_hits(Vector2 position, Vector2 target,
                            uint32_t collision_mask, Ref<World2D>& world) {
  PROFILE_FUNCTION();
  auto query =
      PhysicsRayQueryParameters2D::create(position, target, collision_mask);
  query->set_hit_from_inside(true);
  auto const space_state = world->get_direct_space_state();
  auto const result      = space_state->intersect_ray(query);
  auto const collider    = Node::cast_to<Node2D>(result["collider"]);
  return collider != nullptr;
}

#endif // COREGAME_RAY_CAST_HPP
