#ifndef CRC_GEN_H
#define CRC_GEN_H

#include <stdint.h>

enum
{
    CRC_4_ITU = 0,
    CRC_5_EPC,
    CRC_5_ITU,
    CRC_5_USB,
    CRC_6_ITU,
    CRC_7_MMC,
    CRC_8,
    CRC_8_ITU,
    CRC_8_ROHC,
    CRC_8_MAXIM,
    CRC_16_IBM,
    CRC_16_MAXIM,
    CRC_16_USB,
    CRC_16_MODBUS,
    CRC_16_CCITT,
    CRC_16_CCITT_F,
    CRC_16_X25,
    CRC_16_XMODEM,
    CRC_16_DNP,
    CRC_32,
    CRC_32_MPEG2,
    CRC_SELF_DEFINE,
};

typedef struct CRC_INIT_INFO_T
{
    char *name;
    uint32_t drgree;
    uint32_t poly;
    uint32_t init;
    uint32_t xor_out;
    uint16_t ref_in;
    uint16_t ref_out;
} CRC_INIT_INFO_T;

#endif //CRC_GEN_H
