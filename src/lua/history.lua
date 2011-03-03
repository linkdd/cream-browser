----------------------------------------------------
-- Functions to manage Cream-Browser's history
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.history")

local tab = require ("cream.tab")

--- Go backward in the history.
function backward ()
     capi.go_back (tab.focus)
end

--- Go forward in the history.
function forward ()
     capi.go_forward (tab.focus)
end

--- Get current history.
-- @param limit Number of history items to return.
-- @return A list of history items.
-- @see item
function get (limit)
     return capi.history_get (limit)
end

--- History item's structure.<br/>
-- @name item
-- @class table
-- @field timestamp Unix time when the page was visited.
-- @field uri URI of the visited page.
-- @field title Title ofthe visited page.
-- item = {}
