#ifndef __MMC_H__
#define __MMC_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /* Class 1 */
    MMC_GO_IDLE_STATE           = 0,
    MMC_SEND_OP_COND            = 1,
    MMC_ALL_SEND_CID            = 2,
    MMC_SET_RELATIVE_ADDR       = 3,
    MMC_SET_DSR                 = 4,
    MMC_SWITCH                  = 6,
    MMC_SELECT_CARD             = 7,
    MMC_SEND_EXT_CSD            = 8,
    MMC_SEND_CSD                = 9,
    MMC_SEND_CID                = 10,
    MMC_READ_DAT_UNTIL_STOP     = 11,
    MMC_STOP_TRANSMISSION       = 12,
    MMC_SEND_STATUS             = 13,
    MMC_GO_INACTIVE_STATE       = 15,
    MMC_SPI_READ_OCR            = 58,
    MMC_SPI_CRC_ON_OFF          = 59,

    /* Class 2 */
    MMC_SET_BLOCKLEN            = 16,
    MMC_READ_SINGLE_BLOCK       = 17,
    MMC_READ_MULTIPLE_BLOCK     = 18,

    /* Class 3 */
    MMC_WRITE_DAT_UNTIL_STOP    = 20,

    /* Class 4 */
    MMC_SET_BLOCK_COUNT         = 23,
    MMC_WRITE_SINGLE_BLOCK      = 24,
    MMC_WRITE_MULTIPLE_BLOCK    = 25,
    MMC_PROGRAM_CID             = 26,
    MMC_PROGRAM_CSD             = 27,

    /* Class 5 */
    MMC_ERASE_GROUP_START       = 35,
    MMC_ERASE_GROUP_END         = 36,
    MMC_ERASE                   = 38,

    /* Class 6 */
    MMC_SET_WRITE_PROT          = 28,
    MMC_CLR_WRITE_PROT          = 29,
    MMC_SEND_WRITE_PROT         = 30,

    /* Class 7 */
    MMC_LOCK_UNLOCK             = 42,

    /* Class 8 */
    MMC_APP_CMD                 = 55,
    MMC_GEN_CMD                 = 56,

    /* Class 9 */
    MMC_FAST_IO                 = 39,
    MMC_GO_IRQ_STATE            = 40,
};

enum {
    SD_CMD_SEND_RELATIVE_ADDR   = 3,
    SD_CMD_SWITCH_FUNC          = 6,
    SD_CMD_SEND_IF_COND         = 8,
    SD_CMD_APP_SET_BUS_WIDTH    = 6,
    SD_CMD_ERASE_WR_BLK_START   = 32,
    SD_CMD_ERASE_WR_BLK_END     = 33,
    SD_CMD_APP_SEND_OP_COND     = 41,
    SD_CMD_APP_SEND_SCR         = 51,
};

enum {
    MMC_RSP_PRESENT = (1 << 0),
    MMC_RSP_136     = (1 << 1),
    MMC_RSP_CRC     = (1 << 2),
    MMC_RSP_BUSY    = (1 << 3),
    MMC_RSP_OPCODE  = (1 << 4),
};

enum {
    MMC_RSP_NONE    = 0,
    MMC_RSP_R1      = (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
    MMC_RSP_R1B     = (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE | MMC_RSP_BUSY),
    MMC_RSP_R2      = (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC),
    MMC_RSP_R3      = (MMC_RSP_PRESENT),
    MMC_RSP_R4      = (MMC_RSP_PRESENT),
    MMC_RSP_R5      = (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
    MMC_RSP_R6      = (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
    MMC_RSP_R7      = (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
};

enum {
    OCR_BUSY        = 0x80000000,
    OCR_HCS         = 0x40000000,
    OCR_VOLTAGE_MASK= 0x00ffff80,
    OCR_ACCESS_MODE = 0x60000000,
};

enum {
    MMC_DATA_READ   = 1,
    MMC_DATA_WRITE  = 2,
};

enum {
    MMC_BUS_WIDTH_1 = 1,
    MMC_BUS_WIDTH_4 = 4,
    MMC_BUS_WIDTH_8 = 8,
};

enum {
    MMC_VDD_165_195 = 0x00000080,
    MMC_VDD_20_21   = 0x00000100,
    MMC_VDD_21_22   = 0x00000200,
    MMC_VDD_22_23   = 0x00000400,
    MMC_VDD_23_24   = 0x00000800,
    MMC_VDD_24_25   = 0x00001000,
    MMC_VDD_25_26   = 0x00002000,
    MMC_VDD_26_27   = 0x00004000,
    MMC_VDD_27_28   = 0x00008000,
    MMC_VDD_28_29   = 0x00010000,
    MMC_VDD_29_30   = 0x00020000,
    MMC_VDD_30_31   = 0x00040000,
    MMC_VDD_31_32   = 0x00080000,
    MMC_VDD_32_33   = 0x00100000,
    MMC_VDD_33_34   = 0x00200000,
    MMC_VDD_34_35   = 0x00400000,
    MMC_VDD_35_36   = 0x00800000,
};

enum{
    MMC_TIMING_LEGACY       = 0,
    MMC_TIMING_MMC_HS       = 1,
    MMC_TIMING_SD_HS        = 2,
    MMC_TIMING_UHS_SDR12    = 3,
    MMC_TIMING_UHS_SDR25    = 4,
    MMC_TIMING_UHS_SDR50    = 5,
    MMC_TIMING_UHS_SDR104   = 6,
    MMC_TIMING_UHS_DDR50    = 7,
    MMC_TIMING_MMC_HS200    = 8,
};

enum {
    SD_VERSION_SD       = 0x20000,
    SD_VERSION_3        = (SD_VERSION_SD | 0x300),
    SD_VERSION_2        = (SD_VERSION_SD | 0x200),
    SD_VERSION_1_0      = (SD_VERSION_SD | 0x100),
    SD_VERSION_1_10     = (SD_VERSION_SD | 0x10a),
    MMC_VERSION_MMC     = 0x10000,
    MMC_VERSION_UNKNOWN = (MMC_VERSION_MMC),
    MMC_VERSION_1_2     = (MMC_VERSION_MMC | 0x102),
    MMC_VERSION_1_4     = (MMC_VERSION_MMC | 0x104),
    MMC_VERSION_2_2     = (MMC_VERSION_MMC | 0x202),
    MMC_VERSION_3       = (MMC_VERSION_MMC | 0x300),
    MMC_VERSION_4       = (MMC_VERSION_MMC | 0x400),
    MMC_VERSION_4_1     = (MMC_VERSION_MMC | 0x401),
    MMC_VERSION_4_2     = (MMC_VERSION_MMC | 0x402),
    MMC_VERSION_4_3     = (MMC_VERSION_MMC | 0x403),
    MMC_VERSION_4_41    = (MMC_VERSION_MMC | 0x429),
    MMC_VERSION_4_5     = (MMC_VERSION_MMC | 0x405),
    MMC_VERSION_5_0     = (MMC_VERSION_MMC | 0x500),
    MMC_VERSION_5_1     = (MMC_VERSION_MMC | 0x501),
};
#define IS_SD(x)        ((x)->version & SD_VERSION_SD)
#define IS_MMC(x)       ((x)->version & MMC_VERSION_MMC)

#ifdef __cplusplus
}
#endif

#endif /* __MMC_H__ */
