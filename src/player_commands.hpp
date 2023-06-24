#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

#include <variant>

class Player;

class NullCommand;
class IdleCommand;
class RunCommand;
class JumpCommand;
class JumpOutCommand;
class AttackCommand;

using PlayerCommand = std::variant<NullCommand, IdleCommand, RunCommand,
                                   JumpCommand, JumpOutCommand, AttackCommand>;

class NullCommand {
 public:
  static void execute(Player& player);
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

class AttackCommand {
 public:
  static void execute(Player& player);
};

#endif // COREGAME_PLAYER_COMMANDS_HPP
