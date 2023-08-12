#ifndef COREGAME_HUD_HPP
#define COREGAME_HUD_HPP

#include "lifebar.hpp"
#include "macros.hpp"
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <vector>

using namespace godot;

using Powerups = std::vector<Node2D*>;

class HUD : public CanvasLayer {
  GDCLASS_V2(HUD, CanvasLayer)
 private:
  LifeBar* m_lifebar{nullptr};
  Powerups m_powerups;
  static void _bind_methods();

 public:
  void _ready() override;
  [[nodiscard]] LifeBar* get_lifebar() const;
  void on_player_got_powerup(Node2D* power_up);
  void start_game();
  void show_start();
  void hide_start();
  void show_gameover();
  void hide_gameover();
  void show_in_game();
  void hide_in_game();
};

#endif // COREGAME_HUD_HPP
