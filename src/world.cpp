#include "world.hpp"
#include "gate.hpp"
#include "io.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

std::array<World::NextSceneMessage, 16> World::m_pending{};
size_t World::m_pending_index{0};

void World::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_packed_scene"), &World::set_packed_scene);
  ClassDB::bind_method(D_METHOD("get_packed_scene"), &World::get_packed_scene);
  ADD_PROPERTY(PropertyInfo(godot::Variant::OBJECT, "entry_room"),
               "set_packed_scene", "get_packed_scene");

  ClassDB::bind_method(D_METHOD("set_player"), &World::set_player);
  ClassDB::bind_method(D_METHOD("get_player"), &World::get_player);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "player"), "set_player",
               "get_player");
}

void World::_ready() {
  PROFILE_FUNCTION();
  m_logger = core_game::LoggerLocator::getService();
  m_player = get_node<Player>(m_player_node_path);
  if (m_current_scene == nullptr) {
    update_scene("");
  }
  m_logger->info("World ready!");
}

void World::_process(double) {
  PROFILE_FUNCTION();
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  for (size_t i = 0; i < m_pending_index; ++i) {
    auto const loader       = ResourceLoader::get_singleton();
    String const next_scene = String(m_pending.at(i).next_scene.c_str());
    m_logger->info(std::string{"Load new scene "}
                   + core_game::to_str(next_scene));
    set_packed_scene(loader->load(next_scene));
    auto const new_gate_name = m_pending.at(i).gate_name;
    auto scene_name          = next_scene.get_basename().get_file();
    update_scene(scene_name);
    auto const new_gate =
        m_current_scene->get_node<Gate>(new_gate_name.c_str());
    if (new_gate) {
      new_gate->set_closed(true);
      auto player              = get_node<Player>("/root/Main/Player");
      auto const player_ground = player->get_ground_position();
      auto const gate_position = new_gate->get_position();
      auto const new_x         = gate_position.x;
      auto const new_y         = gate_position.y - player_ground;
      player->set_position(Vector2{new_x, new_y});
      player->set_direction(new_gate->get_exit_direction() == Gate::left
                                ? Player::left
                                : Player::right);
    }
  }
  m_pending_index = 0;
}

static void filter_superpowers(Player* player, Node* scene) {
  auto superpowers_node = player->get_node<Node2D>("Superpowers");
  if (superpowers_node == nullptr) {
    return;
  }
  auto superpowers = superpowers_node->get_children();
  for (auto i = 0; i < superpowers.size(); ++i) {
    auto const superpower = Node2D::cast_to<Node2D>(superpowers[i]);
    auto const name       = superpower->get_name().get_basename();
    if (scene->has_node(name)) {
      auto node = scene->get_node<Node2D>(name);
      node->queue_free();
    }
  }
}

void World::update_scene(String const& scene_name) {
  PROFILE_FUNCTION();

  if (m_packed_scene.is_null()) {
    m_logger->error("Packed scene is null");
    return;
  }
  auto old_scene  = m_current_scene;
  m_current_scene = m_packed_scene->instantiate();
  if (!scene_name.is_empty()) {
    m_current_scene->set_name(scene_name);
  }
  if (has_node("/root/Main/Player")) {
    auto player = get_node<Player>("/root/Main/Player");
    filter_superpowers(player, m_current_scene);
  }
  // If a node with the same path is present remove the old one
  if (!scene_name.is_empty() && has_node(scene_name)) {
    auto node = get_node<Node>(scene_name);
    remove_child(node);
  }
  add_child(m_current_scene);
  if (old_scene) {
    old_scene->call_deferred("free");
  }
}

void World::go_to_scene(const NextSceneMessage& next_scene_message) {
  m_pending.at(m_pending_index) = next_scene_message;
  m_pending_index++;
}

// Getters and setters

Ref<PackedScene> World::get_packed_scene() {
  return m_packed_scene;
}

void World::set_packed_scene(const Ref<PackedScene>& packed_scene) {
  m_packed_scene = packed_scene;
}

void World::set_player(const NodePath& path) {
  m_player_node_path = path;
}

NodePath World::get_player() const {
  return m_player_node_path;
}

void World::remove_powerup(const String& name) const {
  using core_game::to_str;
  if (m_current_scene && !m_current_scene->has_node(name)) {
    m_logger->debug(std::string("World Superpower ") + name.utf8().get_data()
                    + " not found");
    return;
  }
  auto powerup = m_current_scene->get_node<Node2D>(name);
  if (powerup) {
    powerup->queue_free();
  } else {
    m_logger->debug(std::string{"Superpower "} + to_str(name) + " is null");
  }
}

String World::get_current_scene_file_path() {
  return m_current_scene ? m_current_scene->get_scene_file_path() : String();
}

void World::load_scene_from_path(const String& filepath) {
  auto const loader = ResourceLoader::get_singleton();
  set_packed_scene(loader->load(filepath));
  auto scene_name = filepath.get_basename().get_file();
  update_scene(scene_name);
  using core_game::to_str;
  m_logger->info(std::string{"Loaded scene from "}
                 + filepath.utf8().get_data());
}
