#include "gate.hpp"
#include "logger.hpp"
#include "world.hpp"

void Gate::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_body_entered"), &Gate::_on_body_entered);
  ClassDB::bind_method(D_METHOD("_on_body_exited"), &Gate::_on_body_exited);
  ClassDB::bind_method(D_METHOD("set_next_room"), &Gate::set_next_room);
  ClassDB::bind_method(D_METHOD("get_next_room"), &Gate::get_next_room);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "next_room"), "set_next_room",
               "get_next_room");
  ADD_SIGNAL(MethodInfo("go_to_next_room",
                        PropertyInfo(Variant::OBJECT, "next_room")));
}

void Gate::_ready() {
  m_logger          = core_game::LoggerLocator::getService();
  m_collision_shape = get_node<CollisionShape2D>("CollisionShape2D");
}

void Gate::set_next_room(const String& scene) {
  m_next_room = scene.utf8().get_data();
}

String Gate::get_next_room() const {
  return {m_next_room.c_str()};
}

void Gate::_on_body_entered(Node2D* node) {
  if (is_closed()) return;

  if (node->is_in_group("player")) {
    m_logger->info("On body entered");
    CharString gate_name_buf = get_name().get_basename().utf8();
    auto gate_name           = gate_name_buf.get_data();
    World::go_to_scene({m_next_room, gate_name});
    queue_free();
  }
}

void Gate::_on_body_exited(godot::Node2D* node) {
  if (node->is_in_group("player")) {
    set_closed(false);
  }
}

Vector2 Gate::get_collision_shape_position() const {
  return m_collision_shape->get_position();
}

bool Gate::is_closed() const {
  return m_closed;
}

void Gate::set_closed(bool closed) {
  m_closed = closed;
}
