#ifndef __D1_IRQ_H__
#define __D1_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define D1_IRQ_UART0			(18)
#define D1_IRQ_UART1			(19)
#define D1_IRQ_UART2			(20)
#define D1_IRQ_UART3			(21)
#define D1_IRQ_UART4			(22)
#define D1_IRQ_UART5			(23)
#define D1_IRQ_TWI0				(25)
#define D1_IRQ_TWI1				(26)
#define D1_IRQ_TWI2				(27)
#define D1_IRQ_TWI3				(28)
#define D1_IRQ_SPI0				(31)
#define D1_IRQ_SPI1				(32)
#define D1_IRQ_PWM				(34)
#define D1_IRQ_IR_TX			(35)
#define D1_IRQ_LEDC				(36)
#define D1_IRQ_OWA				(39)
#define D1_IRQ_DMIC				(40)
#define D1_IRQ_AUDIO_CODEC		(41)
#define D1_IRQ_I2S0				(42)
#define D1_IRQ_I2S1				(43)
#define D1_IRQ_I2S2				(44)
#define D1_IRQ_USB0_DEVICE		(45)
#define D1_IRQ_USB0_EHCI		(46)
#define D1_IRQ_USB0_OHCI		(47)
#define D1_IRQ_USB1_EHCI		(49)
#define D1_IRQ_USB1_OHCI		(50)
#define D1_IRQ_SMHC0			(56)
#define D1_IRQ_SMHC1			(57)
#define D1_IRQ_SMHC2			(58)
#define D1_IRQ_MSI				(59)
#define D1_IRQ_EMAC				(62)
#define D1_IRQ_ECCU_FERR		(64)
#define D1_IRQ_AHB_TIMEOUT		(65)
#define D1_IRQ_DMAC_NS			(66)
#define D1_IRQ_CE_NS			(68)
#define D1_IRQ_SPINLOCK			(70)
#define D1_IRQ_HSTIME0			(71)
#define D1_IRQ_HSTIME1			(72)
#define D1_IRQ_GPADC			(73)
#define D1_IRQ_THS				(74)
#define D1_IRQ_TIMER0			(75)
#define D1_IRQ_TIMER1			(76)
#define D1_IRQ_LRADC			(77)
#define D1_IRQ_TPADC			(78)
#define D1_IRQ_WATCHDOG			(79)
#define D1_IRQ_IOMMU			(80)
#define D1_IRQ_VE				(82)
#define D1_IRQ_GPIOB_NS			(85)
#define D1_IRQ_GPIOC_NS			(87)
#define D1_IRQ_GPIOD_NS			(89)
#define D1_IRQ_GPIOE_NS			(91)
#define D1_IRQ_GPIOF_NS			(93)
#define D1_IRQ_GPIOG_NS			(95)
#define D1_IRQ_DE				(103)
#define D1_IRQ_DI				(104)
#define D1_IRQ_G2D				(105)
#define D1_IRQ_LCD				(106)
#define D1_IRQ_TV				(107)
#define D1_IRQ_DSI				(108)
#define D1_IRQ_HDMI				(109)
#define D1_IRQ_TVE				(110)
#define D1_IRQ_CSI_DMA0			(111)
#define D1_IRQ_CSI_DMA1			(112)
#define D1_IRQ_CSI_PARSER0		(116)
#define D1_IRQ_CSI_TOP_PKT		(122)
#define D1_IRQ_TVD				(123)
#define D1_IRQ_DSP_DFE			(136)
#define D1_IRQ_DSP_PFE			(137)
#define D1_IRQ_DSP_WDG			(138)
#define D1_IRQ_DSP_MBOX_RISCV_W	(140)
#define D1_IRQ_DSP_TZMA			(141)
#define D1_IRQ_DMAC_IRQ_DSP_NS	(142)
#define D1_IRQ_RISCV_MBOX_RISCV	(144)
#define D1_IRQ_RISCV_MBOX_DSP	(145)
#define D1_IRQ_RISCV_WDG		(147)
#define D1_IRQ_IRRX				(167)
#define D1_IRQ_C0_CTI0			(176)
#define D1_IRQ_C0_CTI1			(177)
#define D1_IRQ_C0_COMMTX0		(180)
#define D1_IRQ_C0_COMMTX1		(181)
#define D1_IRQ_C0_COMMRX0		(184)
#define D1_IRQ_C0_COMMRX1		(185)
#define D1_IRQ_C0_PMU0			(188)
#define D1_IRQ_C0_PMU1			(189)
#define D1_IRQ_C0_AXI_ERROR		(192)
#define D1_IRQ_AXI_WR_IRQ		(194)
#define D1_IRQ_AXI_RD_IRQ		(195)
#define D1_IRQ_DBGWRUPREQ_OUT0	(196)
#define D1_IRQ_DBGWRUPREQ_OUT1	(197)

#ifdef __cplusplus
}
#endif

#endif /* __D1_IRQ_H__ */
