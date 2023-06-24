#include "player_commands.hpp"
#include "player.hpp"

void NullCommand::execute(Player&) {
  // do nothing
}

void IdleCommand::execute(Player& player) {
  player.set_state(&Player::standing);
  player.m_animatedSprite2D->play("Idle");
}

void JumpCommand::execute(Player& player) {
  auto velocity = player.get_velocity();
  player.set_state(&Player::jumping);
  player.m_animatedSprite2D->play("JumpIn");
  velocity.y = -player.get_jump_force();
  player.set_velocity(velocity);
}

void JumpOutCommand::execute(Player& player) {
  player.m_animatedSprite2D->play("JumpOut");
}

void RunCommand::execute(Player& player) {
  player.m_animatedSprite2D->play("Run");
  player.set_state(&Player::running);
}

void AttackCommand::execute(Player& player) {
  player.set_state(&Player::attacking);
  player.m_animatedSprite2D->play("Attack");
  player.set_weapon_monitoring(true);
}
