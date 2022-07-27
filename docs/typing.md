# Naming and other conventions
Here you can find conventions used in this project.

## Contents
- [Structure](#structure)
    - [Files](#files)
- [Code](#code)
    - [Header files and includes](#header-files-and-includes)
    - [Free functions](#free-functions)
    - [Variables and macros](#variables-and-macros)
    - [Classes](#classes)
    - [Structs](#structs)
    - [Templates](#templates-skull)
    - [Types](#types)

## Structure
### Files
- Header files have extensions `.h`
- Header files with definitions have extensions `.hpp`
    - No need to create source file when the definition is small.
- Source files have extensions `.cpp`
- Put header files into `include` directory into its coresponding folder.
    - For example: src file `Ren/Core/GameCore.cpp` should have header file in `include/Ren/Core/GameCore.h`
- Use **Camel case** for naming files.
- Special use case is when use have multiple source files for single header file. In such case use Camel case for the header file name followed by uderscore and descriptive name of implementation.
    - For example: header `GameCore.h` source files `GameCore_impl1.cpp` and `GameCore_impl2.cpp`
- When creating new source file don't forget to put it into CMakeLists.txt for compilation
    - For example for sources in directory `foo` then `foo/CMakeLists.txt` should contain: 
        ```CMake
        target_sources(REN_LIB  # For sandbox it would be ${PROJECT_NAME} or whatever we choose
            PRIVATE
                src1.cpp
                src2.cpp
        )
        ```
    - To add the foo directory use `add_subdirectory(foo)` in the CMakeLists.txt which is on the same directory level as `foo`.

## Code
### Header files and includes
- For external includes use `#include <file>` syntax.
- For internal includes use `#include "file"` synytax.
- Put `#pragma once` above every include in **header files**
- Never include `.cpp` files
- Never use `using namespace` in header files.

### Free functions
- eg. Functions not inside class or struct.
- **Snake-case**
- Return type before name
- Template stuff on line above.
    ```C++
    void do_something_special(int var1, int var2);

    template<typename T>
    T generic_add(T a, T b)
    {
        return a + b;   
    }
    ```
### Variables and macros
- Local variables use **snake-case**
- Prefixes
    - global variables (non constants) - `g_`
    - static variables - `s_`
    - class member variables - `m_`
    - static member variables - `s_`
- Constant variables always **capital snake-case**
```C++
// Macro -- always with capital letters
#define USEFUL_MACRO(a) foo(a + 2)

// Constants -- capital snake-case
const float MY_PI = 3.141592f;

// Global variable (non constant) -- prefix with g_
float g_global_var = 1.23f;

void foo()
{
    // Static variable
    static int s_foo_bar = 0;

    // Local variable -- snake-case
    int file_count = 0; 
}
```

### Classes
- Use **cames-case** for name.
- Member variables
    - Prefix `m_` or `s_` for static member variables
    - Public --> first letter upper case (after prefix)
    - Private --> first letter lower case (after prefix)
- Methods
    - **Camel-case**
    - Public --> first letter upper case
    - Private --> first letter lower case
- Static methods --> same as normal methods
- If feasable use `inline static` keyword to initialize static variables inside class (so that you don't have to put it in source file)
- `public`, `private` and `protected` keywords are at the same tab level as class name
```C++
namespace Ren
{
    class Counter
    {
    public:
        // Public member variable.
        std::string m_Name = "default";

        Counter(std::string name, int first_count_value)
            : m_Name(name)
            , m_currentCount(first_count_value)
        {
            s_instanceCount++;
            fooBar();
        }

        // Public methods.
        int GetCount() { return m_currentCount; };
        void UpdateCount() 
        {
            if (m_currentCount < MAX_COUNT)
                m_currentCount++;
        }

    private:
        inline static int s_instanceCount = 0;
        inline const static int MAX_COUNT = 100;
        int m_currentCount = 0;

        // Private method.
        void fooBar() {}
    };
}
```
### Structs
- In C++ the **ONLY** difference between *struct* and *class* is that by default all members of struct are public and members of class are private. However we differenciate between the two:
    - Use struct when the use case is `hold data in one structure` or `mostly member variables and not many methods`
    - Use class when the use case is `methods encapsulating some funcionality, which can act on some member variables`
    - For example don't use class for something like 2D vector (without defined operations):
        ```C++
            // Use
            struct Vec2 { float x, y; };
            
            // And not
            class Vec2 
            {
            public:
                float m_X;
                float m_Y;
            };
        ```
    - This helps to differenciate between use cases.
- Use `local variables` naming for member variables. That is because we think of struct as just type holding more types and not class.
- For methods use **camel-case** just as with classes.

### Templates :skull:
- Put them **above** functions, methods, variables and other statements like this.
- Template arguments use **camel-case**
- If the use case is simple:
    - Use `T` as a template argument name. 
    - Use `Args...` for unspecified number of arguments.
- Otherwise:
    - Prefix template arguments with `T` and follow by human readable name.
    - For example: `template<typename TComponent>`

### Types
- When it makes sense, use typedefs from `#include <cstdint>` header.
- For example: 
    - Use `uint32_t` instead of `unsigned int`
    - Use `uint8_t` instead of `unsigned char` for RGBA image with 32 bits depth.
- Somethimes typedefs improve readability and managements. For example:
    ```C++
    typedef uint32_t EntityID;

    void assign_name(EntityID ent, std::string ent);
    ```
- Use `std::string` and not `const char*` when storing some strings.
