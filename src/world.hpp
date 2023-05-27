#ifndef COREGAME_WORLD_HPP
#define COREGAME_WORLD_HPP

#include "gate.hpp"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <array>
#include <string>

using namespace godot;
namespace core_game {
class LoggerService;
}

class Player;
class World : public Node2D {
  GDCLASS(World, Node2D);
  struct NextSceneMessage {
    std::string next_scene;
    std::string gate_name;
  };

  core_game::LoggerService* m_logger{nullptr};
  Node* m_current_scene{nullptr};
  Node* m_player{nullptr};
  NodePath m_player_node_path{};
  Ref<PackedScene> m_packed_scene{nullptr};

  static std::array<NextSceneMessage, 16> m_pending;
  static int m_pending_index;

  static void _bind_methods();
  void _ready() override;
  void _process(float);
  void update_scene();
  // Use set/get only for editor only
  Ref<PackedScene> get_packed_scene();
  void set_packed_scene(const Ref<PackedScene>& packed_scene);
  void set_player(const NodePath &path);
  [[nodiscard]] NodePath get_player() const;

 public:
  // deferred method
  static void go_to_scene(const NextSceneMessage& next_sceen_message);
};

#endif // COREGAME_WORLD_HPP
