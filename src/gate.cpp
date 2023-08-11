#include "gate.hpp"
#include "logger.hpp"
#include "profiler.hpp"
#include "world.hpp"

void Gate::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_body_entered"), &Gate::_on_body_entered);
  ClassDB::bind_method(D_METHOD("_on_body_exited"), &Gate::_on_body_exited);
  ClassDB::bind_method(D_METHOD("set_next_room"), &Gate::set_next_room);
  ClassDB::bind_method(D_METHOD("get_next_room"), &Gate::get_next_room);
  ClassDB::bind_method(D_METHOD("get_direction"), &Gate::get_direction);
  ClassDB::bind_method(D_METHOD("set_direction"), &Gate::set_direction);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "next_room"), "set_next_room",
               "get_next_room");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "exit_direction",
                            PropertyHint::PROPERTY_HINT_ENUM, "Left, Right"),
               "set_direction", "get_direction");
  ADD_SIGNAL(MethodInfo("go_to_next_room",
                        PropertyInfo(Variant::OBJECT, "next_room")));
}

void Gate::_ready() {
  m_logger          = core_game::LoggerLocator::getService();
  m_collision_shape = get_node<CollisionShape2D>("CollisionShape2D");
}

void Gate::_on_body_entered(Node2D* node) {
  PROFILE_FUNCTION();
  if (is_closed())
    return;

  if (node->is_in_group("Player")) {
    CharString gate_name_buf = get_name().get_basename().utf8();
    auto gate_name           = std::string{gate_name_buf.get_data()};
    m_logger->info("Player entered gate " + gate_name);
    World::go_to_scene({m_next_room, gate_name});
    queue_free();
  }
}

void Gate::_on_body_exited(godot::Node2D* node) {
  if (node->is_in_group("Player")) {
    set_closed(false);
  }
}

// Getters and setters

void Gate::set_next_room(const String& scene) {
  m_next_room = scene.utf8().get_data();
}

String Gate::get_next_room() const {
  return {m_next_room.c_str()};
}

bool Gate::is_closed() const {
  return m_closed;
}

void Gate::set_closed(bool closed) {
  m_closed = closed;
}

Gate::Direction Gate::get_exit_direction() const {
  return static_cast<Direction>(m_exit_direction);
}

int Gate::get_direction() const {
  return m_exit_direction;
}

void Gate::set_direction(int direction) {
  m_exit_direction = direction;
}
