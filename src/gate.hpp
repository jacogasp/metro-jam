#ifndef COREGAME_GATE_HPP
#define COREGAME_GATE_HPP

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

namespace core_game {
class LoggerService;
}
using namespace godot;
class Gate : public Area2D {
  GDCLASS(Gate, Area2D);
  std::string m_next_room;
  core_game::LoggerService* m_logger{nullptr};
  CollisionShape2D *m_collision_shape{nullptr};
  bool m_closed{false};

  static void _bind_methods();
  void _ready() override;
  void _on_body_entered(Node2D* node);
  void _on_body_exited(Node2D* node);

 public:
  void set_next_room(const String& scene);
  [[nodiscard]] String get_next_room() const;
  Vector2 get_collision_shape_position() const;
  [[nodiscard]] bool is_closed() const;
  void set_closed(bool closed);
};

#endif // COREGAME_GATE_HPP
