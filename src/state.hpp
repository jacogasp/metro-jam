#ifndef COREGAME_STATE_HPP
#define COREGAME_STATE_HPP

template<typename GameActor>
struct State {
  virtual ~State() = default;
  virtual void update(GameActor&) const {
  }
};

#endif // COREGAME_STATE_HPP
