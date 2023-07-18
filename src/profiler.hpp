#ifndef COREGAME_PROFILER_HPP
#define COREGAME_PROFILER_HPP

#include <chrono>
#include <cinttypes>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

namespace core_game {

struct ProfilerResult {
  std::string name;
  long long start, end;
  size_t thread_id;
};

struct InstrumentationSession {
  std::string name;
};

class Instrumentor {
  std::unique_ptr<InstrumentationSession> m_session;
  std::ofstream m_ostream;
  int m_profile_count;
  std::mutex m_lock;

 public:
  Instrumentor()
      : m_profile_count(0) {
  }

  void begin_session(const std::string& filepath = "profiler.json") {
    m_ostream.open(filepath);
    m_session = std::make_unique<InstrumentationSession>();
    write_header();
  }

  void end_session() {
    write_footer();
    m_ostream.close();
    m_profile_count = 0;
  }

  static Instrumentor& get() {
    static Instrumentor instance;
    return instance;
  }

  void write_profile(const ProfilerResult& result) {
    std::lock_guard<std::mutex> lock(m_lock);

    if (m_profile_count++ > 0)
      m_ostream << ",";

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_ostream << "{";
    m_ostream << R"("cat":"function",)";
    m_ostream << R"("dur":")" << (result.end - result.start) << R"(",)";
    m_ostream << R"("name":")" << name << R"(",)";
    m_ostream << R"("ph":"X",)";
    m_ostream << R"("pid":0,)";
    m_ostream << R"("tid":)" << result.thread_id << ",";
    m_ostream << R"("ts":)" << result.start;
    m_ostream << "}";

    m_ostream.flush();
  }

 private:
  void write_header() {
    m_ostream << R"({"otherData": {},"traceEvents":[)";
    m_ostream.flush();
  }

  void write_footer() {
    m_ostream << "]}";
    m_ostream.flush();
  }
};

using namespace std::chrono;
class TimerInstrument {
  const std::string m_name;
  time_point<high_resolution_clock> m_start_timepoint;
  bool m_stopped;

 public:
  explicit TimerInstrument(std::string name)
      : m_name{std::move(name)}
      , m_stopped{false} {
    m_start_timepoint = high_resolution_clock::now();
  }

  ~TimerInstrument() {
    if (!m_stopped) {
      stop();
    }
  }

  void stop() {
    auto end_timepoint = high_resolution_clock::now();
    long long start    = time_point_cast<microseconds>(m_start_timepoint)
                          .time_since_epoch()
                          .count();
    long long end =
        time_point_cast<microseconds>(end_timepoint).time_since_epoch().count();

    auto threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Instrumentor::get().write_profile({m_name, start, end, threadID});

    m_stopped = true;
  }
};
} // namespace core_game

#if CORE_GAME_PROFILING
#  define PROFILE_SCOPE(name) core_game::TimerInstrument timer(name)
#  define PROFILE_FUNCTION()  PROFILE_SCOPE(__FUNCTION__)
#else
#  define PROFILE_SCOPE (name)
#endif

#endif // COREGAME_PROFILER_HPP
