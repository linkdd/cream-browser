--- Utility functions
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.util")

--- Spawn a program
-- @param cmd The command to run
-- @return The new PID
-- @class function
-- @name spawn

--- Spawn a program using the shell
-- @param cmd The command to run.
-- @return The new PID
-- @class function
-- @name spawn_shell

--- Eval Lua code
-- @param s Lua code to parse
-- @return The return value of the Lua code.
-- @class function
-- @name eval

--- Exit Cream-Browser
-- @param ... Exit Code (if needed)
-- @class function
-- @name quit

--- Join all tables given as parameters
-- This will iterate all tables and insert all their keys into a new table
-- @param args A list of tables to join
-- @return A new table containing all keys from the arguments.
-- @class function
-- @name table.join

--- Check if a table has an item and return its key
-- @param t The table
-- @param item The item to look for in values of the table
-- @return The key were the item is found, or nil if not found
-- @class function
-- @name table.hasitem

--- Get a sorted table with all integer keys from a table
-- @param t the table for which the keys to get
-- @return A table with keys
-- @class function
-- @name table.keys

--- Filter a tables keys for certain content types
-- @param t The table to retrieve the keys for
-- @param ... the types to look for
-- @return A filtered table with keys
-- @class function
-- @name table.keys_filter

--- Reverse a table
-- @param t the table to reverse
-- @return the reversed table
-- @class function
-- @name table.reverse

--- Clone a table
-- @param t the table to clone
-- @return a clone of t
-- @class function
-- @name table.clone
