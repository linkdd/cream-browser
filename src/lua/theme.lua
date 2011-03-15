----------------------------------------------------
-- Cream-Browser's themes managment
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.theme")

--- Global theme.<br/>
-- @name global
-- @class table
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
-- @field box Style of GtkBox object, ie: <code>{ horizontal = false, padding = 5 }</code>
global = {}

--- Statusbar's theme.<br/>
-- @name statusbar
-- @class table
-- @field bg_normal Background color
-- @field bg_secure Background color when this is a secure protocol (ie: HTTPS)
-- @field fg_normal Foreground color
-- @field fg_secure Foreground color when this is a secure protocol (ie: HTTPS)
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
-- @field box Style of widget into the GtkBox object, ie: <code>{ pack_start = true, expand = true, fill = true, padding = 0 }</code>
statusbar = {}

--- Notebook's theme.<br/>
-- @name tab
-- @class table
-- @field border Border style, ie: <code>"1px solid black"</code>
-- @field bg_normal Background color
-- @field bg_focus Background color on the focused tab
-- @field fg_normal Foreground color
-- @field fg_focus Foreground color on the focused tab
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
-- @field heigh Height of the tabbar, ie: <code>"11px"</code>
-- @field box Style of widget into the GtkBox object, ie: <code>{ pack_start = true, expand = true, fill = true, padding = 0 }</code>
tab = {}

--- Prompt's theme.<br/>
-- @name prompt
-- @class table
-- @field box Style of widget into the GtkBox object, ie: <code>{ pack_start = true, expand = true, fill = true, padding = 0 }</code>
-- @field font Font family/style/size, ie: <code>"sans normal 8"</code>
-- @field bg_normal Background color
-- @field bg_error Background color when an error is printed
-- @field fg_normal Foreground color
-- @field fg_error Foreground color when an error is printed
prompt = {}

--- Webview area's theme.<br/>
-- @name webview
-- @class table
-- @field box Style of widget into the GtkBox object, ie: <code>{ pack_start = true, expand = true, fill = true, padding = 0 }</code>
webview = {}

--- Modules' theme<br/>
-- @name modules
-- @class table
modules = {}

--- Init theme file.
-- This function loads a Lua theme using dofile().
-- @param filename Path to the lua theme file.
function init (filename)
     dofile (filename)
end
