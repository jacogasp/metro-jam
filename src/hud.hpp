#ifndef COREGAME_HUD_HPP
#define COREGAME_HUD_HPP

#include "lifebar.hpp"
#include "macros.hpp"
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <vector>

using namespace godot;

using Powerups = std::vector<Node2D*>;
using Buttons  = std::vector<Button*>;

class HUD : public CanvasLayer {
  GDCLASS_V2(HUD, CanvasLayer)

  enum State { start, in_game, pause, game_over, the_end };

 private:
  LifeBar* m_lifebar{nullptr};
  Buttons m_active_buttons;
  size_t m_active_button_idx = 0;
  State m_state              = start;
  Powerups m_powerups;

  static void _bind_methods();

  void start_game();
  void resume_game();
  void continue_game();
  void restart_game();

 public:
  void _ready() override;
  void _input(Ref<InputEvent> const& inputEvent) override;
  [[nodiscard]] LifeBar* get_lifebar() const;
  void on_player_got_powerup(Node2D* power_up);
  void go_to(State state);
  void quit();
  void set_can_continue(bool can_continue);
};

#endif // COREGAME_HUD_HPP
