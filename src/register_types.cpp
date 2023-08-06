#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <gdextension_interface.h>

#include "register_types.hpp"

#include "bumblebee.hpp"
#include "chest.hpp"
#include "coin.hpp"
#include "coin_spawner.hpp"
#include "constants.hpp"
#include "gate.hpp"
#include "grenade.hpp"
#include "gun.hpp"
#include "health_bar.hpp"
#include "hud.hpp"
#include "io.hpp"
#include "lifebar.hpp"
#include "main_scene.hpp"
#include "player.hpp"
#include "poison_ranger.hpp"
#include "profiler.hpp"
#include "superpowers.hpp"
#include "world.hpp"
#include "wrench.hpp"

void initialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  core_game::crate_savings_directory(core_game::SAVINGS_DIRECTORY);
  ClassDB::register_class<MainScene>();
  ClassDB::register_class<Player>();
  ClassDB::register_class<Coin>();
  ClassDB::register_class<CoinSpawner>();
  ClassDB::register_class<Chest>();
  ClassDB::register_class<World>();
  ClassDB::register_class<Gate>();
  ClassDB::register_class<LifeBar>();
  ClassDB::register_class<HUD>();
  ClassDB::register_class<BumbleBee>();
  ClassDB::register_class<Wrench>();
  ClassDB::register_class<Grenade>();
  ClassDB::register_class<PoisonRanger>();
  ClassDB::register_class<HealthBar>();
  ClassDB::register_class<Gun>();
  ClassDB::register_class<Immunity>();
  ClassDB::register_class<SlidePower>();
  ClassDB::register_class<AirJumpPower>();

#ifdef CORE_GAME_PROFILING
  core_game::Instrumentor::get().begin_session();
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
example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                     GDExtensionClassLibraryPtr p_library,
                     GDExtensionInitialization* r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                 r_initialization);

  init_obj.register_initializer(initialize_example_module);
  init_obj.register_terminator(uninitialize_example_module);
  init_obj.set_minimum_library_initialization_level(
      MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}