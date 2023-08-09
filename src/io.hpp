

#ifndef COREGAME_IO_HPP
#define COREGAME_IO_HPP

#include "types.hpp"
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace core_game {

godot::Dictionary json_to_dict(godot::String const& s);
godot::String dict_to_json(const godot::Dictionary& d);
void crate_savings_directory(const Directory& directory);
void purge_savings_directory(const Directory& directory);
const char* to_str(const godot::String& g_string);

bool close_to(float a, float b);
bool close_to(double a, double b);
} // namespace core_game

#endif // COREGAME_IO_HPP
