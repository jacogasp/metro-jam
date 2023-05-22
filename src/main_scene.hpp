#ifndef COREGAME_MAIN_SCENE_HPP
#define COREGAME_MAIN_SCENE_HPP

#include "logger.hpp"
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;
class MainScene : public Node2D {
  GDCLASS(MainScene, Node2D);

  core_game::Logger m_logger;
  static void _bind_methods();
  MainScene();
};

#endif // COREGAME_MAIN_SCENE_HPP
