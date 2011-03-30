-- Functions about tabs managment
-- @author David Delassus &lt;linkdd@ydb.me&gt;

local capi =
{
     notebook = notebook
}

module ("cream.tab")

focus = nil

function viewprev ()
     focus = capi.notebook:viewprev ()
end

function viewnext ()
     focus = capi.notebook:viewnext ()
end

function goto (idx)
     focus = capi.notebook.webviews[idx]
     focus:raise ()
end

function close (idx)
     capi.notebook.webviews[idx]:close ()
     focus = capi.notebook.webviews[0]
     focus:raise ()
end

function new (url)
     focus = capi.notebook:new (url)
end
