#ifndef COREGAME_HUD_HPP
#define COREGAME_HUD_HPP

#include "lifebar.hpp"
#include "macros.hpp"
#include <godot_cpp/classes/canvas_layer.hpp>

using namespace godot;

class HUD : public CanvasLayer {
  GDCLASS_V2(HUD, CanvasLayer)
  LifeBar* m_lifebar{nullptr};
  static void _bind_methods();
  void _ready() override;

 public:
  [[nodiscard]] LifeBar* get_lifebar() const;
};

#endif // COREGAME_HUD_HPP
