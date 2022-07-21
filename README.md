# SDL2 learning
Inspired by http://lazyfoo.net/tutorials/SDL

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

## TODO list
- [ ] Support already installed VCPKG, instead of downloading it
- [x] ~~Layering system + event catching~~
- [ ] Proper log handling.
- [x] ~~Move function calls from REN_ASSERT (as they could get removed with some compilers).~~
- [x] ~~Sandbox~~
- [x] ~~Convert ScriptComponent::script to runtime polymorphic class (instead of entt::poly, which is inflexible for this use case)~~
- [ ] Add support for multiple tags on single TagComponent.
- [x] ~~Scene view shortcut~~
- [ ] Scene serialization and de-serialization
- [x] ~~Remove SDL_net dependency~~
- [x] ~~Camera implementation~~
- [x] ~~Fix KeyInterface (segfault on some keys. For ex. arrow keys).~~
- [ ] Camera controller on entity.
- [x] ~~Proper comments on camera and renderer implementation.~~
- [ ] Tilemap loading
- [x] ~~Freetype text rendering~~
- [ ] Text component
- [ ] Scene hiearchy
- [ ] Basic scene editing (far ahead)
- [ ] Box2D implementation
- [ ] Basic networking
- [x] ~~Centralized rendering~~
- [ ] Use EnTT groups instead of views.
 