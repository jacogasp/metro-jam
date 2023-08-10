#include "main_scene.hpp"
#include "constants.hpp"
#include "hud.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "world.hpp"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

static auto SAVE_FILE =
    std::string{core_game::SAVINGS_DIRECTORY} + "/game.json";

void MainScene::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_hit"), &MainScene::on_player_hit);
  ClassDB::bind_method(D_METHOD("on_player_gains_life"),
                       &MainScene::on_player_gains_life);
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &MainScene::on_player_got_powerup);
  ADD_SIGNAL(MethodInfo("save"));
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::LoggerLocator::registerService(&m_logger);
}

MainScene::~MainScene() {
  core_game::LoggerLocator::registerService(nullptr);
#ifndef DEBUG_ENABLED
  purge_savings_directory(core_game::SAVINGS_DIRECTORY);
#endif
}

void MainScene::_ready() {
  m_hud    = get_node<HUD>("HUD");
  m_player = get_node<Player>("Player");
  m_world  = get_node<World>("World");
  if (m_player) {
    m_hud->get_lifebar()->set_max_lives(m_player->get_max_lives());
    m_hud->get_lifebar()->set_current_life(m_player->get_max_lives());
  }
  if (!Engine::get_singleton()->is_editor_hint()) {
    create_savings_directory();
    load();
  }
  m_logger.info("Main scene initialized");
}

void MainScene::on_player_hit() const {
  m_hud->get_lifebar()->lose_life();
}

void MainScene::on_player_gains_life() const {
  m_hud->get_lifebar()->add_life();
}

void MainScene::on_player_got_powerup(Node* node) {
  auto const name = node->get_name().get_basename();
  if (m_world) {
    m_world->remove_powerup(name);
  }
  save();
}

void MainScene::save() {
  emit_signal("save");
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

    Dictionary world{};
    world["current_scene"] = m_world->get_current_scene_file_path();
    d["world"]             = world;

    auto file = FileAccess::open(SAVE_FILE.c_str(), FileAccess::WRITE);
    file->store_string(core_game::dict_to_json(d));
    using core_game::to_str;
    static const auto msg{std::string{"Game state saved to "} + SAVE_FILE};
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

void load_state(World& world, Dictionary const& state) {
  String current_scene = state["current_scene"];
  world.load_scene_from_path(current_scene);
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
  if (!FileAccess::file_exists(SAVE_FILE.c_str())) {
    m_logger.warn("Game save file not found");
    return;
  }
  try {
    auto file = FileAccess::open(SAVE_FILE.c_str(), godot::FileAccess::READ);
    const auto body       = file->get_as_text();
    const auto game_state = core_game::json_to_dict(body);
    if (!game_state.has("player")) {
      using core_game::to_str;
      m_logger.error(std::string{"Game saving data corrupted at "} + SAVE_FILE);
      return;
    }
    // Load States
    load_state(*m_player, game_state["player"]);
    load_state(*m_world, game_state["world"]);
    // Load Environment state
    if (game_state.has("superpowers")) {
      Array superpowers = game_state["superpowers"];
      load_superpowers(superpowers);
    }
    using core_game::to_str;
    static const auto msg{std::string{"Loaded saved game state from "}
                          + SAVE_FILE};
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

void MainScene::create_savings_directory() {
  auto dir = DirAccess::open("user://");
  if (!dir->dir_exists(core_game::SAVINGS_DIRECTORY)) {
    dir->make_dir(core_game::SAVINGS_DIRECTORY);
  }
}

void MainScene::purge_savings_directory() {
  auto& path = core_game::SAVINGS_DIRECTORY;
  auto dir   = godot::DirAccess();
  if (dir.dir_exists(path)) {
    dir.remove(path);
    std::cerr << "Savings directory removed.\n";
  } else {
    std::cerr << "Savings directory " << core_game::to_str(path)
              << " not found. Won't delete.\n";
  }
}
