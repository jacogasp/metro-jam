

#ifndef COREGAME_IO_HPP
#define COREGAME_IO_HPP

#include <godot_cpp/variant/dictionary.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace core_game {

godot::Dictionary json_to_dict(const std::string& s);
std::string dict_to_json(const godot::Dictionary& d);
void crate_savings_directory(const std::filesystem::path& path);

class FileWriter {
  std::ofstream m_ofstream;

 public:
  explicit FileWriter(const std::filesystem::path& path);
  ~FileWriter();
  void write(std::string_view s);
};

class FileReader {
  std::ifstream m_ifstream;

 public:
  explicit FileReader(const std::filesystem::path& path);
  ~FileReader();
  std::string get();
};

} // namespace core_game

#endif // COREGAME_IO_HPP
