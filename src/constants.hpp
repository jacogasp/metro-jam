#ifndef COREGAME_CONSTANTS_HPP
#define COREGAME_CONSTANTS_HPP

#include "types.hpp"

namespace core_game {
static const Directory SAVINGS_DIRECTORY{
    std::filesystem::temp_directory_path().append("savings")};
}

#endif // COREGAME_CONSTANTS_HPP
