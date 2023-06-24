
#include "player_state.hpp"
#include "player.hpp"
#include "player_commands.hpp"
#include "profiler.hpp"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

void StandingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  auto velocity         = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    command = JumpCommand();
  } else if (velocity.x != 0) {
    command = RunCommand();
  } else if (input.is_action_just_pressed("attack")) {
    command = AttackCommand();
  }
  execute(command, player);
}

void StandingState::update(Player& player) {
  Command command = NullCommand();
  auto const velocity   = player.get_velocity();
  if (player.is_on_floor()) {
    command = IdleCommand();
  } else if (velocity.y > 0) {
    command = JumpOutCommand();
  }
  execute(command, player);
}

void JumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  if (input.is_action_just_pressed("jump")) {
    command = JumpCommand();
  } else if (input.is_action_just_pressed("attack")) {
    command = AttackCommand();
  }
  execute(command, player);
}

void JumpingState::update(Player& player) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  auto const velocity   = player.get_velocity();
  if (velocity.y > 0) {
    command = JumpOutCommand();
  } else if (velocity.y == 0 && player.is_on_floor()) {
    if (velocity.x == 0) {
      command = IdleCommand();
    } else {
      command = RunCommand();
    }
  }
  execute(command, player);
}

void AirJumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  if (input.is_action_just_pressed("attack")) {
    command = AttackCommand();
  }
  execute(command, player);
}

void AirJumpingState::update(Player& player) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  auto const velocity   = player.get_velocity();
  if (velocity.y > 0) {
    command = JumpOutCommand();
  }
  if (player.is_on_floor()) {
    if (velocity.x == 0) {
      command = IdleCommand();
    } else {
      command = RunCommand();
    }
  }
  execute(command, player);
}

void RunningState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  if (input.is_action_just_pressed("jump")) {
    command = JumpCommand();
  } else if (input.is_action_just_pressed("attack")) {
    command = AttackCommand();
  }
  execute(command, player);
}

void RunningState::update(Player& player) {
  PROFILE_FUNCTION()
  Command command = NullCommand();
  auto const velocity   = player.get_velocity();
  if (player.is_on_floor()) {
    if (velocity.x != 0) {
      command = RunCommand();
    } else {
      command = IdleCommand();
    }
  } else if (velocity.y > 0) {
    command = JumpOutCommand();
  }
  execute(command, player);
}

void AttackState::handleInput(Player& player, Input& input) {
  // TODO
}

void AttackState::update(Player& player) {
  PROFILE_FUNCTION()
  if (player.m_animatedSprite2D->get_animation().begins_with("Attack")
      && player.m_animatedSprite2D->is_playing()) {
    return;
  }

  player.set_weapon_monitoring(false);
  Command command = NullCommand();
  const auto velocity   = player.get_velocity();
  if (player.is_on_floor()) {
    if (velocity.x != 0) {
      command = RunCommand();
    } else {
      command = IdleCommand();
    }
  } else {
    if (velocity.y > 0) {
      player.m_animatedSprite2D->play("JumpOut");
    } else if (velocity.y < 0) {
      player.m_animatedSprite2D->play("JumpIn");
    }
  }
  execute(command, player);
}
