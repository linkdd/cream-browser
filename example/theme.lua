global.font = "droidsans normal 8"
global.box  = { horizontal = false, padding = 5 }

statusbar.box = { pack_start = false, expand = false, fill = false, padding = 0 }
statusbar.bg_normal = "#000000"
statusbar.bg_secure = "#B0FF00"
statusbar.fg_normal = "#FFFFFF"
statusbar.fg_secure = "#000000"
statusbar.font      = "droidsans normal 8"

tab.box       = { pack_start = true, expand = false, fill = false, padding = 0 }
tab.border    = "1px solid black"
tab.bg_normal = "#505050"
tab.bg_focus  = "#000000"
tab.fg_normal = "#CCCCCC"
tab.fg_focus  = "#FFFFFF"
tab.font      = "droidsans normal 8"
tab.height    = "11px"

prompt.box       = { pack_start = false, expand = false, fill = false, padding = 0 }
prompt.font      = "droidsans normal 8"
prompt.bg_normal = "#151515"
prompt.bg_error  = "#FF0000"
prompt.fg_normal = "#CCCCCC"
prompt.fg_error  = "#000000"

webview.box = { pack_start = true, expand = true, fill = true, padding = 0 }

-- Module VTE (Virtual Terminal Emulator)
modules.vte = {}
modules.vte.font  = "droidsans normal 8"
modules.vte.shell = "/bin/sh"
modules.vte.bg    = "#000000"
modules.vte.fg    = "#CCCCCC"
