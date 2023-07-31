#ifndef COREGAME_SUPERPOWERS_HPP
#define COREGAME_SUPERPOWERS_HPP

class Superpower {
 public:
  virtual ~Superpower() = default;
  virtual void activate() = 0;
};

class Immunity : public Superpower {
  bool m_is_active = false;

 public:
  void activate() override;
  void deactivate();
  [[nodiscard]] bool is_active() const;
};

#endif // COREGAME_SUPERPOWERS_HPP
