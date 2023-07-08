#ifndef COREGAME_LIFEBAR_HPP
#define COREGAME_LIFEBAR_HPP

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <vector>

using namespace godot;

class LifeBar : public Node2D {
  GDCLASS(LifeBar, Node2D)

  Sprite2D* m_life_empty    = nullptr;
  Sprite2D* m_life_full     = nullptr;
  int m_max_lives           = 0;
  int m_current_life        = 0;
  float m_offset            = 0;
  float m_placeholder_width = 0;
  float m_life_width        = 0;
  std::vector<Node*> m_sprites{};

  static void _bind_methods();

  void set_max_lives(int max_lives);
  [[nodiscard]] int get_max_lives() const;
  void set_current_life(int current_life);
  [[nodiscard]] int get_current_life() const;
  void set_offset(float offset);
  [[nodiscard]] float get_offset() const;

  void _ready() override;
  void init();

 public:
  void add_life();
  void lose_life();
};

#endif // COREGAME_LIFEBAR_HPP
