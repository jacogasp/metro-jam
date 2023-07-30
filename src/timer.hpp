#ifndef COREGAME_TIMER_HPP
#define COREGAME_TIMER_HPP

#include "io.hpp"
#include <functional>
#include <future>
#include <iostream>
#include <utility>

using Callback = std::function<void()>;

using TimePoint = double;
using TimeDelta = double;

class Timer {
  std::future<void> m_future;
  TimeDelta m_timeout{0};
  TimePoint m_elapsed_time{0};
  bool m_running{false};
  bool m_repeat{false};
  Callback m_fn;

 public:
  void start();
  void stop();
  void tick(TimeDelta delta_time);
  void set_callback(Callback fn);
  void set_repeat(bool repeat);
  void set_timeout(TimeDelta timeout);
  [[nodiscard]] TimeDelta get_timeout() const;
  [[nodiscard]] bool is_running() const;

 private:
  void fire();
};

#endif // COREGAME_TIMER_HPP
