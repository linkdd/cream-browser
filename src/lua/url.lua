----------------------------------------------------
-- Functions about URLs managment
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.url")

local tab  = require ("cream.tab")
local util = require ("cream.util")

--- Yanked URL.
-- @name yanked
-- @class table
yanked = nil

--- Yank URL
-- Copy current URL in the clipboard.
-- @param primary <code>true</code> to yank the current url to the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
function yank (primary)
     yanked = tab.focus:url ()
     util.clipboard.set (yanked, primary)
end

--- Paste URL.
-- Open in the current tab the yanked URL.
-- @param primary <code>true</code> to paste the yanked url from the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
function paste (primary)
     if yanked == nil then
          yanked = util.clipboard.get (primary)
     end
     tab.focus:open (yanked)
end
