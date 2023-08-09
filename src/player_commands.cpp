#include "player_commands.hpp"
#include "player.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>

void IdleCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  player.set_state(&Player::standing);
  player.m_animatedSprite2D->play("Idle");
}

void JumpCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  auto velocity = player.get_velocity();
  player.set_state(&Player::jumping);
  player.m_animatedSprite2D->play("JumpIn");
  player.m_audio_jump->play();
  velocity.y = -player.get_jump_force();
  player.set_velocity(velocity);
}

void FallCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  player.m_animatedSprite2D->play("JumpOut");
  player.set_state(&Player::falling);
}

void AirJumpCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  auto velocity = player.get_velocity();
  player.set_state(&Player::air_jumping);
  player.m_animatedSprite2D->play("JumpIn");
  player.m_audio_jump->play();
  velocity.y = -player.get_air_jump_force();
  player.set_velocity(velocity);
}

void RunCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  player.m_animatedSprite2D->play("Run");
  player.m_audio_footsteps->play();
  player.set_state(&Player::running);
}

void AttackCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  static const char* animations[] = {"AttackH", "AttackV"};
  static auto generator           = godot::RandomNumberGenerator();
  int animation_index             = generator.randi_range(0, 1);
  auto animation                  = animations[animation_index];
  player.set_state(&Player::attacking);
  player.m_animatedSprite2D->play(animation);
  auto target = player.get_global_position();
  target.x += player.m_direction == Player::right ? player.m_attack_range
                                                  : -player.m_attack_range;
  player.m_wrench_weapon->fire(target);
}

void GrenadeCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  player.set_state(&Player::attacking);
  auto grenade_launcher = player.get_node<Gun>("GrenadeLauncher");
  if (grenade_launcher && !grenade_launcher->cooling_down()) {
    Vector2 target = player.get_direction() == Player::right ? Vector2{1, -1}
                                                             : Vector2{-1, -1};
    grenade_launcher->fire(target);
    player.m_animatedSprite2D->play("AttackGrenade");
  }
}

void SlideCommand::operator()(Player& player) const {
  PROFILE_FUNCTION();
  if (!player.has_node("Superpowers/Slide")) {
    return;
  }

  auto slide = player.get_node<SlidePower>("Superpowers/Slide");
  if (slide) {
    slide->activate();
  }
  auto immunity = player.get_node<Immunity>("Immunity");
  if (immunity) {
    immunity->activate();
  }
}
