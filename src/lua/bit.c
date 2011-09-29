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
* \defgroup bit Bitwise API
* \ingroup lua
* Bitwise operators.
* Package 'bit' of the lua API
* @{
*/

static int luaL_bit_not (lua_State *L)
{
     int ret = luaL_checkint (L, 1);
     lua_pushinteger (L, ~ret);
     return 1;
}

static int luaL_bit_and (lua_State *L)
{
     int i, ret = luaL_checkint (L, 1);

     for (i = lua_gettop (L); i > 1; --i)
          ret &= luaL_checkint (L, i);

     lua_pushinteger (L, ret);
     return 1;
}

static int luaL_bit_or (lua_State *L)
{
     int i, ret = luaL_checkint (L, 1);

     for (i = lua_gettop (L); i > 1; --i)
          ret |= luaL_checkint (L, i);

     lua_pushinteger (L, ret);
     return 1;
}

static int luaL_bit_xor (lua_State *L)
{
     int i, ret = luaL_checkint (L, 1);

     for (i = lua_gettop (L); i > 1; --i)
          ret ^= luaL_checkint (L, i);

     lua_pushinteger (L, ret);
     return 1;
}

static int luaL_bit_lshift (lua_State *L)
{
     int ret = luaL_checkint (L, 1);
     int n   = luaL_checkint (L, 2);
     lua_pushinteger (L, ret << n);
     return 1;
}

static int luaL_bit_rshift (lua_State *L)
{
     int ret = luaL_checkint (L, 1);
     int n   = luaL_checkint (L, 2);
     lua_pushinteger (L, ret >> n);
     return 1;
}

static int luaL_bit_rol (lua_State *L)
{
     int ret = luaL_checkint (L, 1);
     int n   = luaL_checkint (L, 2);
     lua_pushinteger (L, (ret << n) | (ret >> (32 - n)));
     return 1;
}

static int luaL_bit_ror (lua_State *L)
{
     int ret = luaL_checkint (L, 1);
     int n   = luaL_checkint (L, 2);
     lua_pushinteger (L, (ret << (32 - n) | (ret >> n)));
     return 1;
}

static const struct luaL_Reg cream_bit_functions[] =
{
     { "bnot",   luaL_bit_not },
     { "band",   luaL_bit_and },
     { "bor",    luaL_bit_or },
     { "bxor",   luaL_bit_xor },
     { "lshift", luaL_bit_lshift },
     { "rshift", luaL_bit_rshift },
     { "rol",    luaL_bit_rol },
     { "ror",    luaL_bit_ror },
     { NULL, NULL }
};

int luaL_bit_register (lua_State *L)
{
     luaL_register (L, "bit", cream_bit_functions);
     return 1;
}

/*! @} */
