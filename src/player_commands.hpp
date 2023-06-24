#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

#include <variant>
#include "commands.hpp"

class Player;

template<>
void NullCommand::execute(Player& game_actor);

template<>
void IdleCommand::execute(Player &game_actor);

template<>
void RunCommand::execute(Player &game_actor);

template<>
void JumpCommand::execute(Player& game_actor);

template<>
void JumpOutCommand::execute(Player& game_actor);

template <>
void AttackCommand::execute(Player& game_actor);

#endif // COREGAME_PLAYER_COMMANDS_HPP
