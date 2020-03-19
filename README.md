# LM Engine
Made with [EnTT](https://github.com/skypjack/entt/) and 
[Bullet Physics](https://github.com/bulletphysics/bullet3).

Currently builds on Linux and Windows, though improvements are needed on the 
Windows side to streamline setting up the project.

## Modules

### lmpl
Platform/windowing system abstraction library.

### lmgl
Graphics Library implemented with Vulkan.

### lmlib
C++ utility library.

### lmtk
GUI app toolkit built on top of lmpl and lmgl.

### lmengine
Core game logic and simulation library.

### lmeditor
Game asset creation application.

### lmhuv
Presentation layer library. Separates core game logic from graphics/sound.

### sample
Example first/third person game made with the engine.

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

You may now run a regular CMake config/build in the build directory. See 
documentation of CMake and Conan for using different configurations/settings.

### Windows
Currently compiles only with the MinGW-w64 toolset, version 7.

Windows requires DLLs to be copied into the build directory, namely `tbb.dll` 
and `OpenMeshCore.dll` at the time of writing. This is to be streamlined.

## Running the Editor
Run the `Editor` executable in the build directory with the `--project-dir` 
argument set to the `sample` directory, or another directory containing a 
`game` shared library like that built by the `sample_editor_plugin` CMake 
target.
