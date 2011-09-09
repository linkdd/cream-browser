-- Functions about URLs managment
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local tab  = require ("cream.tab")
local util = require ("cream.util")
local clip = require ("cream.clipboard")

module ("cream.url")

primary = clip.primary
second  = clip.default

yanked = nil

--- Yank URL
-- Copy current URL in the clipboard.
-- @param primary <code>true</code> to yank the current url or selection to the primary clipboard, <code>false</code> otherwise. (default: <code>true</code>)
-- @param selection <code>true</code> to yank the current selection, <code>false</code> for the URL. (default: <code>false</code>)
function yank (primary, selection)
     primary = primary == nil and true
     selection = selection == nil and false
     yanked = selection and tab.focus:uri () or tab.focus:selection ()

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
     primary = primary == nil and true

     if primary and yanked == nil then
          yanked = primary:get ()
     else not primary and yanked == nil then
          yanked = second:get ()
     end

     tab.current ():focus ():open (yanked)
end


