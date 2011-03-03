----------------------------------------------------
-- Utility functions (for table, regex, ...)
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.util")

-- {{{ Clipboard

clipboard = {}

--- Put text into the clipboard.
-- @param txt The text to put.
-- @param primary <code>true</code> to put the text into the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
function clipboard.set (txt, primary)
     if primary == nil then
          primary = true
     end

     capi.clipboard_set (primary, txt)
end

--- Get text from the clipboard
-- @param primary <code>true</code> to get the text from the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
-- @return The text stored in the clipboard.
function clipboard.get (primary)
     if primary == nil then
          primary = true
     end

     return capi.clipboard_get (primary)
end

-- }}}

-- {{{ Cream-Browser API

shell = os.getenv ("SHELL") or "/bin/sh"

--- Spawn a program.
-- @param cmd The command to run.
-- @return The new PID.
function spawn (cmd)
     if cmd and cmd ~= "" then
          return capi.spawn_command (cmd)
     end
end

--- Spawn a program using the shell.
-- @param cmd The command to run.
-- @return The new PID.
function spawn_shell (cmd)
     if cmd and cmd ~= "" then
          cmd = shell .. " -c \"" .. cmd .. "\""
          return capi.spawn_command (cmd)
     end
end

--- Eval Lua code.
-- @param s Lua code to parse.
-- @return The return value of the Lua code..
function eval (s)
     return assert (loadstring (s))()
end

--- Exit Cream-Browser.
function quit ()
     capi.quit ()
end

-- }}}

-- {{{ Tables

table = {}

--- Join all tables given as parameters.
-- This will iterate all tables and insert all their keys into a new table.
-- @param ... A list of tables to join.
-- @return A New table containing all keys from the arguments.
function table.join (...)
     local ret = {}

     for i, t in ipairs ({...}) do
          if t then
               for k, v in pairs (t) do
                    if type(k) == "number" then
                         table.insert (ret, v)
                    else
                         ret[k] = v
                    end
               end
          end
     end
     return ret
end

--- Check if a table has an item and returns its key.
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

--- Get a sorted table with all integer keys from a table.
-- @param t The table for which the keys to get.
-- @return A table with keys.
function table.keys (t)
     local keys = {}

     for k, _ in pairs (t) do
          table.insert (keys, k)
     end

     rtable.sort (keys, function (a, b)
          return type(a) == type(b) and a < b or false
     end)

     return keys
end

--- Filter a tables keys for certain content types.
-- @param t The table to retrieve the keys for.
-- @param ... The types to look for.
-- @return A filtered table with keys.
function table.keys_filter (t, ...)
     local keys = table.keys (t)
     local keys_filtered = {}

     for _, k in pairs (keys) do
          for _, el in pairs ({...}) do
               if type (t[k]) == el then
                    table.insert (keys_filtered, k)
                    break
               end
          end
     end

     return keys_filtered
end

--- Reverse a table.
-- @param t The table to reverse.
-- @return The reversed table.
function table.reverse (t)
     local tr = {}

     for _, v in ipairs (t) do
          table.insert (tr, 1, v)
     end

     for k, v in pairs (t) do
          if type(k) ~= "number" then
               tr[k] = v
          end
     end
     return tr
end

--- Clone a table.
-- @param t The table to clone.
-- @return A clone of t.
function table.clone (t)
     local c = {}

     for k, v in pairs(t) do
          if type(v) == "table" then
               c[k] = table.clone (v)
          else
               c[k] = v
          end
     end

     return c
end

-- }}}

-- {{{ Regex

regex = {}

--- Scans for a match in string for a given pattern.
-- @param reg (Perl-Compatible) Regular Expression pattern.
-- @param s String which will be scanned.
-- @return <code>true</code> if the string matched, <code>false</code> otherwise.
function regex.match (reg, s)
     return capi.regex_match (reg, s)
end

--- Replace a match in a string for a given pattern.
-- @param reg (Perl-Compatible) Regular Expression pattern.
-- @param s String which will be scanned for replacment.
-- @param r String to replace each match with.
-- @return The new string.
function regex.replace (reg, s, r)
     return capi.regex_replace (reg, s, r)
end

-- }}}
