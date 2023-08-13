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
  ClassDB::bind_method(D_METHOD("resume_game"), &HUD::resume_game);
  ClassDB::bind_method(D_METHOD("continue_game"), &HUD::continue_game);
  ClassDB::bind_method(D_METHOD("restart_game"), &HUD::restart_game);
  ClassDB::bind_method(D_METHOD("quit"), &HUD::quit);
  ADD_SIGNAL(MethodInfo("start_game"));
  ADD_SIGNAL(MethodInfo("resume_game"));
  ADD_SIGNAL(MethodInfo("continue_game"));
  ADD_SIGNAL(MethodInfo("restart_game"));
  ADD_SIGNAL(MethodInfo("quit"));
}

void HUD::go_to(HUD::State state) {
  auto children = get_children();
  for (auto i = 0; i < children.size(); ++i) {
    auto child = cast_to<CanvasLayer>(children[i]);
    if (child) {
      child->hide();
    }
  }
  m_active_buttons.clear();
  m_active_button_idx = 0;
  switch (state) {
  case start: {
    m_state = start;
    auto cl = get_node<CanvasLayer>("Start");
    if (cl) {
      cl->show();
      auto start_button = cl->get_node<Button>("StartButton");
      auto quit_button  = cl->get_node<Button>("QuitButton");
      m_active_buttons  = Buttons{start_button, quit_button};
      m_active_buttons[m_active_button_idx]->grab_focus();
    }
  } break;
  case in_game: {
    m_state = in_game;
    auto cl = get_node<CanvasLayer>("InGame");
    if (cl) {
      cl->show();
    }
  } break;
  case pause: {
    m_state = pause;
    auto cl = get_node<CanvasLayer>("Pause");
    if (cl) {
      cl->show();
      auto resume_button  = cl->get_node<Button>("ResumeButton");
      auto restart_button = cl->get_node<Button>("RestartButton");
      auto quit_button    = cl->get_node<Button>("QuitButton");
      m_active_buttons    = Buttons{resume_button, restart_button, quit_button};
      m_active_buttons[m_active_button_idx]->grab_focus();
    }
  } break;
  case game_over: {
    m_state = game_over;
    auto cl = get_node<CanvasLayer>("GameOver");
    if (cl) {
      cl->show();
      auto continue_button = cl->get_node<Button>("ContinueButton");
      auto restart_button  = cl->get_node<Button>("RestartButton");
      auto quit_button     = cl->get_node<Button>("QuitButton");
      m_active_buttons = Buttons{continue_button, restart_button, quit_button};
      m_active_buttons[m_active_button_idx]->grab_focus();
    }
  } break;
  }
}

void HUD::_ready() {
  m_lifebar = get_node<LifeBar>("InGame/LifeBar");
}

void change_focus(Buttons const& buttons, size_t old_index, size_t new_index) {
  auto old_button    = buttons.at(old_index);
  auto active_button = buttons.at(new_index);
  old_button->release_focus();
  active_button->grab_focus();
}

void HUD::_input(const Ref<InputEvent>& inputEvent) {
  if (m_state == in_game) {
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
    change_focus(m_active_buttons, old_index, m_active_button_idx);
  } else if (inputEvent->is_action_released("ui_up")) {
    if (m_active_button_idx == 0) {
      m_active_button_idx = m_active_buttons.size() - 1;
    } else {
      m_active_button_idx--;
    }
    change_focus(m_active_buttons, old_index, m_active_button_idx);
  } else if (inputEvent->is_action_released("ui_accept")) {
    auto active_button = m_active_buttons.at(m_active_button_idx);
    active_button->_pressed();
  }
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

void HUD::resume_game() {
  emit_signal("resume_game");
}

void HUD::quit() {
  emit_signal("quit");
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
