--- API for perl compatible regular expression
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

module ("cream.regex")


--- Create a new perl compatible regular expression
-- @param pattern The regex pattern
-- @return A new regex
-- @class function
-- @name cream.regex

--- Scans for a match in string for pattern
-- @param str String to scan
-- @return <code>TRUE</code> is the string matched, <code>FALSE</code> otherwise
-- @class function
-- @name Regex:match

--- Replaces all occurrences of the pattern in regex with the replacement text
-- @param str String to scan
-- @param replace Replace all occurrences with this string
-- @return The new string on success, <code>nil</code> otherwise
-- @class function
-- @name Regex:replace


