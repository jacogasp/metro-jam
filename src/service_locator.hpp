//
// Created by Jacopo Gasparetto on 22/05/23.
//

#ifndef COREGAME_SERVICE_LOCATOR_HPP
#define COREGAME_SERVICE_LOCATOR_HPP

namespace core_game {
template<typename T>
class Locator {
  inline static T* m_service;

 public:
  static T* getService() {
    assert(m_service);
    return m_service;
  }
  static void registerService(T* service) {
    m_service = service;
  }
};
} // namespace core_game

#endif // COREGAME_SERVICE_LOCATOR_HPP
