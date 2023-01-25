#include <Ren/Ren.h>
#include "EditorLayer.hpp"

class Editor : public Ren::GameCore
{
public:
    Editor(const Ren::GameDefinition& def) : Ren::GameCore(def)
    {
        Ren::LogEmmiter::AddListener<Ren::StreamLogger>({ stdout });

        PushLayer(CreateRef<EditorLayer>("EditorLayer"));
    }
};


Ren::GameCore* CreateGame()
{
    Ren::GameDefinition def;
    def.context_def.window_name = "Ren scene editor";
    def.context_def.window_size = { 1600, 900 };
    return new Editor(def);
}
