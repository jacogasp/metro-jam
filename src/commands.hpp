#ifndef COREGAME_COMMANDS_HPP
#define COREGAME_COMMANDS_HPP

#include <variant>

class NullCommand;
class IdleCommand;
class RunCommand;
class JumpCommand;
class JumpOutCommand;
class AttackCommand;

using Command = std::variant<NullCommand, IdleCommand, RunCommand, JumpCommand,
                             JumpOutCommand, AttackCommand>;


template<typename GameActor>
static void execute(Command& command, GameActor& player) {
  std::visit([&player](auto cmd) { cmd.execute(player); }, command);
}

class NullCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

class IdleCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

class RunCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

class JumpCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

class JumpOutCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

class AttackCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor);
};

#endif // COREGAME_COMMANDS_HPP
