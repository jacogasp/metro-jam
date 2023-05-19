#ifndef PLAYER_STATE_HPP
#define PLAYER_STATE_HPP

#include <memory>

namespace godot {
class Input;
}
class Player;

class PlayerState {
 public:
  ~PlayerState() = default;
  virtual void handleInput(Player& player, godot::Input& input){};
  virtual void update(Player& player){};
};

class StandingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
};

class WalkingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player &player) override;
};

class JumpingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

class AirJumpingState : public PlayerState {
  void handleInput(Player& player, godot::Input& input) override;
  void update(Player& player) override;
};

#endif // PLAYER_STATE_HPP
