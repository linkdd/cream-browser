-- Cream-Browser lua API
require ("cream")

useragents = {
     { "google\.(fr|com)", "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.2.1) Gecko/20100122 firefox/3.6.1" }
}

-- Load theme
cream.theme.init ("~/.config/cream-browser/themes/default/theme.lua")

-- Load wanted modules
cream.module.enable ("webkit")
cream.module.enable ("about")
cream.module.enable ("ftp")

-- Misc
cream.config.proxy.url    = "192.168.1.172:8123"
cream.config.proxy.enable = true
cream.config.javascript.enable = true
cream.config.cookies.enable = true
cream.config.session.enable = true

cream.config.encodage = "UTF-8"
cream.config.homepage = "http://cream-browser.net"

-- Build GUI
cream.widgets.box ({
     cream.widgets.notebook,
     cream.widgets.webview,
     cream.widgets.statusbar,
     cream.widgets.promptbox
})

-- Mouse bindings

cream.widgets.notebook:buttons (cream.util.table.join (
     cream.button ({ },        1, function (t) cream.tab.focus = t; t:raise () end),
     cream.button ({ "Mod1" }, 3, function (t) t:close () end),
     cream.button ({ },        4, cream.tab.viewnext),
     cream.button ({ },        5, cream.tab.viewprev)
))

-- Key bindings
globalkeys = cream.util.table.join (
     -- Misc
     cream.key ({ "Mod1", "Control" },  "q",      cream.util.quit),
     cream.key ({ "Control" },          "p",      function () cream.config.proxy.enable = not cream.config.proxy.enable end),
     cream.key ({ "Control" },          "j",      function () cream.config.javascript.enable = not cream.config.javascript.enable end),
     cream.key ({ "Control" },          "c",      function () cream.config.cookies.enable = not cream.config.cookies.enable end),
     cream.key ({ "Control" },          "s",      function () cream.config.session.enable = not cream.session.enable end),
     -- Cream-Browser modes
     cream.key ({ },                    "Insert", function () cream.mode.current = cream.mode.insert end),
     cream.key ({ },                    "i",      function () cream.mode.current = cream.mode.insert end),
     cream.key ({ },                    ":",      function () cream.mode.current = cream.mode.prompt end),
     cream.key ({ },                    "Escape", function () cream.mode.current = cream.mode.command end),
     cream.key ({ },                    "f",      function () cream.mode.current = cream.mode.hint end),
     -- Notebook controls
     cream.key ({ "Mod1" },             "Left",   cream.tab.viewprev),
     cream.key ({ "Mod1" },             "Right",  cream.tab.viewnext),
     cream.key ({ "Control" },          "w",      function () cream.tab.focus:close () end),
     cream.key ({ "Control" },          "t",      cream.tab.new),
     -- Basic commands
     cream.key ({ },                    "o",      function () cream.mode.current = cream.mode.prompt; cream.widgets.promptbox.text (":open ") end),
     cream.key ({ "Shift" },            "o",      function () cream.mode.current = cream.mode.prompt; cream.widgets.promptbox.text (":open " .. cream.tab.focus:url ()) end),
     cream.key ({ },                    "t",      function () cream.mode.current = cream.mode.prompt; cream.widgets.promptbox.text (":tabopen ") end),
     cream.key ({ "Shift" },            "t",      function () cream.mode.current = cream.mode.prompt; cream.widgets.promptbox.text (":tabopen " .. cream.tab.focus:url ()) end),
     cream.key ({ },                    "y",      cream.url.yank),
     cream.key ({ },                    "p",      cream.url.paste),
     cream.key ({ "Shift" },            "p",      function () cream.tab.new (cream.url.yanked) end),
     cream.key ({ },                    "h",      function () cream.tab.focus:url (cream.config.homepage) end),
     cream.key ({ "Shift" },            "h",      function () cream.tab.new (cream.config.homepage) end),
     -- History
     cream.key ({ "Control" },          "Left",   cream.history.backward),
     cream.key ({ "Control" },          "Right",  cream.history.forward)
)

-- Bind all key numbers to notebook
for i = 0, 9 do
     globalkeys = cream.util.table.join (globalkeys,
          cream.key ({ "Mod1" },        i,        function () cream.tab.goto (i) end),
          cream.key ({ "Control" },     i,        function () cream.tab.close (i) end)
     )
end

cream.keys (globalkeys)

-- Add signals handler to webview
cream.widgets.webview:add_signal ("download-requested", function (w, d)
          cream.util.spawn ("urxvt -e wget \"" .. d:url () .. "\" -O \"" .. d:filename () .. "\"")
     end)

cream.widgets.webview:add_signal ("load-commit", function (w)
          for i = 1, #useragents do
               if cream.util.regex.match (useragents[i][1], w:url ()) then
                    w:useragent (useragents[i][2])
               end
          end

          cream.util.spawn ("echo \"" .. w:url () .. " " .. w:title () .. "\" >> ~/.cache/cream-browser/history")
     end)

