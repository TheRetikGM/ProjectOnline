#pragma once
#include <memory>
#include <iostream>

#ifdef REN_DEBUG
    #define REN_ASSERT(condition, message) \
        do { \
            if (! (condition)) { \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                        << ":" << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)
    #define REN_STATUS(message) std::cout << "[Ren] -- " << message << std::endl
#else
    #define REN_ASSERT(condition, message)
    #define REN_STATUS(message)
#endif

template<typename T>
using Ref = std::shared_ptr<T>;