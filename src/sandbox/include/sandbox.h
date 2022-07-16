#pragma once

// FIXME: This is temporary and should be changed in the future to give the user a chocie to choose between sandbox debug and release.
#ifdef REN_DEBUG
    #define SAND_DEBUG
#endif

#ifdef SAND_DEBUG
    #define SAND_ASSERT(condition, message) \
        do { \
            if (! (condition)) { \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                        << ":" << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)
    #define SAND_STATUS(message) std::cout << "[Sandbox] -- " << message << std::endl
#else
    #define SAND_ASSERT(condition, message) if (!(condition)) std::cout << "[Sandbox-error] -- " << message << std::endl
    #define SAND_STATUS(message) do {} while(false)
#endif