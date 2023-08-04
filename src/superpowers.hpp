#ifndef COREGAME_SUPERPOWERS_HPP
#define COREGAME_SUPERPOWERS_HPP

#include <map>
#include <memory>
#include <unordered_map>

class Superpower;

class Superpower {
 public:
  enum Type { Immunity, Slide };
  virtual ~Superpower()   = default;
  virtual void activate() = 0;
};

using PowerUps =
    std::unordered_map<Superpower::Type, std::unique_ptr<Superpower>>;

class Immunity : public Superpower {
  bool m_is_active = false;

 public:
  void activate() override;
  void deactivate();
  [[nodiscard]] bool is_active() const;
};

#endif // COREGAME_SUPERPOWERS_HPP
