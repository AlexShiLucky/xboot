/*
 * driver/spi/spi.c
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
#include <spi/spi.h>

/* 根据名称搜索spi设备 */
struct spi_t * search_spi(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_SPI);
	if(!dev)
		return NULL;
	return (struct spi_t *)dev->priv;
}

/* 注册spi设备 */
bool_t register_spi(struct device_t ** device, struct spi_t * spi)
{
	struct device_t * dev;

	if(!spi || !spi->name || !spi->type)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(spi->name);
	dev->type = DEVICE_TYPE_SPI;
	dev->driver = NULL;
	dev->priv = spi;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
	    /* 如若注册设备,释放先前申请资源 */
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

/* 撤销spi设备 */
bool_t unregister_spi(struct spi_t * spi)
{
	struct device_t * dev;

	if(!spi || !spi->name)
		return FALSE;

    /* 根据spi名称搜索SPI设备 */
	dev = search_device(spi->name, DEVICE_TYPE_SPI);
	if(!dev)
		return FALSE;

    /* 注销设备 */
	if(!unregister_device(dev))
		return FALSE;

    /* 移除设备路径kobj */
	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

/* spi发送消息 */
int spi_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
    /* spi存在并且消息不为空 */
	if(!spi || !msg)
		return 0;
	return spi->transfer(spi, msg);
}

/* spi片选有效 */
void spi_select(struct spi_t * spi, int cs)
{
	if(spi && spi->select)
		spi->select(spi, cs);
}

/* spi片选无效 */
void spi_deselect(struct spi_t * spi, int cs)
{
	if(spi && spi->deselect)
		spi->deselect(spi, cs);
}

/* 根据spi相关参数申请一个spi设备 */
struct spi_device_t * spi_device_alloc(const char * spibus, int cs, int type, int mode, int bits, int speed)
{
	struct spi_device_t * dev;
	struct spi_t * spi;

    /* 根据名称搜索spi */
	spi = search_spi(spibus);
	if(!spi)
		return NULL;

	type &= (SPI_TYPE_SINGLE | SPI_TYPE_DUAL | SPI_TYPE_QUAD | SPI_TYPE_OCTAL);
	type = (type > 0) ? type : SPI_TYPE_SINGLE;
	if(!(spi->type & type))
		return NULL;

    /* 申请一个spi设备块 */
	dev = malloc(sizeof(struct spi_device_t));
	if(!dev)
		return NULL;

    /* 配置spi设备块 */
	dev->spi = spi;
	dev->cs = (cs > 0) ? cs : 0;
	dev->type = type;
	dev->mode = mode & 0x3;
	dev->bits = bits;
	dev->speed = (speed > 0) ? speed : 0;
	return dev;
}

/* 释放一个spi设备 */
void spi_device_free(struct spi_device_t * dev)
{
	if(dev)
		free(dev);
}

/* spi设备先写后读 */
int spi_device_write_then_read(struct spi_device_t * dev, void * txbuf, int txlen, void * rxbuf, int rxlen)
{
	struct spi_msg_t msg;

	if(!dev)
		return -1;

	msg.type = dev->type;
	msg.mode = dev->mode;
	msg.bits = dev->bits;
	msg.speed = dev->speed;

	if(txlen > 0)
	{
		msg.txbuf = txbuf;
		msg.rxbuf = NULL;
		msg.len = txlen;
		if(dev->spi->transfer(dev->spi, &msg) != txlen)
			return -1;
	}
	if(rxlen > 0)
	{
		msg.txbuf = NULL;
		msg.rxbuf = rxbuf;
		msg.len = rxlen;
		if(dev->spi->transfer(dev->spi, &msg) != rxlen)
			return -1;
	}
	return 0;
}

/* spi设备选择有效 */
void spi_device_select(struct spi_device_t * dev)
{
	if(dev && dev->spi && dev->spi->select)
		dev->spi->select(dev->spi, dev->cs);
}

/* spi设备选择无效 */
void spi_device_deselect(struct spi_device_t * dev)
{
	if(dev && dev->spi && dev->spi->deselect)
		dev->spi->deselect(dev->spi, dev->cs);
}
