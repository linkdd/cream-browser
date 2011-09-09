-- Notebook API
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.tab")

--- Get the current focused notebook
function current () end

--- Get the current focused webview
function Notebook:focus () end

--- Go to the previous page
function Notebook:viewprev () end

--- Go to the next page
function Notebook:viewnext () end

--- Go to the nth page
-- @param idx Page's index
-- @class function
-- @name Notebook:goto

--- Close the nth page
-- @param idx Page's index
-- @class function
-- @name Notebook:close

--- Open a new page
-- @param url New URL to load
-- @class function
-- @name Notebook:new
