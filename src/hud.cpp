#include "hud.hpp"
#include <godot_cpp/classes/box_container.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/texture_rect.hpp>

void HUD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &HUD::on_player_got_powerup);
  ClassDB::bind_method(D_METHOD("start_game"), &HUD::start_game);
  ADD_SIGNAL(MethodInfo("start_game"));
}

void HUD::_ready() {
  m_lifebar         = get_node<LifeBar>("InGame/LifeBar");
  auto start_button = get_node<Button>("Start/StartButton");
}

LifeBar* HUD::get_lifebar() const {
  return m_lifebar;
}

void HUD::on_player_got_powerup(Node2D* power_up) {
  m_powerups.push_back(power_up);
  auto container = get_node<BoxContainer>("InGame/PowerUpsContainer");
  if (power_up->has_node("TextureRect") && container) {
    auto texture_rect = power_up->get_node<TextureRect>("TextureRect");
    auto old_parent   = texture_rect->get_parent();
    old_parent->remove_child(texture_rect);
    container->add_child(texture_rect);
  }
}

void HUD::start_game() {
  std::cerr << "Start game\n";
  emit_signal("start_game");
}

void HUD::show_start() {
  auto cl = get_node<CanvasLayer>("Start");
  if (cl) {
    cl->show();
  }
}

void HUD::hide_start() {
  auto cl = get_node<CanvasLayer>("Start");
  if (cl) {
    cl->hide();
  }
}
void HUD::show_gameover() {
  auto cl = get_node<CanvasLayer>("GameOver");
  if (cl) {
    cl->show();
  }
}
void HUD::hide_gameover() {
  auto cl = get_node<CanvasLayer>("GameOver");
  if (cl) {
    cl->hide();
  }
}

void HUD::show_in_game() {
  auto cl = get_node<CanvasLayer>("InGame");
  if (cl) {
    cl->show();
  }
}

void HUD::hide_in_game() {
  auto cl = get_node<CanvasLayer>("InGame");
  if (cl) {
    cl->hide();
  }
}
