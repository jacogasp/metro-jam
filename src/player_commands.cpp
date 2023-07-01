#include "player_commands.hpp"
#include "player.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>

template<>
void NullCommand::execute(Player&) const {
  // do nothing
}

template<>
void IdleCommand::execute(Player& game_actor) const {
  game_actor.set_state(&Player::standing);
  game_actor.m_animatedSprite2D->play("Idle");
}

template<>
void JumpCommand::execute(Player& game_actor) const {
  auto velocity = game_actor.get_velocity();
  game_actor.set_state(&Player::jumping);
  game_actor.m_animatedSprite2D->play("JumpIn");
  velocity.y = -game_actor.get_jump_force();
  game_actor.set_velocity(velocity);
}

template<>
void JumpOutCommand::execute(Player& game_actor) const {
  game_actor.m_animatedSprite2D->play("JumpOut");
}

template<>
void AirJumpCommand::execute(Player& game_actor) const {
  auto velocity = game_actor.get_velocity();
  game_actor.set_state(&Player::air_jumping);
  game_actor.m_animatedSprite2D->play("JumpIn");
  velocity.y = -game_actor.get_air_jump_force();
  game_actor.set_velocity(velocity);
}

template<>
void RunCommand::execute(Player& game_actor) const {
  game_actor.m_animatedSprite2D->play("Run");
  game_actor.set_state(&Player::running);
}

template<>
void AttackCommand::execute(Player& game_actor) const {
  static const char* animations[] = {"AttackH", "AttackV"};
  auto generator                  = godot::RandomNumberGenerator();
  int animation_index             = generator.randi_range(0, 1);
  auto animation                  = animations[animation_index];
  game_actor.set_state(&Player::attacking);
  game_actor.m_animatedSprite2D->play(animation);
  game_actor.set_weapon_monitoring(true);
}
