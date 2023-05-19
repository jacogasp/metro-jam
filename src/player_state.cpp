
#include "player_state.hpp"
#include "player.hpp"

#include <godot_cpp/classes/input.hpp>
using namespace godot;


void StandingState::handleInput(Player& player, Input& input) {
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    velocity.y -= 350;
    player.set_velocity(velocity);
    player.set_state(&Player::jumping);
  } else if (abs(velocity.x) > 0) {
    player.set_animation("Walk");
    player.set_state(&Player::walking);
  }
}

void JumpingState::handleInput(Player& player, Input& input) {
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::air_jumping);
    auto velocity = player.get_velocity();
    velocity.y -= 350;
    player.set_velocity(velocity);
  }
}

void JumpingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.y == 0) {
    player.set_state(&Player::standing);
  }
}

void AirJumpingState::handleInput(Player& player, Input& input) {}

void AirJumpingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.y == 0) {
    player.set_state(&Player::standing);
  }
}

void WalkingState::handleInput(Player& player, Input& input) {
  if (input.is_action_just_pressed("jump")) {
    player.set_state(&Player::jumping);
    auto velocity = player.get_velocity();
    velocity.y -= 350;
    player.set_velocity(velocity);
  }
}

void WalkingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (velocity.length() == 0) {
    player.set_state(&Player::standing);
  }
}
