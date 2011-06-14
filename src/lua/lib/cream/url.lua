-- Functions about URLs managment
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local tab  = require ("cream.tab")
local util = require ("cream.util")
local clip = require ("cream.clipboard")

module ("cream.url")

primary = clip ("primary")
second  = clip ("secondary")

yanked = nil

--- Yank URL
-- Copy current URL in the clipboard.
-- @param primary <code>true</code> to yank the current url or selection to the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
-- @param selection <code>true</code> to yank the current selection, <code>false</code> for the URL. (default: <code>false</code>)
function yank (primary, selection)
     if primary == nil then primary = true end
     if selection == nil then selection = false end

     if selection then
          yanked = tab.focus:url ()
     else
          yanked = tab.focus:selection ()
     end

     if primary then
          primary:set (yanked)
     else
          second:set (yanked)
     end
end

--- Paste URL.
-- Open in the current tab the yanked URL.
-- @param primary <code>true</code> to paste the yanked url from the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
function paste (primary)
     if primary == nil then primary = true end

     if primary and yanked == nil then
          yanked = primary:get ()
     else not primary and yanked == nil then
          yanked = second:get ()
     end

     tab.focus:open (yanked)
end


