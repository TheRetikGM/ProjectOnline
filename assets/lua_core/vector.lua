-- Author: TheRetikGM
--
-- Implements vector math. 
-- Currently support vectors with 2 - 4 components. This could be extended by 
-- editing 'comps' and 'n_comps' variables.
-- Provides:
--     VecN - Vector with N components
--     VecGen - Generic vector with up to n_comps components

local comps = { "x", "y", "z", "w" }
local n_comps = 4

-- Contains generic operations between generic vectors
-- FIXME: Handle zero length vectors. Rigth now there is undefined behaiviour.
Vec = {
    -- Dot product between two vectors.
    dot = function(a, b)
        assert(a.size == b.size, "Dot product must be between two vectors of the same number of components.")

        local res = 0
        for i = 1, n_comps do
            if (a[comps[i]] == nil) then return res end
            res = res + a[comps[i]] * b[comps[i]]
        end
        return res
    end,
    -- Length of the vector.
    length = function(v)
        return math.sqrt(Vec.dot(v, v))
    end,
    -- Convert vector to (x, y, ...) string.
    string = function(v)
        local res = "("
        for i = 1, v.size do
            res = res .. v[comps[i]]
            if (i ~= v.size) then res = res .. ", " end
        end
        res = res .. ")"
        return res
    end,
    -- Angle between two vectors in range 0..PI
    angle = function(a, b)
        assert(a.size == b.size, "Vectors must have the same number of components")
        return math.acos(Vec.dot(a, b) / (Vec.length(a) * Vec.length(b)))
    end,
    -- Floor all components
    floor = function(a)
        local res = a
        for i = 1, a.size do res[comps[i]] = math.floor(a[comps[i]]) end
        return res
    end
}

-- Metatable describing VecGen operators
VecGenMeta = {
    __add = function(a, b)
        if (a.size == nil) then a = VecGen:new(a) end
        if (b.size == nil) then b = VecGen:new(b) end
        assert(a.size == b.size, "Vectors must be the same size")
        local res = {}
        for i = 1, a.size do res[i] = a[comps[i]] + b[comps[i]] end
        return VecGen:new(res)
    end,
    __sub = function(a, b)
        if (a.size == nil) then a = VecGen:new(a) end
        if (b.size == nil) then b = VecGen:new(b) end
        assert(a.size == b.size, "Vectors must be the same size")
        local res = {}
        for i = 1, a.size do res[i] = a[comps[i]] - b[comps[i]] end
        return VecGen:new(res)
    end,
    __mul = function(a, b)
        local res = {}
        if (type(a) == "table" and type(b) == "number") then
            for i = 1, a.size do res[i] = a[comps[i]] * b end
        elseif (type(a) == "number" and type(b) == "table") then
            for i = 1, b.size do res[i] = b[comps[i]] * a end
        else
            if (a.size == nil) then a = VecGen:new(a) end
            if (b.size == nil) then b = VecGen:new(b) end
            if (a.size == b.size) then
                for i = 1, a.size do res[i] = a[comps[i]] * b[comps[i]] end
            end
        end
        return VecGen:new(res)
    end,
    __div = function(a, b)
        local res = {}
        if (type(a) == "table" and type(b) == "number") then
            for i = 1, a.size do res[i] = a[comps[i]] / b end
        elseif (type(a) == "number" and type(b) == "table") then
            for i = 1, b.size do res[i] = b[comps[i]] / a end
        else
            if (a.size == nil) then a = VecGen:new(a) end
            if (b.size == nil) then b = VecGen:new(b) end
            if (a.size == b.size) then
                for i = 1, a.size do
                    if (b[comps[i]] == 0) then return nil end
                    res[i] = a[comps[i]] / b[comps[i]]
                end
            end
        end
        return VecGen:new(res)
    end,
    __eq = function(a, b)
        if (a.size == nil) then a = VecGen:new(a) end
        if (b.size == nil) then b = VecGen:new(b) end

        if (a.size == b.size) then
            for i = 1, a.size do
                if (a[comps[i]] ~= b[comps[i]]) then
                    return false
                end
            end
            return true
        end
        return false
    end,
    __unm = function(a)
        local res = {}
        for i = 1, a.size do res[i] = -a[comps[i]] end
        return VecGen:new(res)
    end,
    __len = Vec.length,
    __concat = function(a, b)
        if (type(a) == "string" and type(b) == "table") then return a .. Vec.string(b) end
        if (type(a) == "table" and type(b) == "string") then return Vec.string(a) .. b end
        if (type(a) == "table" and type(b) == "table") then return Vec.string(a) .. ", " .. Vec.string(b) end
        if (type(a) == "string" and type(b) == "string") then return a .. b end
        return nil
    end,
}


----------------------------------
-- Specific vector definitions ---
----------------------------------

-- Generic vector
VecGen = { size = 0 }
function VecGen:new(components)
    local res = { size = 0 }

    -- Create x, y, ... components based on the contents of components table.
    for i = 1, n_comps do
        if (components[i] ~= nil) then
            res.size = res.size + 1
            res[comps[i]] = components[i]
        else break end
    end

    setmetatable(res, VecGenMeta)
    res.__index = self
    return res
end

Vec2 = {}
function Vec2:new(x, y)
    local res = { size = 2, x = x, y = y }
    setmetatable(res, VecGenMeta)
    res.__index = self
    return res
end
function Vec2:new_from(glmvec2)
    return Vec2:new(glmvec2.x, glmvec2.y)
end

Vec3 = {}
function Vec3:new(x, y, z)
    local res = { size = 3, x = x, y = y, z = z }
    setmetatable(res, VecGenMeta)
    res.__index = self
    return res
end
function Vec3:new_from(glmvec3)
    return Vec3:new(glmvec3.x, glmvec3.y, glmvec3.z)
end

Vec4 = {}
function Vec4:new(x, y, z, w)
    local res = { size = 4, x = x, y = y, z = z, w = w }
    setmetatable(res, VecGenMeta)
    res.__index = self
    return res
end
function Vec4:new_from(glmvec4)
    return Vec4:new(glmvec4.x, glmvec4.y, glmvec4.z, glmvec4.w)
end

return Vec
