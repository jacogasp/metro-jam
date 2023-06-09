#include "io.hpp"
#include <godot_cpp/classes/json.hpp>
#include <sstream>

namespace core_game {

godot::Dictionary json_to_dict(const std::string& s) {
  auto d = godot::JSON::parse_string(s.c_str());
  return d;
}

std::string dict_to_json(const godot::Dictionary& d) {
  auto json   = godot::JSON();
  auto result = godot::JSON::stringify(d);
  return result.utf8().get_data();
}

void crate_savings_directory(const std::filesystem::path& path) {
  if (std::filesystem::exists(path)) {
    std::cerr << "Savings directory already exists, skip.\n";
    return;
  }

  bool const success = std::filesystem::create_directories(path);
  if (!success) {
    std::cerr << "Cannot create directory " << path << '\n';
  }
}

FileWriter::FileWriter(const std::filesystem::path& path)
    : m_ofstream{path} {
  if (!m_ofstream.is_open()) {
    throw std::runtime_error("cannot open file");
  }
}

FileWriter::~FileWriter() {
  { m_ofstream.close(); }
}

void FileWriter::write(std::string_view s) {
  m_ofstream << s;
}

FileReader::FileReader(const std::filesystem::path& path)
    : m_ifstream{path} {
  if (!m_ifstream.is_open()) {
    throw std::runtime_error("cannot open file");
  }
}

FileReader::~FileReader() {
  { m_ifstream.close(); }
}

std::string FileReader::get() {
  std::stringstream ss{};
  ss << m_ifstream.rdbuf();
  return ss.str();
}

} // namespace core_game