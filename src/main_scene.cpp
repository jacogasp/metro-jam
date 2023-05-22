#include "main_scene.hpp"
#include "logger.hpp"
#include "service_locator.hpp"

void MainScene::_bind_methods() {
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::Locator<core_game::Logger>::registerService(&m_logger);
  auto logger = core_game::Locator<core_game::Logger>::getService();
  logger->info("Main scene initialized");
}