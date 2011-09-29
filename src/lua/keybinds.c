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
 * \defgroup lua-keybinds Key Bindings
 * \ingroup lua
 * Package 'keys' of the lua API.
 *
 * @{
 */

/*!
 * \fn static int luaL_keybinds_add (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Add a keybind.
 * \code keys.add (accelerator, mode, func) \endcode
 */
static int luaL_keybinds_add (lua_State *L)
{
     int statemask   = luaL_checkint (L, 1);
     int modmask     = luaL_checkint (L, 2);
     const char *cmd = luaL_checkstring (L, 3);
     int lua_func    = luaL_checkfunction (L, 4);

     keybinds_add (statemask, modmask, cmd, lua_func);
     return 0;
}

static const luaL_reg cream_keybinds_functions[] =
{
     { "add", luaL_keybinds_add },
     { NULL, NULL }
};

/*!
 * \fn int luaL_keybinds_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_keybinds_register (lua_State *L)
{
     luaL_register (L, "keys", cream_keybinds_functions);
     return 1;
}

/*! @} */
