#include "main_scene.hpp"
#include "logger.hpp"

void MainScene::_bind_methods() {
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::LoggerLocator::registerService(&m_logger);
  auto logger = core_game::LoggerLocator::getService();
  logger->info("Main scene initialized");
}