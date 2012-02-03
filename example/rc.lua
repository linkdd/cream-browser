-- Load Cream-Browser's API
require ("cream")

-- keybindings
local state =
{
     all    = { cream.state.all },
     noedit = { cream.state.normal, cream.state.embed, cream.state.caret }
}

-- exit Cream-Browser
cream.keys.map (state.all, { }, "zz",
          function (w)
               cream.util.quit ()
          end)

-- go to normal mode
cream.keys.map (state.all, { }, "Escape",
          function (w)
               cream.inputbox.text ("")
               cream.state.current (cream.state.normal)
          end)

-- close tab
cream.keys.map (state.all, { "Control" }, "w",
          function (w)
               w:close ()
          end)

-- focus to inputbox
cream.keys.map (state.noedit, { }, "colon",
          function (w)
               cream.inputbox.text (":")
               cream.inputbox.focus ()
          end)

cream.keys.map (state.noedit, { }, "/",
          function (w)
               cream.inputbox.text ("/")
               cream.inputbox.focus ()
               cream.state.current (cream.state.search)
          end)

cream.keys.map (state.noedit, { "Shift" }, "?",
          function (w)
               cream.inputbox.text ("?")
               cream.inputbox.focus ()
               cream.state.current (cream.state.search)
          end)

-- Special commands
cream.keys.map (state.noedit, { }, "o",
          function (w)
               cream.inputbox.text (":open ")
               cream.inputbox.focus ()
          end)

cream.keys.map (state.noedit, { "Shift" }, "O",
          function (w)
               cream.inputbox.text (":open " .. w:uri ())
               cream.inputbox.focus ()
          end)

cream.keys.map (state.noedit, { }, "t",
          function (w)
               cream.inputbox.text (":tabopen ")
               cream.inputbox.focus ()
          end)

cream.keys.map (state.noedit, { "Shift" }, "T",
          function (w)
               cream.inputbox.text (":tabopen " .. w:uri ())
               cream.inputbox.focus ()
          end)

-- Yank/Paste
cream.keys.map (state.noedit, { }, "y",
          function (w)
               cream.clipboard.primary:set (w:uri ())
               cream.inputbox.text ("Yanked " .. w:uri ())
          end)

cream.keys.map (state.noedit, { }, "p",
          function (w)
               uri = cream.clipboard.primary:get ()

               if uri ~= "" then
                    w:open (uri)
               end
          end)

cream.keys.map (state.noedit, { "Shift" }, "P",
          function (w)
               uri = cream.clipboard.primary:get ()

               if uri ~= "" then
                    notebook = cream.tab.current ()
                    notebook:new (uri)
               end
          end)

