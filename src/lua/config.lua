----------------------------------------------------
-- Functions to configure Cream-Browser
--
-- @author David Delassus &lt;linkdd@ydb.me&gt;
----------------------------------------------------

module ("cream.config")

--- Configure proxy.<br/>
-- @name proxy
-- @class table
-- @field enable <code>true</code> to use a proxy, <code>false</code> otherwise.
-- @field url Proxy's url.
proxy = { enable = false, url = "localhost" }

--- Configure javascript.<br/>
-- @name javascript
-- @class table
-- @field enable <code>true</code> to use javascript, <code>false</code> otherwise.
javascript = { enable = false }

--- Configure cookies.<br/>
-- @name cookies
-- @class table
-- @field enable <code>true</code> to use cookies, <code>false</code> otherwise.
cookies = { enable = false }

--- Configure session.<br/>
-- @name session
-- @class table
-- @field enable <code>true</code> to restore session at start and save it at exit, <code>false</code> otherwise.
session = { enable = false }

--- Configure default encodage (ie: <code>"UTF-8"</code>).
-- @name encodage
-- @class table
encodage = nil

--- Configure default homepage (ie: <code>"http://cream-browser.net"</code>).
-- @name homepage
-- @class table
homepage = nil

