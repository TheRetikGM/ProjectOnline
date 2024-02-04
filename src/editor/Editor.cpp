#include <Ren/Ren.h>
#include "EditorLayer.hpp"

class Editor : public Ren::GameCore {
public:
    Editor(const Ren::GameDefinition& def) : Ren::GameCore(def) {
        ren_utils::LogEmitter::AddListener<ren_utils::StreamLogger>({ stdout });

        PushLayer(CreateRef<EditorLayer>("EditorLayer"));
    }
};


Ren::GameCore* CreateGame() {
    Ren::GameDefinition def;
    def.context_def.window_name = "Ren scene editor";
    def.context_def.window_size = { 1200, 700 };
    def.context_def.window_flags |= SDL_WINDOW_RESIZABLE;
    return new Editor(def);
}
