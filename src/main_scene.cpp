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
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_joypad_button.hpp>
#include <godot_cpp/classes/input_event_joypad_motion.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

static auto SAVE_FILE =
    std::string{core_game::SAVINGS_DIRECTORY} + "/game.json";

void MainScene::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_player_hit"), &MainScene::on_player_hit);
  ClassDB::bind_method(D_METHOD("on_player_gains_life"),
                       &MainScene::on_player_gains_life);
  ClassDB::bind_method(D_METHOD("on_player_got_powerup"),
                       &MainScene::on_player_got_powerup);
  ClassDB::bind_method(D_METHOD("save"), &MainScene::save);
  ClassDB::bind_method(D_METHOD("start_game"), &MainScene::start_game);
  ClassDB::bind_method(D_METHOD("resume_game"), &MainScene::resume);
  ClassDB::bind_method(D_METHOD("continue_game"), &MainScene::continue_game);
  ClassDB::bind_method(D_METHOD("complete_game"), &MainScene::complete_game);
  ClassDB::bind_method(D_METHOD("restart_game"), &MainScene::restart_game);
  ClassDB::bind_method(D_METHOD("game_over"), &MainScene::game_over);
  ClassDB::bind_method(D_METHOD("quit"), &MainScene::quit);
  ADD_SIGNAL(MethodInfo("save"));
  ADD_SIGNAL(MethodInfo("using_joypad_changed"));
}

MainScene::MainScene()
    : m_logger(core_game::LoggerService::DEBUG) {
  core_game::LoggerLocator::registerService(&m_logger);
}

MainScene::~MainScene() {
  try {
    core_game::LoggerLocator::registerService(nullptr);
#ifndef DEBUG_ENABLED
    purge_savings_directory();
#endif
  } catch (...) {
  }
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
    pause();
    m_hud->go_to(HUD::start);
  }
  m_logger.info("Main scene initialized");
}

void MainScene::_input(const Ref<InputEvent>& event) {
  if (event->is_action_released("pause")) {
    pause();
    return;
  }

  auto maybe_joypad_button = cast_to<InputEventJoypadButton>(event.ptr());
  auto maybe_joypad_motion = cast_to<InputEventJoypadMotion>(event.ptr());
  if (maybe_joypad_button || maybe_joypad_motion) {
    m_using_joypad = true;
    emit_signal("using_joypad_changed", m_using_joypad);
  }
  auto maybe_keyboard = cast_to<InputEventKey>(event.ptr());
  if (maybe_keyboard) {
    m_using_joypad = false;
    emit_signal("using_joypad_changed", m_using_joypad);
  }
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
}

bool MainScene::is_using_joypad() {
  return false;
}

void MainScene::quit() const {
  get_tree()->quit();
}

