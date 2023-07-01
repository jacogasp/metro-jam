#include "player_commands.hpp"
#include "player.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>

void execute(PlayerCommand& command, Player& player) {
  std::visit([&player](auto cmd) { cmd.execute(player); }, command);
}

void NullCommand::execute(Player&) const {
  // do nothing
}

void IdleCommand::execute(Player& game_actor) {
  game_actor.set_state(&Player::standing);
  game_actor.m_animatedSprite2D->play("Idle");
}

void JumpCommand::execute(Player& game_actor) {
  auto velocity = game_actor.get_velocity();
  game_actor.set_state(&Player::jumping);
  game_actor.m_animatedSprite2D->play("JumpIn");
  velocity.y = -game_actor.get_jump_force();
  game_actor.set_velocity(velocity);
}

void JumpOutCommand::execute(Player& game_actor) {
  game_actor.m_animatedSprite2D->play("JumpOut");
}

void AirJumpCommand::execute(Player& game_actor) {
  auto velocity = game_actor.get_velocity();
  game_actor.set_state(&Player::air_jumping);
  game_actor.m_animatedSprite2D->play("JumpIn");
  velocity.y = -game_actor.get_air_jump_force();
  game_actor.set_velocity(velocity);
}

void RunCommand::execute(Player& game_actor) {
  game_actor.m_animatedSprite2D->play("Run");
  game_actor.set_state(&Player::running);
}

void AttackCommand::execute(Player& game_actor) {
  static const char* animations[] = {"AttackH", "AttackV"};
  auto generator                  = godot::RandomNumberGenerator();
  int animation_index             = generator.randi_range(0, 1);
  auto animation                  = animations[animation_index];
  game_actor.set_state(&Player::attacking);
  game_actor.m_animatedSprite2D->play(animation);
  game_actor.set_weapon_monitoring(true);
}

void SlideCommand::execute(Player& game_actor) {
  game_actor.m_animatedSprite2D->play("Slide");
  game_actor.set_state(&Player::sliding);
}
