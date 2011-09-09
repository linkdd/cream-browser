--- API for managing clipboards
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.clipboard")

--- Create a new clipboard
-- @param name Clipboard's ATOM name, usually it is <code>PRIMARY</code>, but it can also be <code>CLIPBOARD</code>.
-- @return A new clipboard
-- @class function
-- @name cream.clipboard

--- Get text from clipboard
-- @param clip The clipboard object
-- @return Text from the clipboard or <code>nil</code> if none.
-- @class function
-- @name Clipboard:get

--- Set text from clipboard
-- @param clip The clipboard object
-- @param text Text to put in the clipboard
-- @class function
-- @name Clipboard:set

--- <code>PRIMARY</code> clipboard
-- @class table
-- @name primary

--- Default clipboard (<code>CLIPBOARD</code>)
-- @class table
-- @name default
