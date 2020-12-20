# LM Engine
Made with [EnTT](https://github.com/skypjack/entt/) and 
[Bullet Physics](https://github.com/bulletphysics/bullet3).

Currently builds on Linux and Windows.

This is _very_ work-in-progress. It exists now primarily as an example of how to
construct an engine with the stated design goals.

## Design Goals
### Keyboard Centric
Every action the editor can perform is accessible through a keyboard shortcut.
Pressing F1 shows the list of currently accessible actions and their shortcuts.
Users should not need to take their hands off the keyboard.

### Plain Text Assets
File formats for assets like maps and archetypes (prefabs) are human-readable
with minimal noise. There are no GUIDs that change with every save of the asset. 
There is only plain, meaningful data.

This makes assets first-class citizens in source code management tools like git.
They can be merged and have conflicts resolved like source code.

### All Changes Saved
Instead of the user deciding when to save an asset, LM-Engine saves all changes
as they are made. The user is expected to utilise version control to protect data 
they don't want overwritten.

### Data Driven
The core API for game logic is based on entity-component-system. 
Mutation of game state is done through plain-old-data instead of interface methods
and classes.

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
* `mkdir build && conan install . -if build --build missing`

You may now run a regular CMake config/build in the build directory. See 
documentation of CMake and Conan for using different configurations/settings.

## Running the Editor
Run the `Editor` executable in the build directory with the `--project-dir` 
argument set to the `sample` project directory eg. `./Editor --project-dir ../sample`.
