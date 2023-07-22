#include "health_bar.hpp"

void HealthBar::_bind_methods() {
  BIND_PROPERTY(HealthBar, autohide_timeout, godot::Variant::FLOAT);
}

void HealthBar::_ready() {
  m_timer.set_timeout(m_autohide_timeout);
  m_timer.set_callback([&]() { hide(); });
}

void HealthBar::_process(double delta) {
  if (is_visible()) {
    m_timer.tick(delta);
  }
}

void HealthBar::set_value(double value) {
  TextureProgressBar::set_value(value);
  if (!is_visible()) {
    show();
    m_timer.start();
  }
}

void HealthBar::set_autohide_timeout(TimeDelta timeout) {
  m_autohide_timeout = timeout;
}

TimeDelta HealthBar::get_autohide_timeout() const {
  return m_autohide_timeout;
}
