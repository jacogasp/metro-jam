#include "main_scene.hpp"
#include "constants.hpp"
#include "hud.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "world.hpp"
#include <godot_cpp/classes/resource_loader.hpp>

auto const SAVE_FILE = [] {
  auto path = core_game::SAVINGS_DIRECTORY.path();
  path.append("wrenchman.json");
  return path;
}();

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
  load();
  m_logger.info("Main scene initialized");
}

void MainScene::on_player_hit() const {
  m_hud->get_lifebar()->lose_life();
}

void MainScene::on_player_gains_life() const {
  m_hud->get_lifebar()->add_life();
  save();
}

void MainScene::on_player_got_powerup(Node* node) const {
  auto const name = node->get_name().get_basename();
  if (m_world) {
    m_world->remove_powerup(name);
  }
  save();
}

void MainScene::save() const {
  try {
    Dictionary d{};
    Dictionary player{};
    player["pos.x"] = m_player->get_global_position().x;
    player["pos.y"] = m_player->get_global_position().y;

    auto superpowers = m_player->get_node<Node>("Superpowers");
    if (superpowers) {
      auto sp = superpowers->get_children();
      Array names;
      for (auto i = 0; i < sp.size(); ++i) {
        auto n = Node::cast_to<Node>(sp[i]);
        names.push_back(n->get_name().get_basename());
      }
      d["superpowers"] = names;
    }
    d["player"] = player;

    core_game::FileWriter file{SAVE_FILE};
    file.write(core_game::dict_to_json(d));
    static const auto msg{std::string{"Game state saved to "}
                          + SAVE_FILE.string()};
    m_logger.info(msg);
  } catch (const std::exception& e) {
    m_logger.error(e.what());
  }
  m_logger.info("Save game");
}

void load_state(Player& player, Dictionary const& state) {
  const float x = static_cast<float>(state["pos.x"]);
  const float y = static_cast<float>(state["pos.y"]);
  player.set_global_position({x, y});
}

static void add_superpower(String const& name, Player& player) {
  auto sp = player.get_node<Node>("Superpowers");
  if (sp == nullptr) {
    std::cerr << "Superpowers node not found\n";
    return;
  }
  auto loader        = ResourceLoader::get_singleton();
  auto resource_path = "res://scenes/superpowers/" + name + ".tscn";
  if (loader->exists(resource_path)) {
    Ref<PackedScene> superpower = loader->load(resource_path);
    auto n = Node2D::cast_to<Node2D>(superpower->instantiate());
    player.pick(n);
  } else {
    std::cerr << "Resource not found " << resource_path.utf8().get_data()
              << '\n';
  }
}

void MainScene::load() {
  if (!std::filesystem::exists(SAVE_FILE)) {
    m_logger.warn("Game save file not found");
    return;
  }
  try {
    core_game::FileReader file{SAVE_FILE};
    const auto body       = file.get();
    const auto game_state = core_game::json_to_dict(body);
    if (!game_state.has("player")) {
      m_logger.error("Game saving data corrupted at " + SAVE_FILE.string());
      return;
    }
    // Load Player state
    load_state(*m_player, game_state["player"]);
    // Load Environment state
    if (game_state.has("superpowers")) {
      Array superpowers = game_state["superpowers"];
      load_superpowers(superpowers);
    }
    static const auto msg{std::string{"Loaded saved game state from "}
                          + SAVE_FILE.string()};
    m_logger.info(msg);
  } catch (const std::exception& e) {
    m_logger.error(e.what());
  }
}

void MainScene::load_superpowers(const Array& superpowers) const {
  using core_game::to_str;
  m_logger.info("Loading superpowers: " + std::to_string(superpowers.size()));
  for (auto i = 0; i < superpowers.size(); ++i) {
    auto const name = String(superpowers[i]);
    if (m_player) {
      add_superpower(name, *m_player);
    }
    if (m_world) {
      m_world->remove_powerup(name);
    }
    m_logger.debug(std::string("Loaded superpower ")
                   + to_str(name.get_basename()));
  }
}
