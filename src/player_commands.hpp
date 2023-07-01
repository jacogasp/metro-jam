#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

#include <variant>

class Player;
class NullCommand;
class IdleCommand;
class RunCommand;
class JumpCommand;
class JumpOutCommand;
class AirJumpCommand;
class AttackCommand;
class SlideCommand;

class PlayerCommandRaw {};

using PlayerCommand =
    std::variant<NullCommand, IdleCommand, RunCommand, JumpCommand,
                 JumpOutCommand, AirJumpCommand, AttackCommand, SlideCommand>;

void execute(PlayerCommand& command, Player& player);

class NullCommand : PlayerCommandRaw {
 public:
  void execute(Player& game_actor) const;
};

class IdleCommand {
 public:
  static void execute(Player& player);
};

class RunCommand {
 public:
  static void execute(Player& player);
};

class JumpCommand {
 public:
  static void execute(Player& player);
};

class JumpOutCommand {
 public:
  static void execute(Player& player);
};

class AirJumpCommand {
 public:
  static void execute(Player& player);
};

class AttackCommand {
 public:
  static void execute(Player& player);
};

class SlideCommand {
 public:
  static void execute(Player& player);
};

#endif // COREGAME_PLAYER_COMMANDS_HPP
