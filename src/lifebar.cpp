#include "lifebar.hpp"
#include <cassert>

void LifeBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_max_lives"), &LifeBar::get_max_lives);
  ClassDB::bind_method(D_METHOD("set_max_lives"), &LifeBar::set_max_lives);
  ClassDB::bind_method(D_METHOD("get_current_life"),
                       &LifeBar::get_current_life);
  ClassDB::bind_method(D_METHOD("set_current_life"),
                       &LifeBar::set_current_life);
  ClassDB::bind_method(D_METHOD("get_offset"), &LifeBar::get_offset);
  ClassDB::bind_method(D_METHOD("set_offset"), &LifeBar::set_offset);

  ADD_PROPERTY(PropertyInfo(Variant::INT, "max_lives"), "set_max_lives",
               "get_max_lives");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "current_life"), "set_current_life",
               "get_current_life");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "offset"), "set_offset",
               "get_offset");
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
  m_life_empty = get_node<Sprite2D>("LifeEmpty");
  m_life_full  = get_node<Sprite2D>("LifeFull");
  if (m_life_full == nullptr || m_life_empty == nullptr)
    return;
  m_life_empty_width = m_life_empty->get_rect().size.x;
  m_life_full_width  = m_life_full->get_rect().size.x;
  init();
}

void LifeBar::init() {
  if (m_life_full == nullptr)
    return;

  float current_x = 0;
  assert(m_current_life > 0);

  for (auto i = 0; i < m_current_life; ++i) {
    Vector2 position = Vector2{current_x, 0};
    current_x += m_life_full_width + m_offset;
    m_sprites.emplace_back(m_life_full->duplicate());
    m_sprites.back()->set("position", position);
    add_child(m_sprites.back());
  }

  if (m_life_empty == nullptr)
    return;

  for (int i = m_current_life; i < m_max_lives; ++i) {
    Vector2 position = Vector2{current_x, 0};
    current_x += m_life_empty_width + m_offset;
    auto instance = m_life_empty->duplicate();
    instance->set("position", position);
    add_child(instance);
  }
}

void LifeBar::add_life() {
  if (m_life_empty == nullptr)
    return;

  auto const index    = static_cast<size_t>(m_current_life);
  auto const position = m_sprites.at(index)->get("position");
  m_sprites.at(index)->queue_free();
  m_sprites.at(index) = m_life_full->duplicate();
  m_sprites.at(index)->set("position", position);
  add_child(m_sprites.at(index));
  if (m_current_life < m_max_lives) {
    m_current_life++;
  }
}

void LifeBar::lose_life() {
  --m_current_life;
  if (m_current_life < 0) {
    m_current_life = 0;
    return;
  }
  if (m_life_empty == nullptr)
    return;

  auto const index    = static_cast<size_t>(m_current_life);
  auto const position = m_sprites.at(index)->get("position");
  m_sprites.at(index)->queue_free();
  m_sprites.at(index) = m_life_empty->duplicate();
  m_sprites.at(index)->set("position", position);
  add_child(m_sprites.at(index));
}
