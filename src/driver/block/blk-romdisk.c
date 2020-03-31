/*
 * driver/block/blk-romdisk.c
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

#include <xboot.h>
#include <block/block.h>

struct blk_romdisk_pdata_t
{
	virtual_addr_t addr;
	virtual_size_t size;
};

/* romdisk读取 */
static u64_t blk_romdisk_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct blk_romdisk_pdata_t * pdat = (struct blk_romdisk_pdata_t *)(blk->priv);
	virtual_addr_t offset = pdat->addr + block_offset(blk, blkno);
	u64_t length = block_size(blk) * blkcnt;
	memcpy((void *)buf, (const void *)(offset), length);
	return blkcnt;
}

/* romdisk写入 */
static u64_t blk_romdisk_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

/* romdisk同步 */
static void blk_romdisk_sync(struct block_t * blk)
{
}

/* romdisk探针 */
static struct device_t * blk_romdisk_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct blk_romdisk_pdata_t * pdat;
	struct block_t * blk;
	struct device_t * dev;
	u64_t blkcnt, blksz = SZ_512;
	virtual_addr_t addr = dt_read_long(n, "address", 0);
	virtual_size_t size = dt_read_long(n, "size", 0);

	if(size <= 0)
		return NULL;
	blkcnt = (size + blksz) / blksz;

	pdat = malloc(sizeof(struct blk_romdisk_pdata_t));
	if(!pdat)
		return NULL;

	blk = malloc(sizeof(struct block_t));
	if(!blk)
	{
		free(pdat);
		return NULL;
	}

	pdat->addr = addr;
	pdat->size = blkcnt * blksz;

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	blk->blksz	= blksz;
	blk->blkcnt	= blkcnt;
	blk->read = blk_romdisk_read;
	blk->write = blk_romdisk_write;
	blk->sync = blk_romdisk_sync;
	blk->priv = pdat;

    /* 注册romdisk块设备 */
	if(!(dev = register_block(blk, drv)))
	{
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
		return NULL;
	}
	return dev;
}

/* romdisk移除 */
static void blk_romdisk_remove(struct device_t * dev)
{
	struct block_t * blk = (struct block_t *)dev->priv;

	if(blk)
	{
		unregister_block(blk);
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
	}
}

/* romdisk挂起 */
static void blk_romdisk_suspend(struct device_t * dev)
{
}

/* romdisk释放 */
static void blk_romdisk_resume(struct device_t * dev)
{
}

/* 全局romdisk驱动块 */
static struct driver_t blk_romdisk = {
	.name		= "blk-romdisk",
	.probe		= blk_romdisk_probe,
	.remove		= blk_romdisk_remove,
	.suspend	= blk_romdisk_suspend,
	.resume		= blk_romdisk_resume,
};

/* romdisk驱动初始化 */
static __init void blk_romdisk_driver_init(void)
{
	register_driver(&blk_romdisk);
}

/* romdisk驱动退出 */
static __exit void blk_romdisk_driver_exit(void)
{
	unregister_driver(&blk_romdisk);
}

driver_initcall(blk_romdisk_driver_init);
driver_exitcall(blk_romdisk_driver_exit);
