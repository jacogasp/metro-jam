#ifndef COREGAME_PERSIST_HPP
#define COREGAME_PERSIST_HPP

class Persist {
 public:
  virtual ~Persist()  = default;
  virtual void save() = 0;
  virtual void load() = 0;
};

#endif // COREGAME_PERSIST_HPP
