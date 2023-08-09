#include "superpowers.hpp"
#include "player.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/label.hpp>

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

// Slide Power
static void set_icon_cooling_level(TextureRect& texture_rect, float level) {
  Ref<ShaderMaterial> material = texture_rect.get_material();
  material->set_shader_parameter("amount", level);
}

void SlidePower::_bind_methods() {
  ClassDB::bind_method(D_METHOD("pick_me"), &SlidePower::pick_me);
}

void SlidePower::_ready() {
  m_texture_rect = get_node<TextureRect>("TextureRect");
  m_cooldown_timer.set_timeout(5);
  m_cooldown_timer.set_callback([&] { m_enabled = true; });
}

void SlidePower::_process(double delta) {
  m_cooldown_timer.tick(delta);
  if (cooling_down() && m_texture_rect) {
    auto const level = 1
                     - static_cast<float>(m_cooldown_timer.remaining()
                                          / m_cooldown_timer.get_timeout());
    set_icon_cooling_level(*m_texture_rect, level);
  }
}

void SlidePower::activate() {
  if (!m_enabled) {
    return;
  }

  // This is pretty bad
  auto player = cast_to<Player>(get_parent()->get_parent());
  if (player == nullptr) {
    std::cerr << "Parent null\n";
    return;
  }

  auto const as = player->get_node<AnimatedSprite2D>("AnimatedSprite2D");
  if (as) {
    as->play("Slide");
  }
  player->set_state(&Player::sliding);
  player->m_audio_footsteps->stop();
  auto immunity = player->get_node<Immunity>("Immunity");
  if (immunity) {
    immunity->activate();
  }

  set_icon_cooling_level(*m_texture_rect, 1);
  m_enabled = false;
  m_cooldown_timer.start();
}

void SlidePower::pick_me(Node2D* picker) {
  auto label = get_node<Label>("Label");
  if (label) {
    label->queue_free();
  }
  auto area = get_node<Area2D>("Area2D");
  if (area) {
    area->queue_free();
  }
  picker->call_deferred("pick", this);
}

bool SlidePower::cooling_down() const {
  return m_cooldown_timer.is_running();
}

void AirJumpPower::_bind_methods() {
  ClassDB::bind_method(D_METHOD("pick_me"), &AirJumpPower::pick_me);
}

void AirJumpPower::activate() {
}

void AirJumpPower::pick_me(Node2D* picker) {
  auto label = get_node<Label>("Label");
  if (label) {
    label->queue_free();
  }
  auto area = get_node<Area2D>("Area2D");
  if (area) {
    area->queue_free();
  }
  picker->call_deferred("pick", this);
}
