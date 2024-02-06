/**
 * @file Ren/Core/Startup.hpp
 * @brief Entry point for execution.
 */

#pragma once
#include "Ren/Core/GameCore.hpp"

extern Ren::GameCore* CreateGame();

int main(int argc, char* argv[]);
