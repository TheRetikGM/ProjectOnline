# New project which uses Ren engine.

## Step 1 - Add to project
- Create new directory in [src](../src) directory. For example: `app`
- Link Ren with project (append this to src/CMakeLists.txt)
    ```Meson
    ############# Sandbox ##############
    sandbox_src = []
    # Add sources in app folder
    subdir('app')
    # Add executable linked with Ren. Also add include directories and default compile arguments.
    executable('NewApp', sandbox_src,
      link_args : compile_link_args,
      cpp_args : compile_cpp_args,
      include_directories : ['sandbox/include'],
      dependencies : [ren_dep])
    ```
## Step 2 - Define source files
- Create `meson.build` in the `app` directory
- Add app sources. Example content of **app/CMakeLists.txt**:
    ```Meson
    # Add sources in this directory. Make sure to use the file() function.
    sandbox_src = [sandbox_src, files('AppSrc1.cpp', 'AppSrc2.cpp')]
    # Go into Player directory and do the same as here (execute meson.build file).
    subdir('Player')
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
            PushLayer(new MyLayer("New layer"));
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