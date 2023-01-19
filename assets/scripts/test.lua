package.path = LUA_PATH
local foo = require("foo")

function Test:OnInit()
    print("[LUA] OnInit(): Hello!")
    foo.hello()
end

function Test:OnDestroy()
    print("[LUA] OnDestroy(): Bye!")
end

function Test:OnUpdate(dt)
    if (KeyHeld(KEY_A)) then
        print("Go left!")
    end
    if (KeyHeld(KEY_D)) then
        print("Go right!")
    end
    if (KeyHeld(KEY_W)) then
        print("Go UP!")
    end
    if (KeyHeld(KEY_S)) then
        print("Go Down!")
    end
end
