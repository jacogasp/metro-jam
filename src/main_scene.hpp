#ifndef COREGAME_MAIN_SCENE_HPP
#define COREGAME_MAIN_SCENE_HPP

#include "logger.hpp"
#include "macros.hpp"
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;
class HUD;
class Player;
class World;

class MainScene : public Node2D {
  GDCLASS_V2(MainScene, Node2D)

  core_game::Logger m_logger;
  HUD* m_hud       = nullptr;
  Player* m_player = nullptr;
  World* m_world   = nullptr;

  static void _bind_methods();
  MainScene();
  ~MainScene() override;

  void _ready() override;
  void on_player_hit() const;
  void on_player_gains_life() const;
  void on_player_got_powerup(Node*) const;
  void save() const;
};

#endif // COREGAME_MAIN_SCENE_HPP
