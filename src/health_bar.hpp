#ifndef COREGAME_HEALTH_BAR_HPP
#define COREGAME_HEALTH_BAR_HPP

#include "macros.hpp"
#include "timer.hpp"
#include <godot_cpp/classes/texture_progress_bar.hpp>

using namespace godot;

class HealthBar : public TextureProgressBar {
  GDCLASS_V2(HealthBar, TextureProgressBar)

 private:
  Timer m_timer;
  TimeDelta m_autohide_timeout = 2;

  static void _bind_methods();

 public:
  void _ready() override;
  void _process(double delta) override;
  void set_value(double value);
  void set_autohide_timeout(TimeDelta timeout);
  [[nodiscard]] TimeDelta get_autohide_timeout() const;
};

#endif // COREGAME_HEALTH_BAR_HPP
