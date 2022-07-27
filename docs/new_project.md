# New project which uses Ren engine.

## Step 1 - Add to project
- Create new directory in [src](../src) directory. For example: `app`
- Link Ren with project (append this to src/CMakeLists.txt)
    ```CMake
    ########## NEW_APP ##########
    set(APP_NAME "NewAPP")
    add_executable(${APP_NAME} "")
    # Go into 'app' subdir to add sources for executable.
    add_subdirectory(app)
    # Set include path for your app. Ren includes are global, so they are not needed.
    target_include_directories(${APP_NAME} PRIVATE app/include)
    # Link with Ren.
    target_link_libraries(${APP_NAME} PRIVATE REN_LIB)
    ```
## Step 2 - Define source files
- Create `CMakeLists.txt` in the `app` directory
- Add app sources. Example content of **app/CMakeLists.txt**:
    ```CMake
    target_sources(${APP_NAME}
        PRIVATE
            AppSrc1.cpp
            AppSrc2.cpp
    )
    # Go into subdirectory player and additional sources here. This directory must contain CMakeLists.txt, which adds them using target_sources.
    add_subdirectory(Player)
    ```
## Step 3 - Code skeleton
- There is no *main* function.
- All you need to do is to define a `Ren::GameCore* CreateGame()` function which returns raw allocated pointer to your class which extends GameCore.
- I also recommend you to use your own definition of Ren::Layer (see example below).
- Example Code:
    ```C++
    #include <Ren/Ren.h>

    class MyLayer : public Ren::Layer
    {
    public:
        MyLayer(std::string name) : Ren::Layer(name) {}

        void OnInit() override { /* Your implementation */ }
        void OnDestroy() override { /* Your implementation */ }
        void OnUpdate(float delta_time) override { /* Your implementation */ }
        void OnRender(SDL_Renderer* renderer) override { /* Your implementation */ }
    };

    class Game : public Ren::GameCore
    {
    public:
        Game(const Ren::GameDefinition& def) : Ren::GameCore(def) 
        {
            // NOTE: Here could be: Rendering layer, network layer, audio layer etc.
            PushLayer(new DemoLayer("Demo layer"));
        }
    };

    Ren::GameCore* CreateGame()
    {
        Ren::GameDefinition def;
        def.context_def.window_size = { 800, 600 };
        def.context_def.window_name = "New app";
        def.context_def.window_flags |= SDL_WINDOW_RESIZABLE;
        
        return new Game(def);
    }
    ```