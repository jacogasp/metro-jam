#include "world.hpp"
#include "gate.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "profiler.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>

std::array<World::NextSceneMessage, 16> World::m_pending{};
int World::m_pending_index{0};

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
  PROFILE_FUNCTION()
  m_logger = core_game::LoggerLocator::getService();
  m_player = get_node<Player>(m_player_node_path);
  update_scene();
  m_logger->info("World ready!");
}

void World::_process(float) {
  PROFILE_FUNCTION()
  for (int i = 0; i < m_pending_index; ++i) {
    auto const loader     = ResourceLoader::get_singleton();
    auto const next_scene = m_pending.at(i).next_scene;
    m_logger->info("Load new scene " + next_scene);
    set_packed_scene(loader->load(next_scene.c_str()));
    auto const new_gate_name = m_pending.at(i).gate_name;
    update_scene();
    auto const new_gate =
        m_current_scene->get_node<Gate>(new_gate_name.c_str());
    if (new_gate) {
      new_gate->set_closed(true);
      auto player              = cast_to<Player>(m_player);
      auto const player_ground = player->get_ground_position();
      auto const gate_position = new_gate->get_position();
      auto const new_x         = gate_position.x;
      auto const new_y         = gate_position.y - player_ground;
      m_player->set("position", Vector2{new_x, new_y});
    }
  }
  m_pending_index = 0;
}

void World::update_scene() {
  PROFILE_FUNCTION()

  if (m_packed_scene.is_null()) {
    m_logger->error("Packed scene is null");
    return;
  }
  auto old_scene  = m_current_scene;
  m_current_scene = m_packed_scene->instantiate();
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
