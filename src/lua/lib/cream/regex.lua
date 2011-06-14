-- Regular Expression
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local setmetatable = setmetatable
local capi =
{
     regex = Regex
}

module ("cream.regex")

setmetatable (_M, { __call = function (_, ...) return capi.regex.new (...) end })