void MainScene::save() {
  emit_signal("save");
  try {
    Dictionary d{};
    Dictionary player{};
    player["pos.x"] = m_player->get_global_position().x;
    player["pos.y"] = m_player->get_global_position().y;

    auto superpowers = m_player->get_node<Node2D>("Superpowers");
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
  m_saved = true;
  m_logger.info("Save game");
}

void load_state(Player& player, Dictionary const& state) {
  const float x = static_cast<float>(state["pos.x"]);
  const float y = static_cast<float>(state["pos.y"]);
  player.restore_health();
  player.set_global_position({x, y});
}

void load_state(World& world, Dictionary const& state) {
  String current_scene = state["current_scene"];
  world.load_scene_from_path(current_scene);
}

static Node2D* add_superpower(String const& name, Player& player) {
  auto sp = player.get_node<Node2D>("Superpowers");
  if (sp == nullptr) {
    std::cerr << "Superpowers node not found\n";
    return nullptr;
  }
  auto loader        = ResourceLoader::get_singleton();
  auto resource_path = "res://scenes/superpowers/" + name + ".tscn";
  if (loader->exists(resource_path)) {
    Ref<PackedScene> superpower = loader->load(resource_path);
    auto n     = Node2D::cast_to<Node2D>(superpower->instantiate());
    auto area  = n->get_node<Area2D>("Area2D");
    auto label = n->get_node<Label>("Label");
    area->set_monitoring(false);
    area->queue_free();
    label->queue_free();
    n->set_name(name);
    player.add_powerup(n);
    return n;
  } else {
    std::cerr << "Resource not found " << resource_path.utf8().get_data()
              << '\n';
  }
  return nullptr;
}

bool MainScene::load() {
  m_loading = true;
  m_logger.info("Start loading savings...");
  if (!FileAccess::file_exists(SAVE_FILE.c_str())) {
    m_logger.warn("Game save file not found");
    m_loading = false;
    return false;
  }
  try {
    auto file = FileAccess::open(SAVE_FILE.c_str(), godot::FileAccess::READ);
    const auto body       = file->get_as_text();
    const auto game_state = core_game::json_to_dict(body);
    if (!game_state.has("player")) {
      using core_game::to_str;
      m_logger.error(std::string{"Game saving data corrupted at "} + SAVE_FILE);
      m_loading = false;
      return false;
    }
    // Load States
    auto player = get_node<Player>("Player");
    if (player) {
      load_state(*player, game_state["player"]);
    }
    auto world = get_node<World>("World");
    if (world) {
      load_state(*world, game_state["world"]);
    }
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
  m_logger.info("Loading complete.");
  m_loading = false;
  return true;
}

void MainScene::_pause() const {
  get_tree()->set_pause(true);
}

void MainScene::pause() const {
  _pause();
  m_hud->go_to(HUD::pause);
}

void MainScene::resume() const {
  get_tree()->set_pause(false);
  m_hud->go_to(HUD::in_game);
}

void MainScene::start_game() const {
  resume();
}

void MainScene::continue_game() {
  m_player->reset();
  if (!load()) {
    get_tree()->reload_current_scene();
  }
  m_hud->go_to(HUD::in_game);
  m_hud->get_lifebar()->reset();
  resume();
}

void MainScene::complete_game() {
  m_hud->go_to(HUD::the_end);
  _pause();
}

void MainScene::restart_game() {
  purge_savings_directory();
  create_savings_directory();
  m_saved = false;
  call_deferred("continue_game");
}

void MainScene::game_over() {
  m_hud->set_can_continue(m_saved);
  m_hud->go_to(HUD::game_over);
  _pause();
}

void MainScene::load_superpowers(const Array& superpowers) const {
  using core_game::to_str;
  m_logger.info("Loading superpowers: " + std::to_string(superpowers.size()));
  for (auto i = 0; i < superpowers.size(); ++i) {
    auto const name = String(superpowers[i]);
    if (m_player && m_world && m_hud) {
      if (m_player->has_node("Superpowers/" + name)) {
        return;
      }
      auto superpower = add_superpower(name, *m_player);
      if (superpower) {
        m_hud->on_player_got_powerup(superpower);
      }
      m_world->remove_powerup(name);
      m_logger.debug(std::string("Loaded superpower ")
                     + to_str(name.get_basename()));
    }
  }
}

void MainScene::create_savings_directory() {
  auto dir = DirAccess::open("user://");
  if (!dir->dir_exists(core_game::SAVINGS_DIRECTORY)) {
    dir->make_dir(core_game::SAVINGS_DIRECTORY);
  }
}

void MainScene::purge_savings_directory() {
  using core_game::SAVINGS_DIRECTORY;
  using core_game::to_str;

  if (!DirAccess::dir_exists_absolute(SAVINGS_DIRECTORY)) {
    std::cerr << "Savings directory " << core_game::SAVINGS_DIRECTORY
              << " not found. Won't delete.\n";
    return;
  }

  auto dir   = DirAccess::open(SAVINGS_DIRECTORY);
  auto files = dir->get_files();
  for (const auto& file : files) {
    auto path  = String{SAVINGS_DIRECTORY} + "/" + file;
    auto error = dir->remove(path);
    if (error) {
      std::cerr << "Cannot remove file " << to_str(path) << '\n';
    }
  }

  auto error = dir->remove(SAVINGS_DIRECTORY);
  if (error) {
    std::cerr << "Removing directory " << SAVINGS_DIRECTORY
              << " failed with error " << error << ".\n";
  } else {
    std::cerr << "Savings directory removed.\n";
  }
}
