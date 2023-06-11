#include "main_scene.hpp"
#include "hud.hpp"
#include "logger.hpp"
#include "world.hpp"

void MainScene::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_hit"), &MainScene::on_player_hit);
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::LoggerLocator::registerService(&m_logger);
}
MainScene::~MainScene() {
  core_game::LoggerLocator::registerService(nullptr);
}

void MainScene::_ready() {
  m_hud = get_node<HUD>("HUD");
  m_logger.info("Main scene initialized");
}

void MainScene::on_player_hit() const {
  std::cerr << "on player hit \n";
  m_hud->get_lifebar()->lose_life();
}
