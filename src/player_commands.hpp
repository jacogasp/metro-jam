#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

#include <variant>

struct Player;
struct NullCommand;
struct IdleCommand;
struct RunCommand;
struct JumpCommand;
struct JumpOutCommand;
struct AirJumpCommand;
struct AttackCommand;
struct SlideCommand;

using PlayerCommand =
    std::variant<NullCommand, IdleCommand, RunCommand, JumpCommand,
                 JumpOutCommand, AirJumpCommand, AttackCommand, SlideCommand>;

void execute(PlayerCommand& command, Player& player);

struct NullCommand {
  void operator()(Player& player) const;
};

struct IdleCommand {
  void operator()(Player& player);
};

struct RunCommand {
  void operator()(Player& player);
};

struct JumpCommand {
  void operator()(Player& player);
};

struct JumpOutCommand {
  void operator()(Player& player);
};

struct AirJumpCommand {
  void operator()(Player& player);
};

struct AttackCommand {
  void operator()(Player& player);
};

struct SlideCommand {
  void operator()(Player& player);
};

#endif // COREGAME_PLAYER_COMMANDS_HPP
