# ProjectOnline - temp name
Concept project which consists of Engine named Ren and Sandbox app.

## Docs
- [Conventions](docs/typing.md "conventions")
- [New project](docs/new_project.md "new_project")

## Windows build dependencies.
- Installed visual studio with tools for C++ and C.
- CMake (added to PATH)
- Git (added to PATH)

## Build - Windows
    git clone https://github.com/theretikgm/sdl2-learning
    go to cloned project
    launch configure.bat
    launch compile.bat
    compiled binary should be in build/src/Release directory

## Build - Linux
    git clone https://github.com/theretikgm/sdl2-learning
    cd sdl2-learning
    cmake --preset linux-release
    cmake --build build

## Windows coding environment setup (Visual Studio)
- Install CMake tools using Visual Studio installer.
- Clone project.
- Open project folder in visual studio.
- Make sure that `Windows msvc` preset is selected in the top tool blade.
- Project -> Configure CMake cache
- In the right side of the tool blade select which executable you want to run.
- Use green arrows in the tool blade to build and run (ctrl + F5 or F5).

## Windows coding environment setup (Visual Studio Code)
- Install CMake Tools and C++ tools extensions.
- Clone project
- Open project in VSCode
- Make sure that `Windows msvc` preset is selected in the bottom tool blade (blue strip with buttons).
- Use arrow or bug in the bottom tool blade to compile and run code.
- Note: Select debug or release build configuration in bottom blade.


## TODO list
- [ ] Support already installed VCPKG, instead of downloading it
- [ ] Fix the VCPKG sdl-mixer mess. The add_library directive changes based on weather and OS... what?
- [ ] Scene serialization and de-serialization
- [ ] Camera controller on entity.
- [ ] Tilemap loading
- [ ] Text component
- [ ] Scene hiearchy
- [ ] Basic scene editing (far ahead)
- [ ] Basic networking
- [ ] Use EnTT groups instead of views.
- [ ] Layout components / or library.
- [x] ~~Layering system + event catching~~
- [x] ~~Proper log handling.~~
- [x] ~~Move function calls from REN_ASSERT (as they could get removed with some compilers).~~
- [x] ~~Sandbox~~
- [x] ~~Convert ScriptComponent::script to runtime polymorphic class (instead of entt::poly, which is inflexible for this use case)~~
- [x] ~~Add support for multiple tags on single TagComponent.~~
- [x] ~~Scene view shortcut~~
- [x] ~~Remove SDL_net dependency~~
- [x] ~~Camera implementation~~
- [x] ~~Fix KeyInterface (segfault on some keys. For ex. arrow keys).~~
- [x] ~~Proper comments on camera and renderer implementation.~~
- [x] ~~Freetype text rendering~~
- [x] ~~Box2D implementation~~ kind of
- [x] ~~Centralized rendering~~
- [x] ~~Move GameCore.cpp, Layer.cpp, LayerStack.cpp into their corresponding subdirectories~~
 