-- Functions about tabs managment
-- @author David Delassus &lt;david.jose.delassus@gmail.com&gt;

local capi =
{
     notebook = notebook,
     widgets = widgets
}

module ("cream.tab")

function current ()
     return capi.widgets.notebook_get_focused ()
end

function focus ()
     return capi.notebook:focus ()
end

function viewprev ()
     capi.notebook:viewprev ()
end

function viewnext ()
     capi.notebook:viewnext ()
end

function goto (idx)
     capi.notebook:webviews(idx):raise ()
end

function close (idx)
     capi.notebook:webviews(idx):close ()
end

function new (url)
     capi.notebook:new (url)
end
