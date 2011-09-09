/*
* Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

#include "../local.h"

/*!
 * \defgroup lua-widgets Widgets
 * \ingroup lua
 * Lua bindings of some Cream-Browser's widgets.
 *
 * @{
 * @}
 */

/*!
 * \defgroup lua-inputbox Inputbox
 * \ingroup lua-widgets
 * #Inputbox bindings for lua.
 * @{
 */

/*!
 * \fn static int luaL_inputbox_text (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get inputbox text or set it.
 * \code function inputbox.text () \endcode
 * \code function inputbox.text (txt) \endcode
 */

static int luaL_inputbox_text (lua_State *L)
{
     if (lua_gettop (L) >= 1)
     {
          const gchar *txt = luaL_checkstring (L, 1);
          gtk_entry_set_text (GTK_ENTRY (global.gui.inputbox), txt);
          return 0;
     }
     else
     {
          lua_pushstring (L, gtk_entry_get_text (GTK_ENTRY (global.gui.inputbox)));
          return 1;
     }
}

/*
 * \fn static int luaL_inputbox_focus (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Give focus to the inputbox.
 * \code function inputbox.focus () \endcode
 */
static int luaL_inputbox_focus (lua_State *L)
{
     gtk_widget_grab_focus (global.gui.inputbox);
     gtk_editable_set_position (GTK_EDITABLE (global.gui.inputbox), -1);
     return 0;
}

/*! @} */

/*!
 * \defgroup lua-statusbar Statusbar
 * \ingroup lua-widgets
 * #Statusbar bindings for lua.
 * @{
 */

/*!
 * \fn static int luaL_statusbar_set_state (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set browser's state.
 * \code function statusbar.set_state (state) \endcode
 */
static int luaL_statusbar_set_state (lua_State *L)
{
     CreamMode state = luaL_checkint (L, 1);
     statusbar_set_state (CREAM_STATUSBAR (global.gui.statusbar), state);
     return 0;
}

/*!
 * \fn static int luaL_statusbar_set_link (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set statusbar's link.
 * \code function statusbar.set_link (link) \endcode
 */
static int luaL_statusbar_set_link (lua_State *L)
{
     const gchar *link = luaL_checkstring (L, 1);
     statusbar_set_link (CREAM_STATUSBAR (global.gui.statusbar), link);
     return 0;
}

/*!
 * \fn static int luaL_statusbar_set_history (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set history's state in statusbar.
 * \code function statusbar.set_history (can_go_back, can_go_forward) \endcode
 */
static int luaL_statusbar_set_history (lua_State *L)
{
     gboolean can_go_back    = luaL_checkboolean (L, 1);
     gboolean can_go_forward = luaL_checkboolean (L, 2);
     statusbar_set_history (CREAM_STATUSBAR (global.gui.statusbar), can_go_back, can_go_forward);
     return 0;
}

/*!
 * \fn static int luaL_statusbar_set_scroll (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set page's scrolling percent.
 * \code function statusbar.set_scroll (scroll) \endcode
 */

static int luaL_statusbar_set_scroll (lua_State *L)
{
     gdouble scroll = (gdouble) luaL_checkint (L, 1);
     statusbar_set_scroll (CREAM_STATUSBAR (global.gui.statusbar), scroll / 100.0);
     return 0;
}

/*!
 * \fn static int luaL_statusbar_set_progress (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set page's loading percent.
 * \code function statusbar.set_progress (progress) \endcode
 */

static int luaL_statusbar_set_progress (lua_State *L)
{
     gdouble progress = (gdouble) luaL_checkint (L, 1);
     statusbar_set_progress (CREAM_STATUSBAR (global.gui.statusbar), progress / 100.0);
     return 0;
}

/*! @} */

/*!
 * \addtogroup lua-widgets
 *
 * @{
 */

static const luaL_reg cream_widgets_functions[] =
{
     { "inputbox_text",          luaL_inputbox_text },
     { "inputbox_focus",         luaL_inputbox_focus },
     { "statusbar_set_state",    luaL_statusbar_set_state },
     { "statusbar_set_link",     luaL_statusbar_set_link },
     { "statusbar_set_history",  luaL_statusbar_set_history },
     { "statusbar_set_scroll",   luaL_statusbar_set_scroll },
     { "statusbar_set_progress", luaL_statusbar_set_progress },
     { NULL, NULL }
};

/*!
 * \fn int luaL_widgets_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_widgets_register (lua_State *L)
{
     luaL_register (L, "widgets", cream_widgets_functions);
     return 1;
}

/*! @} */
