#ifndef COREGAME_GATE_HPP
#define COREGAME_GATE_HPP

#include "macros.hpp"
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

namespace core_game {
class LoggerService;
}
using namespace godot;
class Gate : public Area2D {
 public:
  enum Direction { left, right };

 private:
  std::string m_next_room;
  core_game::LoggerService* m_logger{nullptr};
  CollisionShape2D* m_collision_shape{nullptr};
  bool m_closed{false};
  int m_exit_direction{1};  // Right

  GDCLASS_V2(Gate, Area2D)
  static void _bind_methods();
  void _ready() override;
  void _on_body_entered(Node2D* node);
  void _on_body_exited(Node2D* node);

 public:
  void set_next_room(const String& scene);
  [[nodiscard]] String get_next_room() const;
  [[nodiscard]] bool is_closed() const;
  void set_closed(bool closed);
  [[nodiscard]] Direction get_exit_direction() const;
  [[nodiscard]] int get_direction() const;
  void set_direction(int direction);
};

#endif // COREGAME_GATE_HPP
