#ifndef __K210_REG_GPIO_H__
#define __K210_REG_GPIO_H__

#define K210_GPIO_BASE		(0x50200000)

enum {
	GPIO_DATA_OUTPUT		= 0x00,
	GPIO_DIRECTION			= 0x04,
	GPIO_SOURCE				= 0x08,
	GPIO_INT_ENABLE			= 0x30,
	GPIO_INT_MASK			= 0x34,
	GPIO_INT_LEVEL			= 0x38,
	GPIO_INT_POLARITY		= 0x3c,
	GPIO_INT_STATUS			= 0x40,
	GPIO_INT_STATUS_RAW		= 0x44,
	GPIO_INT_DEBOUNCE		= 0x48,
	GPIO_INT_CLEAR			= 0x4c,
	GPIO_DATA_INPUT			= 0x50,
	GPIO_SYNC_LEVEL			= 0x54,
	GPIO_ID_CODE			= 0x60,
	GPIO_INT_BOTHEDGE		= 0x68,
};

#endif /* __K210_REG_GPIO_H__ */
