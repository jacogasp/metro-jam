#include "main_scene.hpp"
#include "hud.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "world.hpp"

void MainScene::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_hit"), &MainScene::on_player_hit);
  ClassDB::bind_method(D_METHOD("on_player_gains_life"),
                       &MainScene::on_player_gains_life);
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &MainScene::on_player_got_powerup);
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::LoggerLocator::registerService(&m_logger);
}
MainScene::~MainScene() {
  core_game::LoggerLocator::registerService(nullptr);
}

void MainScene::_ready() {
  m_hud    = get_node<HUD>("HUD");
  m_player = get_node<Player>("Player");
  m_world  = get_node<World>("World");
  if (m_player) {
    m_hud->get_lifebar()->set_max_lives(m_player->get_max_lives());
    m_hud->get_lifebar()->set_current_life(m_player->get_max_lives());
  }
  m_logger.info("Main scene initialized");
}

void MainScene::on_player_hit() const {
  m_hud->get_lifebar()->lose_life();
}

void MainScene::on_player_gains_life() const {
  m_hud->get_lifebar()->add_life();
  save();
}

void MainScene::on_player_got_powerup(Node*) const {
  save();
}

void MainScene::save() const {
  m_player->save();
  m_world->save();
  m_logger.info("Save game");
}
