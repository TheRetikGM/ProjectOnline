#include <algorithm>

#include "Ren/ECS/Components.h"
#include "Ren/ECS/NativeScript.h"
#include "Ren/Utils/Logger.hpp"

namespace Ren
{
    void NativeScriptComponent::Unbind()
    {
        if (script_instance)
            delete script_instance;
    }
};