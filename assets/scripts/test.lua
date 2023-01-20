require("vector")

local v1 = Vec3:new(1, 2, 3)
local v2 = Vec3:new(4, 5, 6)

function Test:OnInit()
    print("[LUA] OnInit(): Hello!")
    print("[LUA] v1 = " .. v1 .. ", v2 = " .. v2)
    print("[LUA] v1 + v2 = " .. (v1 + v2))
    print("[LUA] v1 - v2 = " .. (v1 - v2))
    print("[LUA] v1 * v2 = " .. (v1 * v2))
    print("[LUA] v1 / v2 = " .. (v1 / v2))
    print("[LUA] v1 dot v2 = " .. Vec.dot(v1, v2))
    print("[LUA] -v1 = " .. (-v1))
    print("[LUA] ||v1|| = " .. (#v1))
end

function Test:OnDestroy()
    print("[LUA] OnDestroy(): Bye!")
end

function Test:OnUpdate(dt)
    if (KeyHeld(KEY_A)) then
        print("[LUA] Go left!")
    end
    if (KeyHeld(KEY_D)) then
        print("[LUA] Go right!")
    end
    if (KeyHeld(KEY_W)) then
        print("[LUA] Go up!")
    end
    if (KeyHeld(KEY_S)) then
        print("[LUA] Go down!")
    end
end

