--- API for Cream-Browser's statusbar
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.statusbar")

--- Set browser's state
-- @param state The new browser's state
-- @class function
-- @name set_state

--- Set current link
-- @param link The URI of the focused webview
-- @class function
-- @name set_link

--- Set history state
-- @param can_go_back Set to <code>true</code> if we can go back in history
-- @param can_go_forward Set to <code>true</code> if we can go forward in history
-- @class function
-- @name set_history

--- Set current scrolling state
-- @param scroll Current scrolling percent
-- @class function
-- @name set_scroll

--- Set loading state
-- @param progress Current loading percent
-- @class function
-- @name set_progress

