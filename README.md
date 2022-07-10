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

## TODO
    - [] Support already installed VCPKG, instead of downloading it
    - [] Better access to resources

 