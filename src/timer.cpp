#include "timer.hpp"

void Timer::tick(double delta_time) {
  m_ellapsed_time += delta_time;
  if (m_running && m_ellapsed_time >= m_timeout) {
    fire();
    m_ellapsed_time = 0;
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
