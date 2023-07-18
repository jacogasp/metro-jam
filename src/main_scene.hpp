#ifndef COREGAME_MAIN_SCENE_HPP
#define COREGAME_MAIN_SCENE_HPP

#include "logger.hpp"
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;
class HUD;

class MainScene : public Node2D {
  GDCLASS(MainScene, Node2D)

  core_game::Logger m_logger;
  HUD* m_hud{nullptr};

  static void _bind_methods();
  MainScene();
  ~MainScene() override;

  void _ready() override;
  void on_player_hit() const;
};

#endif // COREGAME_MAIN_SCENE_HPP
