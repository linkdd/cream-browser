--- Cream-Browser API
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream")

--- Set/get current browser's state
-- @param ... If given, set the browser's state
-- @return If no parameters are given, return current browser's state
-- @class function
-- @name state.current

--- All available browser's states
-- @field insert <code>INSERT</code> mode (editing)
-- @field command <code>CMD</code> mode (typing a command in the inputbox)
-- @field search <code>FIND</code> mode (typing a search in the inputbox)
-- @field embed <code>EMBED</code> mode (focus on a plugin like flashplayer)
-- @field caret <code>CARET</code> mode (caret browsing)
-- @field normal Normal mode
-- @field all Mask for all states
-- @class table
-- @name state
