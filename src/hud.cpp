#include "hud.hpp"

void HUD::_bind_methods() {
}

void HUD::_ready() {
  m_lifebar = get_node<LifeBar>("LifeBar");
}

LifeBar* HUD::get_lifebar() const {
  return m_lifebar;
}
