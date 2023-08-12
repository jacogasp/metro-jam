
#include "player_state.hpp"
#include "io.hpp"
#include "player.hpp"
#include "player_commands.hpp"
#include "profiler.hpp"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

static constexpr auto idle     = IdleCommand();
static constexpr auto jump     = JumpCommand();
static constexpr auto run      = RunCommand();
static constexpr auto fall     = FallCommand();
static constexpr auto air_jump = AirJumpCommand();
static constexpr auto attack   = AttackCommand();
static constexpr auto grenade  = GrenadeCommand();
static constexpr auto slide    = SlideCommand();

void StandingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION();
  auto velocity = player.get_velocity();
  if (input.is_action_just_pressed("jump")) {
    jump(player);
  } else if (!core_game::close_to(velocity.x, 0.f)) {
    run(player);
  } else if (input.is_action_just_pressed("attack")) {
    attack(player);
  } else if (input.is_action_just_pressed("grenade")) {
    grenade(player);
  }
}

void StandingState::update(Player& player) {
  PROFILE_FUNCTION();
  auto const velocity = player.get_velocity();
  if (!player.is_on_ground() && velocity.y < 0) {
    fall(player);
  }
}

void JumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION();
  if (input.is_action_just_pressed("jump")
      && player.has_node("Superpowers/AirJumpPower")) {
    air_jump(player);
  } else if (input.is_action_just_pressed("attack")) {
    attack(player);
  } else if (input.is_action_just_pressed("grenade")) {
    grenade(player);
  }
}

void JumpingState::update(Player& player) {
  PROFILE_FUNCTION();
  auto const velocity = player.get_velocity();
  if (velocity.y < 0) {
    return;
  }
  if (player.is_on_ground()) {
    core_game::close_to(velocity.x, 0.f) ? idle(player) : run(player);
  } else {
    player.m_animatedSprite2D->play("JumpOut");
  }
}

void FallingState::update(Player& player) {
  auto const velocity = player.get_velocity();
  if (player.is_on_ground()) {
    core_game::close_to(velocity.x, 0.f) ? idle(player) : run(player);
  }
}

void AirJumpingState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION();
  if (input.is_action_just_pressed("attack")) {
    attack(player);
  } else if (input.is_action_just_pressed("grenade")) {
    grenade(player);
  }
}

void AirJumpingState::update(Player& player) {
  PROFILE_FUNCTION();
  auto const velocity = player.get_velocity();
  if (velocity.y > 0) {
    fall(player);
  }
}

void RunningState::handleInput(Player& player, Input& input) {
  PROFILE_FUNCTION();
  if (input.is_action_just_pressed("jump")) {
    player.m_audio_footsteps->stop();
    jump(player);
  } else if (input.is_action_just_pressed("attack")) {
    attack(player);
  } else if (input.is_action_just_pressed("grenade")) {
    grenade(player);
  } else if (input.is_action_just_pressed("slide")) {
    player.m_audio_footsteps->stop();
    slide(player);
  }
}

void RunningState::update(Player& player) {
  PROFILE_FUNCTION();
  auto const velocity = player.get_velocity();
  if (player.is_on_ground()) {
    if (core_game::close_to(velocity.x, 0.f)) {
      player.m_audio_footsteps->stop();
      idle(player);
    }
  } else if (velocity.y > 0) {
    player.m_audio_footsteps->stop();
    fall(player);
  }
}

void AttackState::handleInput(Player&, Input&) {
  // TODO
}

void AttackState::update(Player& player) {
  PROFILE_FUNCTION();
  if (player.m_animatedSprite2D->get_animation().begins_with("Attack")
      && player.m_animatedSprite2D->is_playing()) {
    return;
  }

  const auto velocity = player.get_velocity();
  if (player.is_on_ground()) {
    core_game::close_to(velocity.x, 0.f) ? idle(player) : run(player);
  } else {
    player.m_animatedSprite2D->play(velocity.y > 0 ? "JumpOut" : "JumpIn");
  }
}

void SlideState::update(Player& player) {
  PROFILE_FUNCTION();
  auto velocity = player.get_velocity();
  if (player.m_animatedSprite2D->is_playing()
      && player.m_animatedSprite2D->get_animation().begins_with("Slide")) {
    auto const direction = static_cast<float>(player.m_direction);
    velocity += Vector2{direction * player.m_slide_speed, 0};
    player.set_velocity(velocity);
    return;
  }

  if (velocity.y < 0) {
    return;
  }
  if (player.is_on_ground()) {
    core_game::close_to(velocity.x, 0.f) ? idle(player) : run(player);
  } else {
    fall(player);
  }
}
