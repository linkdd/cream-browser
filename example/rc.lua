-- Cream-Browser lua API
require ("cbapi")

useragents = {
     { "google\.(fr|com)", "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.2.1) Gecko/20100122 firefox/3.6.1" }
}

-- Load theme
cbapi.theme.init ("~/.config/cream-browser/themes/default/theme.lua")

-- Load wanted modules
cbapi.module.enable ("webkit")
cbapi.module.enable ("about")
cbapi.module.enable ("ftp")

-- Misc
cbapi.proxy.url    = "192.168.1.172:8123"
cbapi.proxy.enable = true
cbapi.javascript.enable = true
cbapi.cookies.enable = true
cbapi.session.enable = true

cbapi.encodage = "UTF-8"
cbapi.homepage = "http://cream-browser.net"

-- Build GUI
cbapi.widgets.box ({
     cbapi.widgets.notebook,
     cbapi.widgets.webview,
     cbapi.widgets.statusbar,
     cbapi.widgets.promptbox
})

-- Mouse bindings

cbapi.widgets.notebook.buttons (cbapi.util.table.join (
     cbapi.button ({ },        1, function (t) cbapi.tab.focus = t; t:raise () end),
     cbapi.button ({ "Mod1" }, 3, function (t) t:close () end),
     cbapi.button ({ },        4, cbapi.tab.viewnext),
     cbapi.button ({ },        5, cbapi.tab.viewprev)
))

-- Key bindings
globalkeys = cbapi.util.table.join (
     -- Misc
     cbapi.key ({ "Mod1", "Control" },  "q",      cbapi.quit),
     cbapi.key ({ "Control" },          "p",      function () cbapi.proxy.enable = not cbapi.proxy.enable end),
     cbapi.key ({ "Control" },          "j",      function () cbapi.javascript.enable = not cbapi.javascript.enable end),
     cbapi.key ({ "Control" },          "c",      function () cbapi.cookies.enable = not cbapi.cookies.enable end),
     cbapi.key ({ "Control" },          "s",      function () cbapi.session.enable = not cbapi.session.enable end),
     -- Cream-Browser modes
     cbapi.key ({ },                    "Insert", function () cbapi.mode.current = cbapi.mode.insert end),
     cbapi.key ({ },                    "i",      function () cbapi.mode.current = cbapi.mode.insert end),
     cbapi.key ({ },                    ":",      function () cbapi.mode.current = cbapi.mode.prompt end),
     cbapi.key ({ },                    "Escape", function () cbapi.mode.current = cbapi.mode.command end),
     cbapi.key ({ },                    "f",      function () cbapi.mode.current = cbapi.mode.hint end),
     -- Notebook controls
     cbapi.key ({ "Mod1" },             "Left",   cbapi.tab.viewprev),
     cbapi.key ({ "Mod1" },             "Right",  cbapi.tab.viewnext),
     cbapi.key ({ "Control" },          "w",      function () cbapi.tab.focus:close () end),
     cbapi.key ({ "Control" },          "t",      cbapi.tab.new),
     -- Basic commands
     cbapi.key ({ },                    "o",      function () cbapi.mode.current = cbapi.mode.prompt; cb_promptbox.text = ":open " end),
     cbapi.key ({ "Shift" },            "o",      function () cbapi.mode.current = cbapi.mode.prompt; cb_promptbox.text = ":open " .. cbapi.tab.focus:url () end),
     cbapi.key ({ },                    "t",      function () cbapi.mode.current = cbapi.mode.prompt; cb_promptbox.text = ":tabopen " end),
     cbapi.key ({ "Shift" },            "t",      function () cbapi.mode.current = cbapi.mode.prompt; cb_promptbox.text = ":tabopen " .. cbapi.tab.focus:url () end),
     cbapi.key ({ },                    "y",      cbapi.url.yank),
     cbapi.key ({ },                    "p",      cbapi.url.paste),
     cbapi.key ({ "Shift" },            "p",      function () cbapi.tab.new (cbapi.url.yanked) end),
     cbapi.key ({ },                    "h",      function () cbapi.tab.focus:url (cbapi.homepage) end),
     cbapi.key ({ "Shift" },            "h",      function () cbapi.tab.new (cbapi.homepage) end),
     -- History
     cbapi.key ({ "Control" },          "Left",   cbapi.history.backward),
     cbapi.key ({ "Control" },          "Right",  cbapi.history.forward)
)

-- Bind all key numbers to notebook
for i = 0, 9 do
     globalkeys = cbapi.util.table.join (globalkeys,
          cbapi.key ({ "Mod1" },        i,        function () cbapi.tab.goto (i) end),
          cbapi.key ({ "Control" },     i,        function () cbapi.tab.close (i) end)
     )
end

cbapi.keys (globalkeys)

-- Add signals handler to webview
cbapi.widgets.webview.add_signal ("download-requested", function (w, d)
          cbapi.util.spawn ("urxvt -e wget \"" .. d:url () .. "\" -O \"" .. d:filename () .. "\"")
     end)

cbapi.widgets.webview.add_signal ("load-committed", function (w)
          for i = 1, #useragents do
               if cbapi.util.regex.match (useragents[i][1], w:url ()) then
                    w:useragent (useragents[i][2])
               end
          end

          cbapi.util.spawn ("echo \"" .. w:url () .. " " .. w:title () .. "\" >> ~/.cache/cream-browser/history")
     end)

