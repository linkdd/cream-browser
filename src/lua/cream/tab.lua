-- Functions about tabs managment
-- @author David Delassus &lt;linkdd@ydb.me&gt;

local capi =
{
     notebook = ViewArea,
     tab = WebView
}

module ("cream.tab")

function viewprev ()
     capi.notebook:viewprev ()
end

function viewnext ()
     capi.notebook:viewnext ()
end
