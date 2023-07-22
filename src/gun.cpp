#include "gun.hpp"
#include <godot_cpp/classes/rigid_body2d.hpp>

void Gun::_bind_methods() {
  BIND_PROPERTY(Gun, bullet, Variant::OBJECT);
}

void Gun::set_bullet(const Ref<PackedScene>& scene) {
  m_bullet = scene;
}

Ref<PackedScene> Gun::get_bullet() const {
  return m_bullet;
}
void Gun::fire(const Vector2& target) {
  auto node    = m_bullet->instantiate();
  auto grenade = cast_to<RigidBody2D>(node);
  if (grenade) {
    get_parent()->get_parent()->add_child(grenade);
    grenade->apply_impulse({300, -300});
    grenade->set_global_position(get_global_position());
  } else {
    std::cerr << "Grenade Packed Scene must be of type Grenade\n";
  }
}
