require('vector')

-- Create new component wrapper
-- @param component_name Name of the component in Components array defined in C++
-- @param component_def Table containing accept, get and set arrays. See TransformDef below.
local function new_component(component_name, component_def)
    local table = {}
    setmetatable(table, {
        __index = function(_, index)
            if (component_def.accept[index] == nil) then return nil end
            if (component_def.get[index] == nil) then return Components[component_name][index] end
            return component_def.get[index]()
        end,
        __newindex = function(_, index, value)
            if (component_def.accept[index] == nil) then return end
            if (component_def.set[index] == nil) then 
                Components[component_name][index] = value 
                return
            end
            component_def.set[index](value)
        end
    })
    return table
end

--------------------------------
---------- Transform -----------
--------------------------------
local TransformDef = {
    accept = { position = 0, scale = 0, rotation = 0, layer = 0, dirty = 0 },
    get = {
        position = function() return Vec2:new_from(Components.Transform.position) end,
        scale = function() return Vec2:new_from(Components.Transform.scale) end
    },
    set = {
        position = function(value) Components.Transform.position = glmvec2:new(value.x, value.y) end,
        scale = function(value) Components.Transform.scale = glmvec2:new(value.x, value.y) end,
    }
}
Transform = {
    -- Reprezents Transform component on this entity.
    -- Position of the transform component.
    position = Vec2:new(0, 0),
    -- Size of the transform component.
    scale = Vec2:new(0, 0),
    -- Rotation in degreess ccw from positive X.
    rotation = 0,
    -- Rendering layer.
    layer = 0,
    -- Set this to true if this component has a RigidBody component 
    -- and you want to enforce Transform position for this frame.
    dirty = false,
}
-- We use Transform as a wrapper for Component.Transform defined as usertype in C++
Transform = new_component("Transform", TransformDef) -- Above definition is for intellisense only.


--------------------------------
------------ Sprite ------------
--------------------------------
SpriteDef = {
    accept = { color = 0, ppu = 0 },
    get = {
        color = function() return Vec2:new_from(Components.Sprite.color) end,
        ppu = function() return Vec2:new_from(Components.Sprite.ppu) end,
    },
    set = {
        color = function(value) Components.Sprite.color = glmivec2:new(value.x, value.y) end,
        ppu = function(value) 
            value = Vec.floor(value)
            Components.Sprite.ppu = glmivec2:new(value.x, value.y) 
        end,
    }
}
Sprite = {
    color = Vec2:new(0, 0),
    -- Pixels per single unit.
    ppu = Vec2:new(0, 0)
}
Sprite = new_component("Sprite", SpriteDef)
