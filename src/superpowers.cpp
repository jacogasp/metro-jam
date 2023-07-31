#include "superpowers.hpp"

void Immunity::activate() {
  m_is_active = true;
}

bool Immunity::is_active() const {
  return m_is_active;
}
void Immunity::deactivate() {
  m_is_active = false;
}
