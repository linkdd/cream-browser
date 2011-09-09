--- Key-bindings managment
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.keys")

--- Add a keybinding
-- @param statelist List of state where the keybind is affected
-- @param modlist List of modifiers key (ie. <code>{ "Shift", "Control" }</code>)
-- @param command Command
-- @param callback Function to call when the keybind is activated
-- @class function
-- @name map

--- All modifier keys mask
-- @field Shift Shift
-- @field Lock CapsLock
-- @field Control Control
-- @field Mod1 Alt
-- @field Mod2 NumLock
-- @field Mod3 The sixth modifier key
-- @field Mod4 Super, Hyper
-- @field Mod5 Mode_switch, ISO_Level3_Shift
-- @field Button1 First button mouse
-- @field Button2 Second button mouse
-- @field Button3 Third button mouse
-- @field Button4 Fourth button mouse
-- @field Button5 Fifth button mouse
-- @field Super Super
-- @field Hyper Hyper
-- @field Meta Meta
-- @field Mask Mask for all modifier keys
-- @class table
-- @name modkeys
