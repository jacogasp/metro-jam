#ifndef COREGAME_PLAYER_COMMANDS_HPP
#define COREGAME_PLAYER_COMMANDS_HPP

#include <variant>
#include "commands.hpp"

class Player;

template<>
void NullCommand::execute(Player& game_actor) const;

template<>
void IdleCommand::execute(Player &game_actor) const;

template<>
void RunCommand::execute(Player &game_actor) const;

template<>
void JumpCommand::execute(Player& game_actor) const;

template<>
void JumpOutCommand::execute(Player& game_actor) const;

template <>
void AirJumpCommand::execute(Player& game_actor) const;

template <>
void AttackCommand::execute(Player& game_actor) const;

template <>
void SlideCommand::execute(Player& game_actor) const;

#endif // COREGAME_PLAYER_COMMANDS_HPP
