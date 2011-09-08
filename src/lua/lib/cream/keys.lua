-- Cream-Browser key bindings API
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local unpack = unpack
local table = table
local capi =
{
     keys = keys,
     bit = bit,
}

local modkeys =
{
     Shift   = 0x00000001,
     Lock    = 0x00000002,
     Control = 0x00000004,
     Mod1    = 0x00000008,
     Mod2    = 0x00000010,
     Mod4    = 0x00000020,
     Mod5    = 0x00000040,
     Button1 = 0x00000080,
     Button2 = 0x00000100,
     Button3 = 0x00000200,
     Button4 = 0x00000400,
     Button5 = 0x00000800,

     Super   = 0x02000000,
     Hyper   = 0x04000000,
     Meta    = 0x08000000,

     Mask    = 0x5C001FFF
}

module ("cream.keys")

function map (states, modifiers, cmd, callback)
     statemask = #states == 1 and states[1] or capi.bit.bor (unpack (states))
     modmask = 0

     if #modifiers > 1 then
          m = {}

          for i = 1, #modifiers do
               table.insert (m, modkeys[modifiers[i]])
          end

          modmask = capi.bit.bor (unpack (m))
     elseif #modifiers == 1 then
          modmask = modkeys[modifiers[1]]
     end

     capi.keys.add (statemask, modmask, cmd, callback)
end

