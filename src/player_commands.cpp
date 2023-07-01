#include "player_commands.hpp"
#include "player.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>

void execute(PlayerCommand& command, Player& player) {
  std::visit([&player](auto cmd) { cmd(player); }, command);
}

void NullCommand::operator()(Player&) const {
  // do nothing
}

void IdleCommand::operator()(Player& player) {
  player.set_state(&Player::standing);
  player.m_animatedSprite2D->play("Idle");
}

void JumpCommand::operator()(Player& player) {
  auto velocity = player.get_velocity();
  player.set_state(&Player::jumping);
  player.m_animatedSprite2D->play("JumpIn");
  velocity.y = -player.get_jump_force();
  player.set_velocity(velocity);
}

void JumpOutCommand::operator()(Player& player) {
  player.m_animatedSprite2D->play("JumpOut");
}

void AirJumpCommand::operator()(Player& player) {
  auto velocity = player.get_velocity();
  player.set_state(&Player::air_jumping);
  player.m_animatedSprite2D->play("JumpIn");
  velocity.y = -player.get_air_jump_force();
  player.set_velocity(velocity);
}

void RunCommand::operator()(Player& player) {
  player.m_animatedSprite2D->play("Run");
  player.set_state(&Player::running);
}

void AttackCommand::operator()(Player& player) {
  static const char* animations[] = {"AttackH", "AttackV"};
  static auto generator           = godot::RandomNumberGenerator();
  int animation_index             = generator.randi_range(0, 1);
  auto animation                  = animations[animation_index];
  player.set_state(&Player::attacking);
  player.m_animatedSprite2D->play(animation);
  player.set_weapon_monitoring(true);
}

void SlideCommand::operator()(Player& player) {
  player.m_animatedSprite2D->play("Slide");
  player.set_state(&Player::sliding);
}
