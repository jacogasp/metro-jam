#include "timer.hpp"

void Timer::start() {
  m_elapsed_time = 0;
  m_running      = true;
}

void Timer::stop() {
  m_running = false;
}

void Timer::tick(double delta_time) {
  if (!m_running) {
    return;
  }

  m_elapsed_time += delta_time;
  if (m_elapsed_time >= m_timeout) {
    fire();
    m_elapsed_time = 0;
  }
}

void Timer::set_callback(Callback fn) {
  m_fn = std::move(fn);
}

void Timer::set_repeat(bool repeat) {
  m_repeat = repeat;
}

void Timer::set_timeout(TimeDelta timeout) {
  m_timeout = timeout;
}

void Timer::fire() {
  try {
    m_fn();
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
  }
  if (!m_repeat) {
    stop();
  }
}

TimeDelta Timer::get_timeout() const {
  return m_timeout;
}
