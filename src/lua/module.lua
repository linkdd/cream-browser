----------------------------------------------------
-- Functions to configure, enable or disable
-- Cream-Browser's modules.
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.module")

--- Enable a module.
-- @param modulename The name of the module to load.
-- @return The return code of capi.module_enable().
function enable (modulename)
     return capi.module_enable (modulename)
end

--- Disable a module.
-- Disable a loaded module, usable into a keybinding/mouse binding.
-- @param modulename The name of the module to unload.
-- @return The return code of capi.module_disable().
function disable (modulename)
     return capi.module_disable (modulename)
end

--- Reload a module.
-- Disable a loaded module and then load it again, this prevents
-- module's freeze.
-- @param modulename The name of the module to reload.
-- @return The return code of disable/enable.
function reload (modulename)
     local ret = 0

     ret = disbale (modulename)
     if ret == 0 then
          ret = enable (modulename)
     end

     return ret
end
