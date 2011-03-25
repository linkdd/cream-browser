-- Modules
-- @author David Delassus &lt;linkdd@ydb.me&gt;

local setmetatable = setmetatable
local capi =
{
     mod = CreamModule
}

module ("cream.modules")

available =
{
     webkit = { "http", "https" },
     ftp    = { "ftp", "ftps" },
     about  = { "about" }
}

loaded = { }

function enable (name)
     if available[name] and not loaded[name] then
          loaded[name] = capi.mod.new (name, available[name])
          loaded[name]:init ()
     end
end

function disable (name)
     if loaded[name] then
          loaded[name]:unload ()
          loaded[name] = nil
     end
end

function reload (name)
     disable (name)
     enable (name)
end

setmetatable (_M, { __call = function (_, ...) return capi.mod.new (...) end })
