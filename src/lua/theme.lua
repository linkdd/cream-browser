----------------------------------------------------
-- Cream-Browser's themes managment
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.theme")

local util = require ("cream.util")

--- Cream-Browser's theme.<br/>
-- @name theme
-- @class table
-- @field global Global theme.
-- @field statusbar Statusbar's theme.
-- @field tab Notebook's theme.
-- @field prompt Prompt's theme.
-- @field module Modules' theme.
theme = {}

--- Global theme.<br/>
-- @name theme.global
-- @class table
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
theme.global = {}

--- Statusbar's theme.<br/>
-- @name theme.statusbar
-- @class table
-- @field bg_normal Background color
-- @field bg_secure Background color when this is a secure protocol (ie: HTTPS)
-- @field fg_normal Foreground color
-- @field fg_secure Foreground color when this is a secure protocol (ie: HTTPS)
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
theme.statusbar = {}

--- Notebook's theme.<br/>
-- @name theme.tab
-- @class table
-- @field border Border style, ie: <code>"1px solid black"</code>
-- @field bg_normal Background color
-- @field bg_focus Background color on the focused tab
-- @field fg_normal Foreground color
-- @field fg_focus Foreground color on the focused tab
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
theme.tab = {}

--- Prompt's theme<br/>
-- @name theme.prompt
-- @class table
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
-- @field bg_normal Background color
-- @field bg_error Background color when an error is printed
-- @field fg_normal Foreground color
-- @field fg_error Foreground color when an error is printed
theme.prompt = {}

--- Modules' theme<br/>
-- @name theme.module
-- @class table
theme.module = {}

--- Init theme file.
-- This function loads a Lua theme using dofile().
-- @param filename Path to the lua theme file.
function init (filename)
     dofile (filename)
end
