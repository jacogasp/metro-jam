#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

class Player;

struct IdleCommand;
struct RunCommand;
struct JumpCommand;
struct JumpOutCommand;
struct AirJumpCommand;
struct AttackCommand;
struct SlideCommand;

struct IdleCommand {
  void operator()(Player& player) const;
};

struct RunCommand {
  void operator()(Player& player) const;
};

struct JumpCommand {
  void operator()(Player& player) const;
};

struct JumpOutCommand {
  void operator()(Player& player) const;
};

struct AirJumpCommand {
  void operator()(Player& player) const;
};

struct AttackCommand {
  void operator()(Player& player) const;
};

struct SlideCommand {
  void operator()(Player& player) const;
};

#endif // COREGAME_PLAYER_COMMANDS_HPP
