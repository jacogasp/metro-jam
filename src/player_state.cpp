
#include "player_state.hpp"
#include "player.hpp"

#include <godot_cpp/classes/input.hpp>
using namespace godot;

void StandingState::handleInput(Player& player, Input& input) {
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::jumping);
    player.set_animation("JumpIn");
    velocity.y = -player.get_jump_force();
    player.set_velocity(velocity);
  } else if (velocity.x != 0) {
    player.set_animation("Run");
    player.set_state(&Player::walking);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.set_animation("Attack");
    player.set_is_animation_locked(true);
  }
}

void JumpingState::handleInput(Player& player, Input& input) {
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::air_jumping);
    player.set_animation("JumpIn");
    velocity.y = -player.get_air_jump_force();
    player.set_velocity(velocity);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.set_animation("Attack");
    player.set_is_animation_locked(true);
  }
}

void JumpingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.y > 0) {
    player.set_animation("JumpOut");
  } else if (velocity.y == 0 && player.is_on_floor()) {
    if (velocity.x == 0) {
      player.set_animation("Idle");
      player.set_state(&Player::standing);
    } else {
      player.set_animation("Run");
      player.set_state(&Player::walking);
    }
  }
}

void AirJumpingState::handleInput(Player& player, Input& input) {
  if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.set_animation("Attack");
    player.set_is_animation_locked(true);
  }
}

void AirJumpingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.y > 0) {
    player.set_animation("JumpOut");
  }

  if (player.is_on_floor()) {
    if (velocity.x == 0) {
      player.set_animation("Idle");
      player.set_state(&Player::standing);
    } else {
      player.set_animation("Run");
      player.set_state(&Player::walking);
    }
  }
}

void WalkingState::handleInput(Player& player, Input& input) {
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::jumping);
    player.set_animation("JumpIn");
    auto velocity = player.get_velocity();
    velocity.y    = -player.get_jump_force();
    player.set_velocity(velocity);
  } else if (input.is_action_just_pressed("attack")) {
    player.set_state(&Player::attacking);
    player.set_animation("Attack");
    player.set_is_animation_locked(true);
  }
}

void WalkingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.x == 0) {
    player.set_animation("Idle");
    player.set_state(&Player::standing);
  }
}

void AttackState::handleInput(Player& player, Input& input) {
  // TODO
}

void AttackState::update(Player& player) {
  const auto is_animation_locked = player.get_is_animation_locked();

  if (is_animation_locked) {
    return;
  }

  const auto velocity = player.get_velocity();

  if (velocity.y != 0) {
    player.set_state(&Player::jumping);
    if (velocity.y > 0) {
      player.set_animation("JumpOut");
    }
    else {
      player.set_animation("JumpIn");
    }
    return;
  }

  if (velocity.x != 0) {
    player.set_state(&Player::walking);
    player.set_animation("Run");
  }
  else
  {
    player.set_state(&Player::standing);
    player.set_animation("Idle");
  }
}
