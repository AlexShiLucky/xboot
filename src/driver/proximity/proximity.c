/*
 * driver/proximity/proximity.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <proximity/proximity.h>

static ssize_t proximity_read_distance(struct kobj_t * kobj, void * buf, size_t size)
{
    struct proximity_t * p = (struct proximity_t *)kobj->priv;
    int d = proximity_get_distance(p);
    return sprintf(buf, "%dmm", d);
}

struct proximity_t * search_proximity(const char * name)
{
    struct device_t * dev;

    dev = search_device(name, DEVICE_TYPE_PROXIMITY);
    if(!dev)
        return NULL;
    return (struct proximity_t *)dev->priv;
}

struct proximity_t * search_first_proximity(void)
{
    struct device_t * dev;

    dev = search_first_device(DEVICE_TYPE_PROXIMITY);
    if(!dev)
        return NULL;
    return (struct proximity_t *)dev->priv;
}

bool_t register_proximity(struct device_t ** device,struct proximity_t * p)
{
    struct device_t * dev;

    if(!p || !p->name)
        return FALSE;

    dev = malloc(sizeof(struct device_t));
    if(!dev)
        return FALSE;

    dev->name = strdup(p->name);
    dev->type = DEVICE_TYPE_PROXIMITY;
    dev->priv = p;
    dev->kobj = kobj_alloc_directory(dev->name);
    kobj_add_regular(dev->kobj, "distance", proximity_read_distance, NULL, p);

    if(!register_device(dev))
    {
        kobj_remove_self(dev->kobj);
        free(dev->name);
        free(dev);
        return FALSE;
    }

    if(device)
        *device = dev;
    return TRUE;
}

bool_t unregister_proximity(struct proximity_t * p)
{
    struct device_t * dev;

    if(!p || !p->name)
        return FALSE;

    dev = search_device(p->name, DEVICE_TYPE_PROXIMITY);
    if(!dev)
        return FALSE;

    if(!unregister_device(dev))
        return FALSE;

    kobj_remove_self(dev->kobj);
    free(dev->name);
    free(dev);
    return TRUE;
}

int proximity_get_distance(struct proximity_t * p)
{
    if(p && p->get)
        return p->get(p);
    return 0;
}
