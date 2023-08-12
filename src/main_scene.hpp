#ifndef COREGAME_MAIN_SCENE_HPP
#define COREGAME_MAIN_SCENE_HPP

#include "logger.hpp"
#include "macros.hpp"
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;
class HUD;
class Player;
class World;

class MainScene : public Node2D {
  GDCLASS_V2(MainScene, Node2D)

  core_game::Logger m_logger;
  HUD* m_hud          = nullptr;
  Player* m_player    = nullptr;
  World* m_world      = nullptr;
  bool m_using_joypad = false;
  bool m_loading      = false;

  static void _bind_methods();
  MainScene();
  ~MainScene() override;

  void _ready() override;
  void _input(const Ref<InputEvent>& event) override;
  void on_player_hit() const;
  void on_player_gains_life() const;
  void on_player_got_powerup(Node* node);
  [[nodiscard]] bool is_using_joypad();
  void quit() const;
  void save();
  void load();
  void pause() const;
  void resume() const;
  void start_game() const;

 private:
  void load_superpowers(const Array& superpowers) const;
  static void create_savings_directory();
  static void purge_savings_directory();
};

#endif // COREGAME_MAIN_SCENE_HPP
