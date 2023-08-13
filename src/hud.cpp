#include "hud.hpp"
#include <godot_cpp/classes/box_container.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event_mouse.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/texture_rect.hpp>

void HUD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &HUD::on_player_got_powerup);
  ClassDB::bind_method(D_METHOD("start_game"), &HUD::start_game);
  ClassDB::bind_method(D_METHOD("continue_game"), &HUD::continue_game);
  ClassDB::bind_method(D_METHOD("restart_game"), &HUD::restart_game);
  ClassDB::bind_method(D_METHOD("quit"), &HUD::quit);
  ADD_SIGNAL(MethodInfo("start_game"));
  ADD_SIGNAL(MethodInfo("continue_game"));
  ADD_SIGNAL(MethodInfo("restart_game"));
  ADD_SIGNAL(MethodInfo("quit"));
}

void HUD::_ready() {
  m_lifebar = get_node<LifeBar>("InGame/LifeBar");
}

void HUD::_input(const Ref<InputEvent>& inputEvent) {
  if (m_in_game) {
    return;
  }
  auto old_index = m_active_button_idx;
  auto mouse     = inputEvent->cast_to<InputEventMouse>(inputEvent.ptr());
  if (mouse) {
    return;
  }

  if (inputEvent->is_action_released("ui_down")) {
    m_active_button_idx += 1;
    if (m_active_button_idx >= m_active_buttons.size()) {
      m_active_button_idx = 0;
    }
  }
  if (inputEvent->is_action_released("ui_up")) {
    if (m_active_button_idx == 0) {
      m_active_button_idx = m_active_buttons.size();
    } else {
      m_active_button_idx--;
    }
  }
  if (inputEvent->is_action_released("ui_accept")) {
    auto active_button = m_active_buttons.at(m_active_button_idx);
    active_button->_pressed();
    return;
  }
  auto old_button    = m_active_buttons.at(old_index);
  auto active_button = m_active_buttons.at(m_active_button_idx);
  old_button->release_focus();
  active_button->grab_focus();
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
    auto start_button   = cl->get_node<Button>("StartButton");
    auto quit_button    = cl->get_node<Button>("QuitButton");
    m_active_buttons    = Buttons{start_button, quit_button};
    m_active_button_idx = 0;
  }
}

void HUD::quit() {
  emit_signal("quit");
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
    auto continue_button = cl->get_node<Button>("ContinueButton");
    auto restart_button  = cl->get_node<Button>("RestartButton");
    auto quit_button     = cl->get_node<Button>("QuitButton");
    m_active_buttons    = Buttons{continue_button, restart_button, quit_button};
    m_active_button_idx = 0;
  }
}
void HUD::hide_gameover() {
  auto cl = get_node<CanvasLayer>("GameOver");
  if (cl) {
    cl->hide();
  }
}

void HUD::show_in_game() {
  m_in_game = true;
  auto cl   = get_node<CanvasLayer>("InGame");
  if (cl) {
    cl->show();
  }
  m_active_buttons.clear();
}

void HUD::hide_in_game() {
  auto cl = get_node<CanvasLayer>("InGame");
  if (cl) {
    cl->hide();
  }
  m_in_game = false;
}

void HUD::continue_game() {
  emit_signal("continue_game");
}

void HUD::restart_game() {
  emit_signal("restart_game");
}

void HUD::set_can_continue(bool can_continue) {
  auto button = get_node<Button>("GameOver/ContinueButton");
  if (button) {
    button->set_disabled(!can_continue);
  }
}
