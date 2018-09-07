/*
 * driver/sd/sdhci.c
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
#include <sd/sdcard.h>
#include <sd/sdhci.h>

struct sdhci_t * search_sdhci(const char * name)
{
    struct device_t * dev;

    dev = search_device(name, DEVICE_TYPE_SDHCI);
    if(!dev)
        return NULL;
    return (struct sdhci_t *)dev->priv;
}

bool_t register_sdhci(struct device_t ** device, struct sdhci_t * sdhci)
{
    struct device_t * dev;

    if(!sdhci || !sdhci->name)
        return FALSE;

    dev = malloc(sizeof(struct device_t));
    if(!dev)
        return FALSE;

    dev->name = strdup(sdhci->name);
    dev->type = DEVICE_TYPE_SDHCI;
    dev->priv = sdhci;
    dev->kobj = kobj_alloc_directory(dev->name);

    if(!register_device(dev))
    {
        kobj_remove_self(dev->kobj);
        free(dev->name);
        free(dev);
        return FALSE;
    }
    sdhci->sdcard = sdcard_probe(sdhci);

    if(device)
        *device = dev;
    return TRUE;
}

bool_t unregister_sdhci(struct sdhci_t * sdhci)
{
    struct device_t * dev;

    if(!sdhci || !sdhci->name)
        return FALSE;

    dev = search_device(sdhci->name, DEVICE_TYPE_SDHCI);
    if(!dev)
        return FALSE;
    sdcard_remove(sdhci->sdcard);

    if(!unregister_device(dev))
        return FALSE;

    kobj_remove_self(dev->kobj);
    free(dev->name);
    free(dev);
    return TRUE;
}

bool_t sdhci_detect(struct sdhci_t * sdhci)
{
    if(sdhci && sdhci->detect)
        return sdhci->detect(sdhci);
    return FALSE;
}

bool_t sdhci_set_width(struct sdhci_t * sdhci, u32_t width)
{
    if(sdhci && sdhci->setwidth)
        return sdhci->setwidth(sdhci, width);
    return FALSE;
}

bool_t sdhci_set_clock(struct sdhci_t * sdhci, u32_t clock)
{
    if(sdhci && sdhci->setclock)
        return sdhci->setclock(sdhci, (clock <= sdhci->clock) ? clock : sdhci->clock);
    return FALSE;
}

bool_t sdhci_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
    if(sdhci && sdhci->transfer)
        return sdhci->transfer(sdhci, cmd, dat);
    return FALSE;
}
