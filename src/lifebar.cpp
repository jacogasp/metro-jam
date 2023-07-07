#include "lifebar.hpp"

void LifeBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_placeholder_sprite"),
                       &LifeBar::get_placeholder_sprite);
  ClassDB::bind_method(D_METHOD("set_placeholder_sprite"),
                       &LifeBar::set_placeholder_sprite);
  ClassDB::bind_method(D_METHOD("get_life_sprite"), &LifeBar::get_life_sprite);
  ClassDB::bind_method(D_METHOD("set_life_sprite"), &LifeBar::set_life_sprite);
  ClassDB::bind_method(D_METHOD("get_max_lives"), &LifeBar::get_max_lives);
  ClassDB::bind_method(D_METHOD("set_max_lives"), &LifeBar::set_max_lives);
  ClassDB::bind_method(D_METHOD("get_current_life"),
                       &LifeBar::get_current_life);
  ClassDB::bind_method(D_METHOD("set_current_life"),
                       &LifeBar::set_current_life);
  ClassDB::bind_method(D_METHOD("get_offset"), &LifeBar::get_offset);
  ClassDB::bind_method(D_METHOD("set_offset"), &LifeBar::set_offset);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "placeholder_texture"),
               "set_placeholder_sprite", "get_placeholder_sprite");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "life_texture"), "set_life_sprite",
               "get_life_sprite");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "max_lives"), "set_max_lives",
               "get_max_lives");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "current_life"), "set_current_life",
               "get_current_life");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "offset"), "set_offset",
               "get_offset");
}

void LifeBar::set_placeholder_sprite(const Ref<Texture2D>& texture) {
  m_placeholder_sprite.set_texture(texture);
}

Ref<Texture2D> LifeBar::get_placeholder_sprite() const {
  return m_placeholder_sprite.get_texture();
}

void LifeBar::set_life_sprite(const Ref<Texture2D>& texture) {
  m_life_sprite.set_texture(texture);
}

Ref<Texture2D> LifeBar::get_life_sprite() const {
  return m_life_sprite.get_texture();
}

void LifeBar::set_max_lives(int max_lives) {
  m_max_lives = max_lives;
}

int LifeBar::get_max_lives() const {
  return m_max_lives;
}

void LifeBar::set_current_life(int current_life) {
  m_current_life = current_life;
}

int LifeBar::get_current_life() const {
  return m_current_life;
}

void LifeBar::set_offset(float offset) {
  m_offset = offset;
}

float LifeBar::get_offset() const {
  return m_offset;
}

void LifeBar::_ready() {
  m_placeholder_width = m_placeholder_sprite.get_rect().size.x;
  m_life_width        = m_life_sprite.get_rect().size.x;
  init();
}

void LifeBar::init() {
  float current_x = 0;
  for (size_t i = 0; i < m_current_life; ++i) {
    Vector2 position = Vector2{current_x, 0};
    current_x += m_life_width + m_offset;
    m_sprites.emplace_back(m_life_sprite.duplicate());
    m_sprites.back()->set("position", position);
    add_child(m_sprites.back());
  }

  for (size_t i = m_current_life; i < m_max_lives; ++i) {
    Vector2 position = Vector2{current_x, 0};
    current_x += m_life_width + m_offset;
    auto instance    = m_placeholder_sprite.duplicate();
    instance->set("position", position);
    add_child(instance);
  }
}

void LifeBar::add_life() {
  ++m_current_life;
  auto const position = m_sprites.at(m_current_life)->get("position");
  m_sprites.at(m_current_life)->queue_free();
  m_sprites.at(m_current_life) = m_life_sprite.duplicate();
  m_sprites.at(m_current_life)->set("position", position);
  add_child(m_sprites.at(m_current_life));
}

void LifeBar::lose_life() {
  --m_current_life;
  if (m_current_life < 0) {
    m_current_life = 0;
    return ;
  }
  auto const position = m_sprites.at(m_current_life)->get("position");
  m_sprites.at(m_current_life)->queue_free();
  m_sprites.at(m_current_life) = m_placeholder_sprite.duplicate();
  m_sprites.at(m_current_life)->set("position", position);
  add_child(m_sprites.at(m_current_life));
}
