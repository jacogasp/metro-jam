#include "superpowers.hpp"

void Immunity::_bind_methods() {
  BIND_PROPERTY(Immunity, duration, Variant::FLOAT);
  BIND_PROPERTY(Immunity, cooldown, Variant::FLOAT);
}

void Immunity::_ready() {
  m_timer.set_callback([this] { deactivate(); });
  m_timer.set_timeout(1);
  m_cooldown_timer.set_callback([] {});
  m_cooldown_timer.set_timeout(5);
}

void Immunity::_process(double delta) {
  m_timer.tick(delta);
  m_cooldown_timer.tick(delta);
}

void Immunity::activate() {
  std::cerr << "Immunity activated\n";
  m_timer.start();
}

bool Immunity::is_active() const {
  return m_timer.is_running();
}

void Immunity::set_duration(TimeDelta duration) {
  m_timer.set_timeout(duration);
}

TimeDelta Immunity::get_duration() const {
  return m_timer.get_timeout();
}

void Immunity::set_cooldown(TimeDelta cooldown) {
  m_cooldown_timer.set_timeout(cooldown);
}

TimeDelta Immunity::get_cooldown() const {
  return m_cooldown_timer.get_timeout();
}

bool Immunity::cooling_down() const {
  return m_timer.remaining() > 0;
}

void Immunity::deactivate() {
  std::cerr << "Immunity deactivated\n";
  m_cooldown_timer.start();
}
