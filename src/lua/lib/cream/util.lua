-- Utility functions
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local os = os
local assert = assert
local loadstring = loadstring
local ipairs = ipairs
local pairs = pairs
local type = type
local rtable = table
local capi =
{
     util = util
}

module ("cream.util")

table = { }
shell = os.getenv ("SHELL") or "/bin/sh"

function spawn (cmd)
     if cmd and cmd ~= "" then
          return capi.util.spawn (cmd)
     end
end

function spawn_shell (cmd)
     if cmd and cmd ~= "" then
          cmd = shell .. " -c \"" .. cmd .. "\""
          return capi.util.spawn (cmd)
     end
end

function eval (s)
     return assert (loadstring (s))()
end

function quit (...)
     capi.util.quit (...)
end

function table.join (...)
     local ret = { }

     for i, t in ipairs ({...}) do
          if t then
               for k, v in pairs (t) do
                    if type (k) == "number" then
                         rtable.insert (ret, v)
                    else
                         ret[k] = v
                    end
               end
          end
     end

     return ret
end

function table.hasitem (t, item)
     for k, v in pairs (t) do
          if v == item then
               return k
          end
     end
end

function table.keys (t)
     local keys = { }
     for k, _ in pairs (t) do
          rtable.insert (keys, k)
     end
     rtable.sort (keys, function (a, b)
          return type (a) == type (b) and a < b or false
     end)
     return keys
end

function table.keys_filter (t, ...)
     local keys = table.keys (t)
     local keys_filtered = { }

     for _, k in pairs (keys) do
          for _, et in pairs ({...}) do
               if type (t[k]) == et then
                    rtable.insert (keys_filtered, k)
                    break
               end
          end
     end
     return keys_filtered
end

function table.reverse (t)
     local tr = { }

     -- reverse all elements with integer keys
     for _, v in ipairs (t) do
          rtable.insert (tr, 1, v)
     end

     -- add the remaining elements
     for k, v in pairs (t) do
          if type (k) ~= "number" then
               tr[k] = v
          end
     end
     return tr
end

function table.clone (t)
     local c = { }

     for k, v in pairs (t) do
          if type (v) == "table" then
               c[k] = table.clone (v)
          else
               c[k] = v
          end
     end
     return c
end


