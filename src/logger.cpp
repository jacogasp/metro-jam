#include "logger.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <utility>

namespace core_game {
// LoggerService
LoggerService::LoggerService(Level logLevel, std::string fmt)
    : m_logLevel{logLevel}
    , m_fmt{std::move(fmt)} {
}

std::stringstream LoggerService::getCurrentTime() const {
  using namespace std::chrono;
  std::time_t t = system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm ltime{};
  localtime_r(&t, &ltime);
  std::stringstream ss{""};
  ss << std::put_time(&ltime, m_fmt.c_str());
  return ss;
}

// Logger
Logger::Logger(Level logLevel, std::string fmt)
    : LoggerService{logLevel, std::move(fmt)} {
}

void Logger::log(const std::string_view& msg, Level logLevel) const {
  if (logLevel < m_logLevel)
    return;
  auto currentTime = getCurrentTime().str();
  std::stringstream s{};
  s << "[" << currentTime << "] " << m_levels[logLevel] << " " << msg << '\n';
  godot::UtilityFunctions::print(s.str().c_str());
}

void Logger::debug(const std::string_view& msg) const {
  log(msg, DEBUG);
}
void Logger::info(const std::string_view& msg) const {
  log(msg, INFO);
}
void Logger::warn(const std::string_view& msg) const {
  log(msg, WARN);
}
void Logger::error(const std::string_view& msg) const {
  log(msg, ERROR);
}
} // namespace core_game