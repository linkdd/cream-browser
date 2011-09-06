-- Cream-Browser API
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

require ("cream.regex")
require ("cream.clipboard")
require ("cream.util")
--require ("cream.tab")
--require ("cream.url")

require ("cream.theme")

local capi =
{
     widgets = widgets
}

module ("cream")

inputbox =
{
     text = capi.widgets.inputbox_text
}

statusbar =
{
     set_state    = capi.widgets.statusbar_set_state,
     set_link     = capi.widgets.statusbar_set_link,
     set_history  = capi.widgets.statusbar_set_history,
     set_scroll   = capi.widgets.statusbar_set_scroll,
     set_progress = capi.widgets.statusbar_set_progress
}
