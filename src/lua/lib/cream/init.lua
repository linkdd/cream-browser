-- Cream-Browser API
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

require ("cream.regex")
require ("cream.clipboard")
require ("cream.util")
require ("cream.keys")
require ("cream.theme")

local capi =
{
     widgets = widgets,
     util = util,
     bit = bit
}

module ("cream")

-- bitwise API
bit = capi.bit

-- inputbox functions
inputbox =
{
     text  = capi.widgets.inputbox_text,
     focus = capi.widgets.inputbox_focus
}

-- statusbar functions
statusbar =
{
     set_state    = capi.widgets.statusbar_set_state,
     set_link     = capi.widgets.statusbar_set_link,
     set_history  = capi.widgets.statusbar_set_history,
     set_scroll   = capi.widgets.statusbar_set_scroll,
     set_progress = capi.widgets.statusbar_set_progress
}

-- browser's mode
state =
{
     insert  = 0x01,
     command = 0x02,
     search  = 0x04,
     embed   = 0x08,
     caret   = 0x10,
     normal  = 0x20,
     all     = 0x3F
}

function state.current (...)
     capi.util.state (...)
end

