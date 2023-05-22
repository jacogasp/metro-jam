#ifndef COREGAME_SERVICE_LOCATOR_HPP
#define COREGAME_SERVICE_LOCATOR_HPP

#include <cassert>

namespace core_game {
template<typename T, typename NullLocator>
class Locator {
  inline static T* m_service;

 public:
  static T* getService() {
    //    assert(m_service);
    if (m_service == nullptr) {
      NullLocator::get();
    }
    return m_service;
  }
  static void registerService(T* service) {
    m_service = service;
  }
};
} // namespace core_game

#endif // COREGAME_SERVICE_LOCATOR_HPP
