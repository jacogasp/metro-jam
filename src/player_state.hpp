#ifndef PLAYER_STATE_HPP
#define PLAYER_STATE_HPP

#include <memory>

namespace godot {
class Input;
}
class Player;

struct PlayerState {
  virtual ~PlayerState() = default;
  virtual void handleInput(Player&, godot::Input&){}
  virtual void update(Player&){}
};

struct StandingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

struct RunningState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

struct JumpingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

struct FallingState : public PlayerState {
  void update(Player& player) override;
};

struct AirJumpingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

struct AttackState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

struct SlideState : public PlayerState {
  void update(Player& player) override;
};

struct DyingState : public PlayerState {
  void update(Player& player) override;
};

#endif // PLAYER_STATE_HPP
