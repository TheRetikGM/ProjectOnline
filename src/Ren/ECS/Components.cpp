#include "Ren/ECS/Components.h"
#include "Ren/ECS/NativeScript.h"

namespace Ren
{
    void NativeScriptComponent::Unbind()
    {
        if (script_instance)
            delete script_instance;
    }
};