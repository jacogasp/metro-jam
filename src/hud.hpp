#ifndef COREGAME_HUD_HPP
#define COREGAME_HUD_HPP

#include <godot_cpp/classes/canvas_layer.hpp>
#include "lifebar.hpp"

using namespace godot;

class HUD : public CanvasLayer {
  GDCLASS(HUD, CanvasLayer);
  LifeBar* m_lifebar{nullptr};
  static void _bind_methods();
  void _ready() override;
 public:
  LifeBar* get_lifebar() const;
};

#endif // COREGAME_HUD_HPP
