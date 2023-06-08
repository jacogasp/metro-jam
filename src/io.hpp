

#ifndef COREGAME_IO_HPP
#define COREGAME_IO_HPP

#include <godot_cpp/variant/dictionary.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

namespace core_game {

godot::Dictionary json_to_dict(const std::string& s);
std::string dict_to_json(const godot::Dictionary& d);
void crate_savings_directory(const std::filesystem::path& path);

class File {
  std::ofstream m_ofstream;
  std::ifstream m_ifstream;

 public:
  explicit File(std::string_view path);
  ~File();
  std::string get();
  void write(std::string_view s);
};
} // namespace core_game

#endif // COREGAME_IO_HPP
