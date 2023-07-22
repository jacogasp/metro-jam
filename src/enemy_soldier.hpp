#ifndef COREGAME_ENEMY_SOLDIER_HPP
#define COREGAME_ENEMY_SOLDIER_HPP

#include <godot_cpp/classes/character_body2d.hpp>

#include "damageable.hpp"
#include "macros.hpp"

using namespace godot;

class EnemySoldier
    : public CharacterBody2D
    , public Damageable {

  GDCLASS_V2(EnemySoldier, CharacterBody2D)
 private:
  static void _bind_methods();

 public:
  void _process(double) override;
  void take_hit(int) override;
};

#endif // COREGAME_ENEMY_SOLDIER_HPP
