#include "hud.hpp"
#include <godot_cpp/classes/box_container.hpp>
#include <godot_cpp/classes/texture_rect.hpp>

void HUD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &HUD::on_player_got_powerup);
}

void HUD::_ready() {
  m_lifebar = get_node<LifeBar>("LifeBar");
}

LifeBar* HUD::get_lifebar() const {
  return m_lifebar;
}

void HUD::on_player_got_powerup(Node2D* power_up) {
  m_powerups.push_back(power_up);
  auto container = get_node<BoxContainer>("PowerUpsContainer");
  if (power_up->has_node("TextureRect") && container) {
    auto texture_rect = power_up->get_node<TextureRect>("TextureRect");
    auto old_parent   = texture_rect->get_parent();
    old_parent->remove_child(texture_rect);
    container->add_child(texture_rect);
  }
}
