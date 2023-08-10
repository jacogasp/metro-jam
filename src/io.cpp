#include "io.hpp"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <iostream>
#include <sstream>
#include <system_error>

namespace core_game {

godot::Dictionary json_to_dict(godot::String const& s) {
  return godot::JSON::parse_string(s);
}

godot::String dict_to_json(const godot::Dictionary& d) {
  auto json   = godot::JSON();
  auto result = godot::JSON::stringify(d);
  return result;
}


bool close_to(float a, float b) {
  return abs(a - b) < CMP_EPSILON;
}

bool close_to(double a, double b) {
  return abs(a - b) < UNIT_EPSILON;
}

const char* to_str(const godot::String& g_string) {
  return g_string.utf8().get_data();
}
} // namespace core_game