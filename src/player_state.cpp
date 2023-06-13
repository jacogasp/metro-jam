
#include "player_state.hpp"
#include "player.hpp"
#include "profiler.hpp"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

void StandingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::jumping);
    player.m_animatedSprite2D->play("JumpIn");
    velocity.y = -player.get_jump_force();
    player.set_velocity(velocity);
  } else if (velocity.x != 0) {
    player.m_animatedSprite2D->play("Run");
    player.set_state(&Player::running);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.m_animatedSprite2D->play("Attack");
    player.set_weapon_monitoring(true);
  }
}

void StandingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (player.is_on_floor()) {
    player.m_animatedSprite2D->play("Idle");
  } else if (velocity.y > 0) {
    player.m_animatedSprite2D->play("JumpOut");
  }
}

void JumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::air_jumping);
    player.m_animatedSprite2D->stop();
    player.m_animatedSprite2D->play("JumpIn");
    velocity.y = -player.get_air_jump_force();
    player.set_velocity(velocity);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.m_animatedSprite2D->play("Attack");
    player.set_weapon_monitoring(true);
  }
  player.m_animatedSprite2D->play();
}

void JumpingState::update(Player& player) {
  PROFILE_FUNCTION()
  auto const velocity = player.get_velocity();
  if (velocity.y > 0) {
    player.m_animatedSprite2D->play("JumpOut");
  } else if (velocity.y == 0 && player.is_on_floor()) {
    if (velocity.x == 0) {
      player.m_animatedSprite2D->play("Idle");
      player.set_state(&Player::standing);
    } else {
      player.m_animatedSprite2D->play("Run");
      player.set_state(&Player::running);
    }
  }
}

void AirJumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.m_animatedSprite2D->play("Attack");
    player.set_weapon_monitoring(true);
  }
}

void AirJumpingState::update(Player& player) {
  PROFILE_FUNCTION()
  auto const velocity = player.get_velocity();
  if (velocity.y > 0) {
    player.m_animatedSprite2D->play("JumpOut");
  }

  if (player.is_on_floor()) {
    if (velocity.x == 0) {
      player.m_animatedSprite2D->play("Idle");
      player.set_state(&Player::standing);
    } else {
      player.m_animatedSprite2D->play("Run");
      player.set_state(&Player::running);
    }
  }
}

void RunningState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::jumping);
    player.m_animatedSprite2D->play("JumpIn");
    auto velocity = player.get_velocity();
    velocity.y    = -player.get_jump_force();
    player.set_velocity(velocity);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.m_animatedSprite2D->play("Attack");
    player.set_weapon_monitoring(true);
  }
}

void RunningState::update(Player& player) {
  PROFILE_FUNCTION()
  auto const velocity = player.get_velocity();
  if (player.is_on_floor()) {
    if (velocity.x != 0) {
      player.m_animatedSprite2D->play("Run");
    } else {
      player.m_animatedSprite2D->play("Idle");
      player.set_state(&Player::standing);
    }
  } else if (velocity.y > 0) {
    player.m_animatedSprite2D->play("JumpOut");
  }
}

void AttackState::handleInput(Player& player, Input& input) {
  // TODO
}

void AttackState::update(Player& player) {
  PROFILE_FUNCTION()
  if (player.m_animatedSprite2D->get_animation().match("Attack")
      && player.m_animatedSprite2D->is_playing()) {
    return;
  }

  player.set_weapon_monitoring(false);
  const auto velocity = player.get_velocity();

  if (player.is_on_floor()) {
    if (velocity.x != 0) {
      player.set_state(&Player::running);
      player.m_animatedSprite2D->play("Run");
    } else {
      player.set_state(&Player::standing);
      player.m_animatedSprite2D->play("Idle");
    }
  } else {
    if (velocity.y > 0) {
      player.m_animatedSprite2D->play("JumpOut");
    } else if (velocity.y < 0) {
      player.m_animatedSprite2D->play("JumpIn");
    }
  }
}
