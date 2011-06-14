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

--- Spawn a program.
-- @param cmd The command to run.
-- @return The new PID.
function spawn (cmd)
     if cmd and cmd ~= "" then
          return capi.util.spawn (cmd)
     end
end

-- Spawn a program using the shell.
-- @param cmd The command to run.
-- @return The new PID.
function spawn_shell (cmd)
     if cmd and cmd ~= "" then
          cmd = shell .. " -c \"" .. cmd .. "\""
          return capi.util.spawn (cmd)
     end
end

--- Eval Lua code.
-- @param s Lua code to parse
-- @return The return value of the Lua code.
function eval (s)
     return assert (loadstring (s))()
end

--- Exit Cream-Browser
-- @param ... Exit Code (if needed)
function quit (...)
     capi.util.quit (...)
end

--- Join all tables given as parameters.
-- This will iterate all tables and insert all their keys into a new table.
-- @param args A list of tables to join
-- @return A new table containing all keys from the arguments.
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

--- Check if a table has an item and return its key.
-- @param t The table.
-- @param item The item to look for in values of the table.
-- @return The key were the item is found, or nil if not found.
function table.hasitem (t, item)
     for k, v in pairs (t) do
          if v == item then
               return k
          end
     end
end

--- Get a sorted table with all integer keys from a table
-- @param t the table for which the keys to get
-- @return A table with keys
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

--- Filter a tables keys for certain content types
-- @param t The table to retrieve the keys for
-- @param ... the types to look for
-- @return A filtered table with keys
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

--- Reverse a table
-- @param t the table to reverse
-- @return the reversed table
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

--- Clone a table
-- @param t the table to clone
-- @return a clone of t
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


