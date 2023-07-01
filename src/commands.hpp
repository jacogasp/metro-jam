#ifndef COREGAME_COMMANDS_HPP
#define COREGAME_COMMANDS_HPP

#include <variant>

class NullCommand;
class IdleCommand;
class RunCommand;
class JumpCommand;
class JumpOutCommand;
class AirJumpCommand;
class AttackCommand;
class SlideCommand;

using Command = std::variant<NullCommand, IdleCommand, RunCommand, JumpCommand,
                             JumpOutCommand, AirJumpCommand, AttackCommand, SlideCommand>;


template<typename GameActor>
static void execute(Command& command, GameActor& player) {
  std::visit([&player](auto cmd) { cmd.execute(player); }, command);
}

class NullCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class IdleCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class RunCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class JumpCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class JumpOutCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class AirJumpCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class AttackCommand {
 public:
  template<typename GameActor>
  void execute(GameActor& game_actor) const;
};

class SlideCommand {
 public:
  template <typename GameActor>
  void execute(GameActor& game_actor) const;
};

class FlipCommand {
 public:
  template <typename GameActor>
  void execute(GameActor& game_actor) const;
};

#endif // COREGAME_COMMANDS_HPP
