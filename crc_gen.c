#include <stdio.h>
#include <stdint.h>


#define FALSE 0
#define TRUE 1

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


struct CRC_INIT_INFO_T crc_init_table[] = {
        {"CRC_4_ITU",       4,  0x03,       0x00,       0x00,       TRUE,  TRUE},
        {"CRC_5_EPC",       5,  0x09,       0x09,       0x00,       FALSE, FALSE},
        {"CRC_5_ITU",       5,  0x15,       0x00,       0x00,       TRUE,  TRUE},
        {"CRC_5_USB",       5,  0x05,       0x1F,       0x1F,       TRUE,  TRUE},
        {"CRC_6_ITU",       6,  0x05,       0x00,       0x00,       FALSE, FALSE},
        {"CRC_7_MMC",       7,  0x09,       0x00,       0x00,       FALSE, FALSE},
        {"CRC_8",           8,  0x07,       0x00,       0x00,       FALSE, FALSE},
        {"CRC_8_ITU",       8,  0x07,       0x00,       0x55,       FALSE, FALSE},
        {"CRC_8_ROHC",      8,  0x07,       0xFF,       0x00,       TRUE,  TRUE},
        {"CRC_8_MAXIM",     8,  0x9,        0x01,       0x00,       TRUE,  FALSE},
        {"CRC_16_IBM",      16, 0x8005,     0x0000,     0x0000,     TRUE,  TRUE},
        {"CRC_16_MAXIM",    16, 0x8005,     0x0000,     0xFFFF,     TRUE,  TRUE},
        {"CRC_16_USB",      16, 0x8005,     0xFFFF,     0XFFFF,     TRUE,  TRUE},
        {"CRC_16_MODBUS",   16, 0x8005,     0xFFFF,     0x0000,     TRUE,  TRUE},
        {"CRC_16_CCITT",    16, 0x1021,     0x0000,     0x0000,     TRUE,  TRUE},
        {"CRC_16_CCITT_F",  16, 0x1021,     0xFFFF,     0x0000,     FALSE, FALSE},
        {"CRC_16_X25",      16, 0x1021,     0xFFFF,     0xFFFF,     TRUE,  TRUE},
        {"CRC_16_XMODEM",   16, 0x1021,     0x0000,     0x0000,     FALSE, FALSE},
        {"CRC_16_DNP",      16, 0x3D65,     0x0000,     0xFFFF,     TRUE,  TRUE},
        {"CRC_32",          32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, TRUE,  TRUE},
        {"CRC_32_MPEG2",    32, 0x04C11DB7, 0xFFFFFFFF, 0x00000000, FALSE, FALSE},
        {"CRC_SELF_DEFINE", 0,  0,          0,          0, 0, 0},
};

void
set_crc_self_define(uint32_t degree, uint32_t poly, uint32_t init, uint32_t xor_out, uint32_t ref_in, uint32_t ref_out)
{
    crc_init_table[CRC_SELF_DEFINE].drgree = degree;
    crc_init_table[CRC_SELF_DEFINE].poly = poly;
    crc_init_table[CRC_SELF_DEFINE].init = init;
    crc_init_table[CRC_SELF_DEFINE].xor_out = xor_out;
    crc_init_table[CRC_SELF_DEFINE].ref_in = ref_in;
    crc_init_table[CRC_SELF_DEFINE].ref_out = ref_out;
}

uint8_t reverse_byte(uint8_t data)
{
    data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
    data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
    data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
    return data;
}

uint32_t reverse_bits_index(uint32_t data, uint32_t bit_index)
{
    uint32_t mask = bit_index == 31 ? 0xFFFFFFFF : (1 << (bit_index + 1)) - 1;
    uint32_t reversed = 0;

    for (uint32_t i = 0; i <= bit_index; ++i)
    {
        if (data & (1 << i))
        {
            reversed |= (1 << (bit_index - i));
        }
    }

    return (data & ~mask) | (reversed & mask);
}

void init_crc_table(uint32_t *crc_table, uint32_t crc_poly, uint32_t degree)
{
    uint32_t crc_shift = degree - 8;
    uint32_t msb = 1 << (degree - 1);
    uint32_t mask = degree == 32 ? 0xFFFFFFFF : ((1 << degree) - 1);

    if (degree < 8)
    {
        crc_shift = 0;
        msb = 0x80;
        mask = mask << (8 - degree);
        crc_poly = crc_poly << (8 - degree);
    }

    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t crc = i << crc_shift;
        for (uint32_t j = 0; j < 8; j++)
        {
            if (crc & msb)
            {
                crc = (crc << 1) ^ crc_poly;
            }
            else
            {
                crc = crc << 1;
            }
        }
        crc_table[i] = crc & mask;
    }
}

uint32_t crc_gen(const uint8_t *data, size_t length, uint32_t crc_sel)
{
    CRC_INIT_INFO_T crc_info = crc_init_table[crc_sel];
    uint32_t crc = crc_info.init;
    uint32_t degree = crc_info.drgree;
    uint32_t poly = crc_info.poly;
    uint32_t mask = degree == 32 ? 0xFFFFFFFF : ((1 << degree) - 1);
    uint32_t table_shift = degree > 8 ? degree - 8 : 0;
    uint32_t result_shift = degree > 8 ? 0 : 8 - degree;
    uint32_t crc_table[256];
    init_crc_table(crc_table, poly, degree);
    while (length--)
    {
        uint8_t d = crc_info.ref_in ? reverse_byte(*data++) : *data++;
        crc = (crc << 8) ^ crc_table[((crc >> table_shift) ^ d) & 0xFF];
    }

    crc = ((crc >> result_shift) & mask) ^ crc_info.xor_out;
    crc = crc_info.ref_out ? reverse_bits_index(crc, degree - 1) : crc;
    return crc;
}

int main()
{
    uint8_t data[] = {0x55, 0x99};
    size_t data_len = sizeof(data) / sizeof(data[0]);
    uint32_t crc_sel = CRC_SELF_DEFINE;
    set_crc_self_define(32, 0x9, 0xff, 0, 1, 0);
    uint32_t crc = crc_gen(data, data_len, crc_sel);
    printf("CRC: 0x%X\n", crc);
    return 0;
}
