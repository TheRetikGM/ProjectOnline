# ProjectOnline - temp name

Concept project which consists of Engine named Ren and Sandbox app.

![your face after seeing this project](./assets/awesomeface.png "test")

## Docs

- [Conventions](docs/typing.md "conventions")
- [New project](docs/new_project.md "new_project")
- [Custom component](docs/custom_component.md)

## Windows build dependencies.

- Installed visual studio with tools for C++ and C.
- Meson (install using Python's pip) 
  - `pip install meson`
- Git (added to PATH)

## Build - Windows
    git clone https://github.com/theretikgm/sdl2-learning
    go to cloned project
    launch configure.bat
    launch compile.bat
    compiled binary should be in build/src directory

## Build - Linux
    git clone https://github.com/theretikgm/sdl2-learning
    cd sdl2-learning
    meson setup --buildtype=release build
    meson compile -C build

## Windows coding environment setup (Visual Studio)
- It is possible to generate VS project using meson, but it is not suited for development.
- I will have to create project files manually in the future ig.

## Windows coding environment setup (Visual Studio Code)
- Install C++ and meson (from mesonbuild) extensions.
- Clone project
- Open project in VSCode
- Use meson tasks to configure, build and run code.

## Windows/Linux environment setup (CLion)
- Clone project
- run configure.bat or `meson setup build` command
- Open `compile_commands.json` file (the one in project dir) as project using CLion.

## TODO list

- [ ] Review the event system, to support mouse offsetting and such.
- [ ] Camera controller on entity.
- [ ] Tilemap loading
- [ ] Text component
- [ ] Scene hiearchy
- [ ] Basic scene editing (far ahead)
- [ ] Basic networking
- [ ] Use EnTT groups instead of views.
- [ ] Layout components / or library.
- [x] ~~Box2D Collision callbacks~~
- [x] ~~Box2D shape offsetting~~
- [x] ~~RigidBodyComponent support for multiple fixtures (and shapes).~~
- [x] ~~Scene serialization and de-serialization~~
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
