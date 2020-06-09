/*
 * framework/hardware/l-motor.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <motor/motor.h>
#include <framework/hardware/l-hardware.h>

static int l_motor_new(lua_State * L)
{
    const char * name = luaL_checkstring(L, 1);
    struct motor_t * m = search_motor(name);
    if(!m)
        return 0;
    lua_pushlightuserdata(L, m);
    luaL_setmetatable(L, MT_HARDWARE_MOTOR);
    return 1;
}

static int l_motor_list(lua_State * L)
{
    struct device_t * pos, * n;
    struct motor_t * m;

    lua_newtable(L);
    list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_STEPPER], head)
    {
        m = (struct motor_t *)(pos->priv);
        if(!m)
            continue;
        lua_pushlightuserdata(L, m);
        luaL_setmetatable(L, MT_HARDWARE_MOTOR);
        lua_setfield(L, -2, pos->name);
    }
    return 1;
}

static const luaL_Reg l_motor[] = {
    {"new",     l_motor_new},
    {"list",    l_motor_list},
    {NULL,  NULL}
};

static int m_motor_tostring(lua_State * L)
{
    struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
    lua_pushstring(L, m->name);
    return 1;
}

static int m_motor_enable(lua_State * L)
{
    struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
    motor_enable(m);
    lua_settop(L, 1);
    return 1;
}

static int m_motor_disable(lua_State * L)
{
    struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
    motor_disable(m);
    lua_settop(L, 1);
    return 1;
}

static int m_motor_set_speed(lua_State * L)
{
    struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
    int speed = luaL_optinteger(L, 2, 0);
    motor_set_speed(m, speed);
    lua_settop(L, 1);
    return 1;
}

static const luaL_Reg m_motor[] = {
    {"__tostring",  m_motor_tostring},
    {"enable",      m_motor_enable},
    {"disable",     m_motor_disable},
    {"setSpeed",    m_motor_set_speed},
    {NULL,  NULL}
};

int luaopen_hardware_motor(lua_State * L)
{
    luaL_newlib(L, l_motor);
    luahelper_create_metatable(L, MT_HARDWARE_MOTOR, m_motor);
    return 1;
}
