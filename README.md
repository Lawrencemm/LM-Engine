# LM Engine
Made with [EnTT](https://github.com/skypjack/entt/) and 
[Bullet Physics](https://github.com/bulletphysics/bullet3).

Currently builds on Linux and Windows.

## Modules

### lmpl
Platform/windowing system abstraction library.

### lmgl
Graphics Library implemented with Vulkan.

### lmlib
C++ utility library.

### lmtk
GUI app toolkit built on top of lmpl and lmgl.

### lmng
Core game logic and simulation library.

### lmeditor
Game asset creation application.

### lmhuv
Presentation layer library. Separates core game logic from graphics/sound.

### sample
Example 3D game made with the engine.

## Requirements
* Python 3
* The [Conan](https://conan.io/) package manager
    * You may need to run 
    `conan profile update settings.compiler.libcxx=libstdc++11 default`
    to set the standard library Conan uses by default.
* [CMake](https://cmake.org/)
* Vulkan graphics drivers
* Linux
    * Redhat/Fedora
        * libwayland-dev
        * libx11-xcb-dev
        * libxcb-randr0-dev
        * libxcb-util0-dev
        * pkg-config
        * libgtkmm-3.0-dev
        * vulkan-validation-layers

## Setup
On the command line, in the repository root directory, run:
* `python init.py`
* `conan config install conan/config/common`
    * This may override your Conan config. If you need to keep your config,
    set the environment variable CONAN_USER_HOME to a directory of your 
    choice while running these commands.
* `mkdir build && conan workspace install . -if build --build missing`
    * Windows: add the arguments `-e WORKSPACE_DIR=../../../build` to copy 
    required DLLs to the build directory. 

You may now run a regular CMake config/build in the build directory. See 
documentation of CMake and Conan for using different configurations/settings.

## Running the Editor
Run the `Editor` executable in the build directory with the `--project-dir` 
argument set to the `sample` project directory eg. `./Editor --project-dir ../sample`.
