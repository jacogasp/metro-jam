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
  [[nodiscard]] TimePoint remaining() const;

 private:
  void fire();
};

class Timer2 {
  std::mutex m_mut;
  std::condition_variable m_cv;
  std::thread m_thread;
  std::atomic<bool> m_running;

 public:
  ~Timer2() {
    stop();
    if (m_thread.joinable()) {
      m_thread.join();
    }
  }

  template<typename Callback, typename Duration>
  void timeout_handler(Callback& fn, Duration const& timeout) {
    std::unique_lock<std::mutex> lk(m_mut);
    auto const start     = std::chrono::steady_clock::now();
    auto const stop_time = start + timeout;
    auto predicate       = [&] {
      auto const expired = std::chrono::steady_clock::now() > stop_time;
      return (expired || !m_running);
    };
    m_cv.wait_until(lk, stop_time, predicate);
    fn();
    auto const stop = std::chrono::steady_clock::now();
    auto const elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();
    std::cerr
        << "Timer exit after " << elapsed << " s, expected: "
        << std::chrono::duration_cast<std::chrono::seconds>(timeout).count()
        << "s \n";
  }

  template<class Callback, class Duration>
  void set_timeout(Callback fn, Duration const& timeout) {
    m_running = true;
    m_thread  = std::thread([&] { timeout_handler(fn, timeout); });
  }

  template<class Callback, class Duration>
  void set_interval(Callback fn, Duration const& timeout) {
    m_running = true;
    m_thread  = std::thread([&] {
      while (m_running) {
        timeout_handler(fn, timeout);
      }
    });
  }

  void stop() {
    m_running = false;
    m_cv.notify_all();
  }
};

#endif // COREGAME_TIMER_HPP
