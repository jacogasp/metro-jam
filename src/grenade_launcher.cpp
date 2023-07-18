#include "grenade_launcher.hpp"
#include "grenade.hpp"

void GrenadeLauncher::_bind_methods() {
  BIND_PROPERTY(GrenadeLauncher, grenade, Variant::OBJECT);
  BIND_PROPERTY(GrenadeLauncher, impulse, Variant::VECTOR2);
}

void GrenadeLauncher::fire(Vector2 const&) {
  auto node    = m_grenade->instantiate();
  auto grenade = cast_to<Grenade>(node);
  if (grenade) {
    get_parent()->get_parent()->add_child(grenade);
    grenade->apply_impulse(m_impulse + m_velocity);
    grenade->set_global_position(get_global_position());
  } else {
    std::cerr << "Grenade Packed Scene must be of type Grenade\n";
  }
}

void GrenadeLauncher::set_grenade(const Ref<PackedScene>& scene) {
  m_grenade = scene;
}

Ref<PackedScene> GrenadeLauncher::get_grenade() const {
  return m_grenade;
}

void GrenadeLauncher::set_impulse(const Vector2& impulse) {
  m_impulse = impulse;
}

Vector2 GrenadeLauncher::get_impulse() const {
  return m_impulse;
}

void GrenadeLauncher::_process(double t) {
  auto const position = get_global_position();
  m_velocity          = (position - m_prev_position) / static_cast<float>(t);
  m_prev_position     = position;
}