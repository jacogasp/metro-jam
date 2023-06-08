#include "io.hpp"
#include <godot_cpp/classes/json.hpp>

namespace core_game {

godot::Dictionary json_to_dict(const std::string& s) {
  auto json           = godot::JSON();
  godot::Dictionary d = json.get_data();
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

File::File(const std::string_view path)
    : m_ofstream{path}
    , m_ifstream{path} {
  if (!m_ofstream.is_open()) {
    throw std::runtime_error("cannot open file");
  }
}

File::~File() {
  {
    m_ofstream.close();
    m_ifstream.close();
  }
}

std::string File::get() {
  std::string s;
  m_ifstream >> s;
  return s;
}

void File::write(std::string_view s) {
  m_ofstream << s;
}

} // namespace core_game