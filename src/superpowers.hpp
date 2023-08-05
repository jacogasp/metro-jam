#ifndef COREGAME_SUPERPOWERS_HPP
#define COREGAME_SUPERPOWERS_HPP

#include "macros.hpp"
#include "timer.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <map>
#include <memory>
#include <unordered_map>

class Superpower;
using namespace godot;

class Superpower {
 public:
  virtual ~Superpower() = default;

 protected:
  virtual void activate() = 0;
};

class Immunity
    : public Node2D
    , public Superpower {
  GDCLASS_V2(Immunity, Node2D)

 private:
  Timer m_timer;
  Timer m_cooldown_timer;

 protected:
  static void _bind_methods();

 public:
  void _ready() override;
  void _process(double delta) override;
  void activate() override;
  bool is_active() const;
  void set_duration(TimeDelta duration);
  [[nodiscard]] TimeDelta get_duration() const;
  void set_cooldown(TimeDelta cooldown);
  [[nodiscard]] TimeDelta get_cooldown() const;
  [[nodiscard]] bool cooling_down() const;

 private:
  void deactivate();
};

class SlidePower
    : public Node2D
    , public Superpower {
  GDCLASS_V2(SlidePower, Node2D)

 private:
  static void _bind_methods();

 public:
  void activate() override;
  void pick_me(Node2D* picker);
};

#endif // COREGAME_SUPERPOWERS_HPP
