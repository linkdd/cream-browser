--- Register commands
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local capi =
{
     commands = { register = command_register, modify = command_modify, set_func = command_set_func }
}

module ("cream.commands")

function register (command, nargs, return, func)
     capi.commands.register (command, nargs, return)
     if func ~= nil then
          capi.commands.set_func (command, func)
     end
end

function modify (command, binding, func)
     capi.commands.modify (command, binding, func)
end

-- default callbacks
local function set (var, value)
     local f = assert (loadstring (var .. " = " .. value))
     f()
end

local function get (var)
     local f = assert (loadstring ("return " .. var))
     return f()
end

-- Register default commands
register ("set", 2, false, set)
register ("get", 1, true, get)

