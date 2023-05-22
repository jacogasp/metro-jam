#ifndef COREGAME_LOGGER_H
#define COREGAME_LOGGER_H

#include <array>
#include <string>
#include <string_view>

namespace core_game {

class LoggerService {
 public:
  enum Level { DEBUG, INFO, WARN, ERROR };

 protected:
  Level m_logLevel;
  std::string m_fmt;
  [[nodiscard]] std::stringstream getCurrentTime() const;
  static constexpr std::array m_levels{"DEBUG", "INFO", "WARN", "ERROR"};

 public:
  explicit LoggerService(Level logLevel  = INFO,
                         std::string fmt = "%Y-%m-%d %H:%M:%S");
  virtual ~LoggerService() = default;

  virtual void log(const std::string_view& msg, Level logLevel) const = 0;
  virtual void debug(const std::string_view& msg) const               = 0;
  virtual void info(const std::string_view& msg) const                = 0;
  virtual void warn(const std::string_view& msg) const                = 0;
  virtual void error(const std::string_view& msg) const               = 0;
};

class LoggerNull : public LoggerService {
 public:
  void log(const std::string_view& msg, Level logLevel) const final {
  }
  void debug(const std::string_view& msg) const final {
  }
  void info(const std::string_view& msg) const final {
  }
  void warn(const std::string_view& msg) const final {
  }
  void error(const std::string_view& msg) const final {
  }
};

class Logger : public LoggerService {
 public:
  explicit Logger(Level logLevel = INFO, std::string fmt = "%Y-%m-%d %H:%M:%S");
  void log(const std::string_view& msg, Level logLevel) const override;
  void debug(const std::string_view& msg) const override;
  void info(const std::string_view& msg) const override;
  void warn(const std::string_view& msg) const override;
  void error(const std::string_view& msg) const override;
};
} // namespace core_game

#endif // COREGAME_LOGGER_H
