#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <gdextension_interface.h>

#include "chest.hpp"
#include "coin.hpp"
#include "coin_spawner.hpp"
#include "constants.hpp"
#include "gate.hpp"
#include "io.hpp"
#include "main_scene.hpp"
#include "player.hpp"
#include "profiler.hpp"
#include "register_types.hpp"
#include "weapon.hpp"
#include "world.hpp"

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  core_game::crate_savings_directory(core_game::SAVINGS_DIRECTORY);
  ClassDB::register_class<MainScene>();
  ClassDB::register_class<Player>();
  ClassDB::register_class<Weapon>();
  ClassDB::register_class<Coin>();
  ClassDB::register_class<CoinSpawner>();
  ClassDB::register_class<Chest>();
  ClassDB::register_class<World>();
  ClassDB::register_class<Gate>();

#ifdef CORE_GAME_PROFILING
  core_game::Instrumentor::get().begin_session("Example session");
#endif
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
#ifdef CORE_GAME_PROFILING
  core_game::Instrumentor::get().end_session();
#endif
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT
example_library_init(const GDExtensionInterface* p_interface,
                     const GDExtensionClassLibraryPtr p_library,
                     GDExtensionInitialization* r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library,
                                                 r_initialization);

  init_obj.register_initializer(initialize_example_module);
  init_obj.register_terminator(uninitialize_example_module);
  init_obj.set_minimum_library_initialization_level(
      MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}