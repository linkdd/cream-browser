-- Theme
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local setmetatable = setmetatable
local capi =
{
     theme = theme
}

module ("cream.theme")

setmetatable (_M, { __call = function (_, ...) return capi.theme.new (...) end })
