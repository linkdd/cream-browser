----------------------------------------------------
-- Functions about tabs managment
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.tab")

focus = {}

--- Switch to the previous tab.
-- @name viewprev
-- @class function
viewprev = capi.tab_viewprev

--- Switch to the next tab.
-- @name viewnext
-- @class function
viewnext = capi.tab_viewnext

--- Go to a specific tab.
-- @name goto
-- @class function
-- @param i Tab's number.
goto = capi.tab_goto

--- Close a specific tab.
-- @name close
-- @class function
-- @param i Tab's number.
close = capi.tab_close

--- Open a new tab
-- @name new
-- @class function
-- @param uri URI to load in the new tab
new = capi.tab_new



