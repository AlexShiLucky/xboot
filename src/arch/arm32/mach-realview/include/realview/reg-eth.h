#ifndef __REALVIEW_REG_ETH_H__
#define __REALVIEW_REG_ETH_H__

#include <xboot.h>

#define REALVIEW_ETH_RX_DATA_FIFO           (0x4e000000 + 0x00)
#define REALVIEW_ETH_TX_DATA_FIFO           (0x4e000000 + 0x20)
#define REALVIEW_ETH_RX_STATUS_FIFO         (0x4e000000 + 0x40)
#define REALVIEW_ETH_TX_STATUS_FIFO         (0x4e000000 + 0x48)

#define REALVIEW_ETH_ID_REV                 (0x4e000000 + 0x50)
#define REALVIEW_ETH_BYTE_TEST              (0x4e000000 + 0x64)
#define REALVIEW_ETH_FIFO_INT               (0x4e000000 + 0x68)
#define REALVIEW_ETH_RX_CFG                 (0x4e000000 + 0x6c)
#define REALVIEW_ETH_TX_CFG                 (0x4e000000 + 0x70)

#define REALVIEW_ETH_HW_CFG                 (0x4e000000 + 0x74)

#define REALVIEW_ETH_RX_DP_CTRL             (0x4e000000 + 0x78)

#define REALVIEW_ETH_RX_FIFO_INF            (0x4e000000 + 0x7c)
#define REALVIEW_ETH_TX_FIFO_INF            (0x4e000000 + 0x80)
#define REALVIEW_ETH_PMT_CTRL               (0x4e000000 + 0x84)

#define REALVIEW_ETH_MAC_CSR_CMD            (0x4e000000 + 0xa4)
#define REALVIEW_ETH_MAC_CSR_DATA           (0x4e000000 + 0xa8)

#define REALVIEW_ETH_AFC_CFG                (0x4e000000 + 0xac)

#endif /* __REALVIEW_REG_ETH_H__ */
