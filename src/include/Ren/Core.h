#pragma once
#include <memory>
#include <iostream>

#ifdef REN_DEBUG
    #define REN_ASSERT_A(condition, message) \
        do { \
            if (! (condition)) { \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                        << ":" << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)
#else
    // FIXME: Print statement, to avoid compiler deleting the condition. That would not be problem, if the code in condition wouldn't need to be executed.
    #define REN_ASSERT(condition, message) if (!(condition)) std::cout << "[Ren-error] -- " << message << std::endl
    #define REN_STATUS(message) do {} while(false)
#endif

template<typename T>
using Ref = std::shared_ptr<T>;