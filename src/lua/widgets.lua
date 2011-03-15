----------------------------------------------------
-- Functions to configure the Cream-Browser's GUI
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.widgets")

local theme = require ("cream.theme")

capi.widget_box (theme.theme.global.box)

notebook  = capi.widget_notebook()
webview   = capi.widget_webview()
statusbar = capi.widget_statusbar()
promptbox = capi.widget_promptbox()

notebook.theme  = theme.tab
webview.theme   = theme.webview
statusbar.theme = theme.statusbar
promptbox.theme = theme.prompt

--- Pack widgets.
-- This function packs every widgets into a GtkBox
-- object.
-- @param t A table containing all widgets wanted.
function box (t)
     for _, v in pairs (t) do
          capi.widget_box_pack (v)
     end
end

--- Set notebook mouse bindings.
-- @param t A table containing all mouse bindings.
function notebook.buttons (t)
     capi.widget_notebook_set_buttons (t)
end

--- Add signal to Notebook.
-- This function will connect a Lua function to a GTK signal
-- associated to the notebook object.
-- @param signame Signal's name.
-- @param sigfunc Signal'd handler.
function notebook.add_signal (signame, sigfunc)
     capi.widget_connect (notebook, signame, sigfunc)
end

--- Add signal to Webview.
-- This function will connect a Lua function to a GTK signal
-- associated to the webview object.
-- @param signame Signal's name.
-- @param sigfunc Signal'd handler.
function webview.add_signal (signame, sigfunc)
     capi.widget_connect (webview, signame, sigfunc)
end

--- Add signal to Statusbar.
-- This function will connect a Lua function to a GTK signal
-- associated to the statusbar object.
-- @param signame Signal's name.
-- @param sigfunc Signal'd handler.
function statusbar.add_signal (signame, sigfunc)
     capi.widget_connect (statusbar, signame, sigfunc)
end

--- Add signal to Promptbox.
-- This function will connect a Lua function to a GTK signal
-- associated to the promptbox object.
-- @param signame Signal's name.
-- @param sigfunc Signal'd handler.
function promptbox.add_signal (signame, sigfunc)
     capi.widget_connect (promptbox, signame, sigfunc)
end

--- Set the text printed in the promptbox.
-- @param txt The text to be printed.
function promptbox.text (txt)
     capi.promptbox_set_text (txt)
end
