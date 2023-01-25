package.path = LUA_PATH
require("logger")
require("input")
require("keys")
require('ecs')

function C_SetupParam(instance, param_name)
    if (_G[instance][param_name] == nil) then
        _G[instance][param_name] = {}
    end
    setmetatable(_G[instance][param_name], {
        __newindex = function(self, index, value)
            rawset(self, index, value)
            _G[instance].API_RegParam(index)
        end,
    })
end
