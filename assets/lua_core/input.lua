-- TODO: Requires keys.lua

-- Check if given key is pressed or not. Will return true only once
function KeyPressed(key)
    assert(REN_INPUT ~= nil, "Global REN_INPUT is not set. It should be set to pointer to existing instance of Ren::KeyInterface.")
    return API_KeyPressed(REN_INPUT, key)
end

-- Check if given key is being held down or not.
function KeyHeld(key)
    assert(REN_INPUT ~= nil, "Global REN_INPUT is not set. It should be set to pointer to existing instance of Ren::KeyInterface.")
    return API_KeyHeld(REN_INPUT, key)
end
