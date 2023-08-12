#include "wrench.hpp"
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>
#include <godot_cpp/classes/world2d.hpp>

void Wrench::_bind_methods() {
  BIND_PROPERTY(Wrench, damages, godot::Variant::FLOAT);
}

void Wrench::fire(Vector2 const&) {
  auto area2d = get_node<Area2D>("Area2D");
  if (area2d) {
    auto bodies = area2d->get_overlapping_bodies();
    for (auto i = 0; i < bodies.size(); ++i) {
      auto body = cast_to<Node2D>(bodies[i]);
      body->call("take_hit", m_damages, get_global_position());
      auto audio = get_node<AudioStreamPlayer>("Audio/Fire");
      if (audio) {
        audio->play();
      }
    }
  }
}

void Wrench::set_damages(int damages) {
  m_damages = damages;
}

int Wrench::get_damages() const {
  return m_damages;
}
