# Gogot GDExtension Template

This project provides a ready to use Godot game template using GDExtension and
C++.

## Building with CMake

To build this project, use one of the following CMakePresets:

```shell
  "debug"
  "release"
  "macos"
  "linux"
  "windows"
  "ci-macos"
  "ci-linux"
  "ci-windows"
  ```

  for example, for configuring the project:

  ```shell
  cmake --preset macos
  ```

  and to build:

  ```shell
  cmake --build --preset macos
  ```

## Git Hooks

This project is run a GitHub CI/CD pipeline which builds the game sorce code as
shared libraries for each OS (`.so` for Linux, `.dylib` for macOS and `.dll`)
and store them as artifacts.

The `.githooks` directory provides scripts to automatically download the
artifacts to your local `build` directory, in order to run the game.

To automatically download the artifacts at each pull, link the `.githooks` to
your `.git/hooks` folder with

```shell
ln -s $(pwd)/.githooks/* $(pwd)/.git/hooks
```

You need Python installed.

## Manually download artifacts

To manually download artifacts, run

```shell
python download_artifacts.py <user>/<repo-name>
```
