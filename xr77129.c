/**
 * @file xr77129.c
 *
 *
 * @brief XR77129 interface implementation
 *
 * @ingroup XR7129
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"

/* Project Includes */
#include "xr77129.h"

/* data for EXARS on AMC [0] and RTM [1] */
static xr77129_data_t xr77129_data[2];

// example of flash cfg config
// IIRC flash addresses goes from 0 to size of the binary
// and the runtime are different, so there I used tuple with (addr, value)
// easy to convert tho
const uint8_t xr77129_flash_config[] = {
	    0xFF,   0xC5,   0x00,   0x0A,   0x03,   0x41,   0x00,   0xFA,   0x02,   0xDA,   0x20,   0x27,   0xE1,   0x28,   0x33,   0x0D,
	    0x00,   0x00,   0x03,   0x00,   0x4D,   0x00,   0x10,   0x02,   0x15,   0xEB,   0x50,   0x2D,   0x2F,   0x3C,   0x13,   0x01,
	    0x01,   0x01,   0x01,   0x01,   0x01,   0x1E,   0x1E,   0xCE,   0x04,   0xB0,   0x0D,   0x00,   0x40,   0x00,   0x40,   0xCB,
	    0x00,   0x29,   0x60,   0x1E,   0x1E,   0x16,   0x45,   0x55,   0xDE,   0x13,   0xED,   0x20,   0x00,   0x01,   0x18,   0x17,
	    0xFF,   0xC5,   0x00,   0x0A,   0x03,   0x41,   0x00,   0xFA,   0x02,   0xDA,   0x20,   0x27,   0xE1,   0x28,   0x33,   0x0D,
	    0x00,   0x00,   0x04,   0x00,   0x66,   0x00,   0x15,   0x02,   0x19,   0xE7,   0x42,   0x2C,   0x2F,   0x3C,   0x1A,   0x80,
	    0x80,   0x80,   0x80,   0x80,   0x80,   0x1E,   0x1E,   0xCE,   0x04,   0xB0,   0x0D,   0x00,   0x40,   0x00,   0x40,   0xCB,
	    0x00,   0x2A,   0x80,   0x1E,   0x1E,   0x16,   0x63,   0x55,   0xA4,   0x14,   0x08,   0x20,   0x00,   0x01,   0x18,   0xB9,
	    0xFF,   0xC5,   0x00,   0x0A,   0x03,   0x41,   0x00,   0xFA,   0x02,   0xDA,   0x20,   0x13,   0xE1,   0x14,   0x33,   0x0D,
	    0x00,   0x00,   0x04,   0x00,   0x5D,   0x00,   0x13,   0x02,   0x19,   0xE7,   0x46,   0x2B,   0x2E,   0x3C,   0x17,   0x08,
	    0x08,   0x08,   0x08,   0x08,   0x08,   0x1D,   0x1E,   0xCE,   0x04,   0xB0,   0x0D,   0x00,   0x40,   0x00,   0x40,   0xCB,
	    0x00,   0x61,   0x74,   0x3D,   0x3D,   0x16,   0xB9,   0x55,   0x01,   0x14,   0x55,   0x20,   0x00,   0x12,   0x28,   0x23,
	    0xFF,   0xC5,   0x00,   0x0A,   0x03,   0x41,   0x00,   0xFA,   0x02,   0xDA,   0x20,   0x09,   0xE1,   0x0A,   0x19,   0x07,
	    0x00,   0x00,   0x03,   0x00,   0x50,   0x00,   0x11,   0x02,   0x15,   0xEB,   0x4E,   0x38,   0x3D,   0x3C,   0x14,   0x22,
	    0x22,   0x02,   0x02,   0x22,   0x22,   0x1E,   0x1E,   0x66,   0x04,   0xB0,   0x0D,   0x00,   0x40,   0x00,   0x40,   0x63,
	    0x00,   0x6B,   0x64,   0xF7,   0xF7,   0x13,   0x23,   0x5B,   0x54,   0x11,   0x90,   0x21,   0x7F,   0x23,   0x38,   0x9D,
	    0x05,   0x00,   0x00,   0x46,   0x48,   0x48,   0x1E,   0x1C,   0x00,   0x30,   0x01,   0x9F,   0x95,   0x44,   0x44,   0x00,
	    0x10,   0x16,   0x04,   0x0A,   0x10,   0x00,   0x00,   0x00,   0x00,   0x20,   0x0F,   0x17,   0x10,   0x17,   0x0F,   0x60,
	    0x80,   0x74,   0x64,   0x30,   0x30,   0x18,   0x0C,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0xFF,
	    0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x0F,   0x00,   0x00,   0x00,   0x16,
	    0x00,   0x00,   0x00,   0x02,   0x00,   0x00,   0x37,   0x00,   0x00,   0x0F,   0x02,   0x02,   0x02,   0x02,   0x09,   0x09,
	    0x09,   0x09,   0x00,   0x00,   0x00,   0x00,   0x0F,   0x00,   0x05,   0x00,   0x00,   0x00,   0x00,   0x01,   0x00,   0x00,
	    0x00,   0x00,   0x00,   0x04,   0x04,   0x00,   0x00,   0x00,   0x62,   0x62,   0x62,   0x62,   0x62,   0x00,   0x00,   0x62,
	    0x01,   0x62,   0xFA,   0x00,   0x80,   0x00,   0x00,   0xFF,   0x12,   0x02,   0xFF,   0x00,   0x00,   0x00,   0x00,   0x2A,
	    0x00,   0x02,   0x52,   0x00,   0x00,   0xFF,   0xFF,   0x64,   0x00,   0xC8,   0x04,   0xC8,   0x02,   0xFA,   0x06,   0x00,
	    0x09,   0x00,   0x00,   0x00,   0x00,   0x00,   0x02,   0x00,   0x04,   0x00,   0x06,   0x00,   0x09,   0x00,   0x00,   0x64,
	    0x09,   0x64,   0x64,   0x64,   0x09,   0x64,   0x64,   0x64,   0x09,   0x64,   0x64,   0x64,   0x09,   0x64,   0x64,   0x0A,
	    0x20,   0x0A,   0x05,   0x19,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0xFF,   0x00,   0x04,   0xFF,   0xFF,   0x93
};

// example exar runtime config DO NOT USE FOR YOUR BOARD
xr_data xr77129_afcz_exar1_runtime_p5_cfg[434] = { { 0x8072, 0x00 }, { 0x8073, 0x02 }, { 0x8074, 0x50 }, { 0x8075, 0x00 }, { 0x8076, 0x00 }, { 0x8077, 0xFF }, { 0x8078, 0xFF }, { 0x8079, 0x00 }, { 0x807a, 0x00 }, { 0x807b, 0x05 }, { 0x807c, 0x02 }, { 0x807d, 0x05 }, { 0x807e, 0x04 }, { 0x807f, 0x05 }, { 0x8080, 0x06 }, { 0x8081, 0x00 }, { 0x8082, 0x09 }, { 0x8083, 0x00 }, { 0x8084, 0x00 }, { 0x8085, 0x00 }, { 0x8086, 0x00 }, { 0x8087, 0x00 }, { 0x8088, 0x02 }, { 0x8089, 0x00 }, { 0x808a, 0x04 }, { 0x808b, 0x00 }, { 0x808c, 0x06 }, { 0x808d, 0x00 }, { 0x808e, 0x09 }, { 0x808f, 0x00 }, { 0x8090, 0x00 }, { 0x8091, 0x64 }, { 0x8092, 0x28 }, { 0x8093, 0x64 }, { 0x8094, 0x64 }, { 0x8095, 0x64 }, { 0x8096, 0x28 }, { 0x8097, 0x64 }, { 0x8098, 0x64 }, { 0x8099, 0x64 }, { 0x809a, 0x28 }, { 0x809b, 0x64 }, { 0x809c, 0x64 }, { 0x809d, 0x64 }, { 0x809e, 0x28 }, { 0x809f, 0x64 }, { 0x80a0, 0x64 }, { 0x80a1, 0x0A }, { 0x80a2, 0x20 }, { 0x80a3, 0x0A }, { 0x80a4, 0x05 }, { 0x80a5, 0x19 }, { 0x80a6, 0xFF }, { 0x80a7, 0xFF }, { 0x80a8, 0x00 }, { 0x80a9, 0xFF }, { 0x80aa, 0x01 }, { 0x80ab, 0x03 }, { 0x80ac, 0xFF }, { 0x80ae, 0x04 }, { 0x80af, 0xFF }, { 0x80b0, 0xFF }, { 0x80b1, 0x44 }, { 0x80b2, 0x60 }, { 0x80b3, 0x60 }, { 0x80b4, 0x48 }, { 0x80b5, 0x30 }, { 0x80b6, 0x30 }, { 0x80b7, 0x30 }, { 0x80b8, 0x30 }, { 0x80b9, 0x00 }, { 0x80ba, 0x00 }, { 0x80bb, 0x00 }, { 0x80bc, 0x00 }, { 0x80bd, 0x00 }, { 0x80be, 0x00 }, { 0x80bf, 0x00 }, { 0x80c0, 0x00 }, { 0x80c1, 0xFF }, { 0x80c2, 0xFF }, { 0x80c3, 0xFF }, { 0x80c4, 0xFF }, { 0x80c5, 0xFF }, { 0x80c6, 0xFF }, { 0x80c7, 0xFF }, { 0x80c8, 0xFF }, { 0x80c9, 0xFF }, { 0x80ca, 0xFF }, { 0x80cb, 0xFF }, { 0x80cc, 0xFF }, { 0x80cd, 0x0F }, { 0x80ce, 0x00 }, { 0x80cf, 0x00 }, { 0x80d0, 0x00 }, { 0x80d1, 0x14 }, { 0xc000, 0xFF }, { 0xc001, 0xC5 }, { 0xc002, 0x00 }, { 0xc003, 0x0A }, { 0xc004, 0x03 }, { 0xc005, 0x41 }, { 0xc006, 0x00 }, { 0xc007, 0xFA }, { 0xc008, 0x02 }, { 0xc009, 0xDA }, { 0xc00a, 0x20 }, { 0xc00b, 0x27 }, { 0xc00c, 0xE1 }, { 0xc00d, 0x28 }, { 0xc00e, 0x33 }, { 0xc00f, 0x4D }, { 0xc010, 0x00 }, { 0xc011, 0x70 }, { 0xc012, 0x02 }, { 0xc013, 0xF0 }, { 0xc014, 0x36 }, { 0xc015, 0x00 }, { 0xc016, 0x0B }, { 0xc017, 0x02 }, { 0xc018, 0x0D }, { 0xc019, 0xF3 }, { 0xc01a, 0x5E }, { 0xc01b, 0x69 }, { 0xc01c, 0x7A }, { 0xc01d, 0x78 }, { 0xc01e, 0x14 }, { 0xc01f, 0x01 }, { 0xc020, 0x01 }, { 0xc021, 0x01 }, { 0xc022, 0x01 }, { 0xc023, 0x01 }, { 0xc024, 0x01 }, { 0xc025, 0x17 }, { 0xc026, 0x1E }, { 0xc027, 0xCE }, { 0xc028, 0x04 }, { 0xc029, 0xB0 }, { 0xc02a, 0x0D }, { 0xc02b, 0x00 }, { 0xc02c, 0x40 }, { 0xc02d, 0x00 }, { 0xc02e, 0x40 }, { 0xc02f, 0xCB }, { 0xc030, 0x00 }, { 0xc031, 0x2C }, { 0xc032, 0x44 }, { 0xc033, 0x1E }, { 0xc034, 0x1E }, { 0xc035, 0x1D }, { 0xc036, 0xB8 }, { 0xc037, 0x47 }, { 0xc038, 0x9A }, { 0xc039, 0x1A }, { 0xc03a, 0xC0 }, { 0xc03b, 0x2B }, { 0xc03c, 0x75 }, { 0xc03d, 0x00 }, { 0xc03e, 0x08 }, { 0xc100, 0xFF }, { 0xc101, 0xC5 }, { 0xc102, 0x00 }, { 0xc103, 0x0A }, { 0xc104, 0x03 }, { 0xc105, 0x41 }, { 0xc106, 0x00 }, { 0xc107, 0xFA }, { 0xc108, 0x02 }, { 0xc109, 0xDA }, { 0xc10a, 0x20 }, { 0xc10b, 0x27 }, { 0xc10c, 0xE1 }, { 0xc10d, 0x28 }, { 0xc10e, 0x33 }, { 0xc10f, 0x4D }, { 0xc110, 0x00 }, { 0xc111, 0x70 }, { 0xc112, 0x03 }, { 0xc113, 0xF0 }, { 0xc114, 0x4D }, { 0xc115, 0x00 }, { 0xc116, 0x10 }, { 0xc117, 0x02 }, { 0xc118, 0x15 }, { 0xc119, 0xEB }, { 0xc11a, 0x50 }, { 0xc11b, 0x2B }, { 0xc11c, 0x2E }, { 0xc11d, 0x78 }, { 0xc11e, 0x13 }, { 0xc11f, 0x04 }, { 0xc120, 0x04 }, { 0xc121, 0x04 }, { 0xc122, 0x04 }, { 0xc123, 0x04 }, { 0xc124, 0x04 }, { 0xc125, 0x1E }, { 0xc126, 0x1E }, { 0xc127, 0xCE }, { 0xc128, 0x04 }, { 0xc129, 0xB0 }, { 0xc12a, 0x0D }, { 0xc12b, 0x00 }, { 0xc12c, 0x40 }, { 0xc12d, 0x00 }, { 0xc12e, 0x40 }, { 0xc12f, 0xCB }, { 0xc130, 0x00 }, { 0xc131, 0x13 }, { 0xc132, 0x60 }, { 0xc133, 0x1E }, { 0xc134, 0x1E }, { 0xc135, 0x15 }, { 0xc136, 0x2D }, { 0xc137, 0x58 }, { 0xc138, 0xE1 }, { 0xc139, 0x12 }, { 0xc13a, 0x0F }, { 0xc13b, 0x20 }, { 0xc13c, 0x00 }, { 0xc13d, 0x00 }, { 0xc13e, 0x08 }, { 0xc200, 0xFF }, { 0xc201, 0xC5 }, { 0xc202, 0x00 }, { 0xc203, 0x0A }, { 0xc204, 0x03 }, { 0xc205, 0x41 }, { 0xc206, 0x00 }, { 0xc207, 0xFA }, { 0xc208, 0x02 }, { 0xc209, 0xDA }, { 0xc20a, 0x20 }, { 0xc20b, 0x27 }, { 0xc20c, 0xE1 }, { 0xc20d, 0x28 }, { 0xc20e, 0x33 }, { 0xc20f, 0x4D }, { 0xc210, 0x00 }, { 0xc211, 0x70 }, { 0xc212, 0x03 }, { 0xc213, 0xF0 }, { 0xc214, 0x4D }, { 0xc215, 0x00 }, { 0xc216, 0x10 }, { 0xc217, 0x02 }, { 0xc218, 0x15 }, { 0xc219, 0xEB }, { 0xc21a, 0x50 }, { 0xc21b, 0x27 }, { 0xc21c, 0x28 }, { 0xc21d, 0x78 }, { 0xc21e, 0x13 }, { 0xc21f, 0x08 }, { 0xc220, 0x08 }, { 0xc221, 0x08 }, { 0xc222, 0x08 }, { 0xc223, 0x08 }, { 0xc224, 0x08 }, { 0xc225, 0x1E }, { 0xc226, 0x1E }, { 0xc227, 0xCE }, { 0xc228, 0x04 }, { 0xc229, 0xB0 }, { 0xc22a, 0x0D }, { 0xc22b, 0x00 }, { 0xc22c, 0x40 }, { 0xc22d, 0x00 }, { 0xc22e, 0x40 }, { 0xc22f, 0xCB }, { 0xc230, 0x00 }, { 0xc231, 0x13 }, { 0xc232, 0x60 }, { 0xc233, 0x1E }, { 0xc234, 0x1E }, { 0xc235, 0x15 }, { 0xc236, 0x2D }, { 0xc237, 0x58 }, { 0xc238, 0xE2 }, { 0xc239, 0x12 }, { 0xc23a, 0x0E }, { 0xc23b, 0x20 }, { 0xc23c, 0x00 }, { 0xc23d, 0x00 }, { 0xc23e, 0x08 }, { 0xc300, 0xFF }, { 0xc301, 0xC5 }, { 0xc302, 0x00 }, { 0xc303, 0x0A }, { 0xc304, 0x03 }, { 0xc305, 0x41 }, { 0xc306, 0x00 }, { 0xc307, 0xFA }, { 0xc308, 0x02 }, { 0xc309, 0xDA }, { 0xc30a, 0x20 }, { 0xc30b, 0x27 }, { 0xc30c, 0xE1 }, { 0xc30d, 0x28 }, { 0xc30e, 0x33 }, { 0xc30f, 0x4D }, { 0xc310, 0x00 }, { 0xc311, 0x70 }, { 0xc312, 0x02 }, { 0xc313, 0xF0 }, { 0xc314, 0x3A }, { 0xc315, 0x00 }, { 0xc316, 0x0C }, { 0xc317, 0x02 }, { 0xc318, 0x11 }, { 0xc319, 0xEF }, { 0xc31a, 0x5C }, { 0xc31b, 0x27 }, { 0xc31c, 0x29 }, { 0xc31d, 0x78 }, { 0xc31e, 0x0E }, { 0xc31f, 0x40 }, { 0xc320, 0x40 }, { 0xc321, 0x40 }, { 0xc322, 0x40 }, { 0xc323, 0x40 }, { 0xc324, 0x40 }, { 0xc325, 0x1E }, { 0xc326, 0x1E }, { 0xc327, 0xCE }, { 0xc328, 0x04 }, { 0xc329, 0xB0 }, { 0xc32a, 0x0D }, { 0xc32b, 0x00 }, { 0xc32c, 0x40 }, { 0xc32d, 0x00 }, { 0xc32e, 0x40 }, { 0xc32f, 0xCB }, { 0xc330, 0x00 }, { 0xc331, 0x13 }, { 0xc332, 0x48 }, { 0xc333, 0x1E }, { 0xc334, 0x1E }, { 0xc335, 0x15 }, { 0xc336, 0x1A }, { 0xc337, 0x59 }, { 0xc338, 0x04 }, { 0xc339, 0x11 }, { 0xc33a, 0xFE }, { 0xc33b, 0x20 }, { 0xc33c, 0x00 }, { 0xc33d, 0x00 }, { 0xc33e, 0x08 }, { 0xc400, 0x05 }, { 0xc401, 0x00 }, { 0xc402, 0x00 }, { 0xc403, 0x4C }, { 0xc404, 0x4F }, { 0xc405, 0x4C }, { 0xc406, 0x0F }, { 0xc407, 0x0E }, { 0xc408, 0x00 }, { 0xc409, 0x30 }, { 0xc40a, 0x01 }, { 0xc40b, 0x9F }, { 0xc40c, 0x55 }, { 0xc40d, 0x02 }, { 0xc40e, 0x02 }, { 0xc40f, 0x00 }, { 0xc410, 0x10 }, { 0xc411, 0x16 }, { 0xc412, 0x04 }, { 0xc413, 0x0A }, { 0xc414, 0x10 }, { 0xc415, 0x00 }, { 0xc416, 0x00 }, { 0xc417, 0x00 }, { 0xc418, 0x00 }, { 0xc419, 0x20 }, { 0xc41a, 0x0F }, { 0xc41b, 0x17 }, { 0xc41c, 0x10 }, { 0xc41d, 0x17 }, { 0xc41e, 0x0F }, { 0xd001, 0x00 }, { 0xd002, 0x00 }, { 0xd003, 0x02 }, { 0xd004, 0x00 }, { 0xd005, 0x00 }, { 0xd006, 0x37 }, { 0xd007, 0x00 }, { 0xd009, 0x0F }, { 0xd00a, 0x03 }, { 0xd00b, 0x03 }, { 0xd00c, 0x03 }, { 0xd00d, 0x03 }, { 0xd00e, 0x0A }, { 0xd00f, 0x09 }, { 0xd010, 0x09 }, { 0xd011, 0x09 }, { 0xd012, 0x00 }, { 0xd013, 0x00 }, { 0xd014, 0x00 }, { 0xd015, 0x00 }, { 0xd016, 0x0F }, { 0xd017, 0x00 }, { 0xd018, 0x02 }, { 0xd019, 0x00 }, { 0xd01a, 0x00 }, { 0xd01b, 0x00 }, { 0xd01c, 0x00 }, { 0xd01d, 0x01 }, { 0xd01e, 0x00 }, { 0xd01f, 0x00 }, { 0xd020, 0x00 }, { 0xd021, 0x00 }, { 0xd022, 0x00 }, { 0xd023, 0x04 }, { 0xd024, 0x04 }, { 0xd025, 0x00 }, { 0xd026, 0x00 }, { 0xd027, 0x00 }, { 0xd302, 0x62 }, { 0xd303, 0x62 }, { 0xd304, 0x61 }, { 0xd305, 0x62 }, { 0xd306, 0x61 }, { 0xd308, 0x02 }, { 0xd900, 0x62 }, { 0xd901, 0x01 }, { 0xd902, 0x62 }, { 0xd903, 0xFA }, { 0xffa4, 0x80 }, { 0xffa6, 0x00 }, { 0xffa9, 0x00 }, { 0xffab, 0x0F }, { 0xffad, 0x12 }, { 0xffaf, 0x02 }, { 0xffb2, 0xEB }, { 0xffdc, 0x00 } };

void xr77129_init ()
{
	xr77129_data[0].chipid = CHIP_ID_XR77129;

	xr77129_data[0].status_regs_addr[HOST_STS_REG]	 = XR77129_GET_HOST_STS;
	xr77129_data[0].status_regs_addr[FAULT_STS_REG]  = XR77129_GET_FAULT_STS;
	xr77129_data[0].status_regs_addr[PWR_STATUS_REG] = XR77129_PWR_GET_STATUS;
	xr77129_data[0].status_regs_addr[PWR_CHIP_READY] = XR77129_CHIP_READY;
	xr77129_data[0].status_regs_addr[GPIO_STATE] 	 = XR77129_GPIO_READ_GPIO;

	/* Check if there is valid configuration in xr77128 flash */
//	if ( ! xr77129_flash_verify( &xr77129_data[0], xr77129_flash_config, sizeof(xr77129_flash_config)/sizeof(xr77129_flash_config[0]) ) )
//	{
//		xr77129_flash_load ( &xr77129_data[0], xr77129_flash_config, sizeof(xr77129_flash_config)/sizeof(xr77129_flash_config[0]) );
//	}

	xTaskCreate( vTaskXR77129, "XR77129", 100, (void *) NULL, tskXR77129_PRIORITY, &vTaskXR77129_Handle);
}


void vTaskXR77129( void *Parameters )
{
	/* Power Good flags */
	uint8_t xr77129_PowerGood = 0;
	uint8_t P0V95_PowerGood = 0;
	uint8_t P1V2_PowerGood = 0;

    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = XR77129_UPDATE_RATE / portTICK_PERIOD_MS;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
    	/* Payload power good flag */
    	if (gpio_read_pin( PIN_PORT(GPIO_P12V0_OK), PIN_NUMBER(GPIO_P12V0_OK)))
    	{
			/* Read power status of xr77129 outputs, P0V95 and P1V2 */
			xr77129_read_status( &xr77129_data[0] );

			if ( xr77129_data[0].status_regs[PWR_STATUS_REG] == XR77129_POWER_OK ) {
				xr77129_PowerGood = 1;
			} else {
				xr77129_PowerGood = 0;
			}

			if ( xr77129_data[0].status_regs[GPIO_STATE] & XR77129_GPIO0 ) {
				P0V95_PowerGood = 1;
			} else {
				P0V95_PowerGood = 0;
			}

			if ( xr77129_data[0].status_regs[GPIO_STATE] & XR77129_PSIO1 ) {
				P1V2_PowerGood = 1;
			} else {
				P1V2_PowerGood = 0;
			}

			if ( xr77129_PowerGood && P0V95_PowerGood && P1V2_PowerGood ) {
				/* Let Payload Task know that Power is OK */
				payload_send_message( FRU_AMC, PAYLOAD_MESSAGE_DCDC_PGOOD );
			} else {
				payload_send_message( FRU_AMC, PAYLOAD_MESSAGE_DCDC_PGOODn );
			}
    	}

    	vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

uint8_t xr77129_flash_load ( xr77129_data_t * data, const uint8_t * xr77129_config, uint32_t size )
{

	xr77129_flash_erase ( data );

	xr77129_flash_program ( data, xr77129_config, size );

	xr77129_flash_verify ( data, xr77129_config, size );

	return 0;
}


uint8_t xr77129_flash_erase ( xr77129_data_t * data )
{
	uint8_t page;

	for ( page=0 ; page <= 6 ; page++)
	{
		xr77129_flash_page_clear(data, page);
	}

	for ( page=0 ; page <= 6 ; page++)
	{
		xr77129_flash_page_erase(data, page);
	}

	return 0;
}

uint8_t xr77129_flash_page_clear ( xr77129_data_t * data, uint8_t page )
{
	uint16_t flash_enable_clear = 0x0001;
	xr77129_write_value( data, XR77129_FLASH_INIT, flash_enable_clear);

	uint16_t page_clear = page;
	xr77129_write_value( data, XR77129_FLASH_PAGE_CLEAR, page_clear);

	uint16_t page_clear_status = 0x01;
	while ( page_clear_status & 0xFF )
	{
		/* TODO: timeout */
		xr77129_read_value( data, XR77129_FLASH_PAGE_CLEAR, &page_clear_status );
	}

	return 0;
}

uint8_t xr77129_flash_page_erase (xr77129_data_t * data, uint8_t page )
{
	uint16_t init_page_erase = 0x0005;
	xr77129_write_value( data, XR77129_FLASH_INIT, init_page_erase);

	uint16_t page_erase = page;
	xr77129_write_value( data, XR77129_FLASH_PAGE_ERASE, page_erase );

	uint16_t page_erase_status = 0x01;
	while ( page_erase_status & 0xFF )
	{
		/* TODO: timeout */
		xr77129_read_value( data, XR77129_FLASH_PAGE_ERASE, &page_erase_status );
	}

	return 0;
}


uint8_t xr77129_flash_program ( xr77129_data_t * data, const uint8_t * flash_data, uint32_t size )
{
	/* Enable flash program */
	uint16_t page_clear = 0x0001;
	xr77129_write_value( data, XR77129_FLASH_INIT, page_clear);

	/* set flash address pointer */
	uint16_t flash_program_address = 0x0000;
	xr77129_write_value( data, XR77129_FLASH_PROGRAM_ADDRESS, flash_program_address);


	/* program data */
	uint32_t i;
	uint16_t val;

	for ( i=0 ; i < size - 1 ; i+=2 )
	{
		val = (flash_data[1] << 8) | flash_data[i+1] ;
		xr77129_write_value( data, XR77129_FLASH_PROGRAM_DATA_INC_ADDRESS, val);
	}

	return 0;
}

uint8_t xr77129_flash_verify ( xr77129_data_t * data, const uint8_t * flash_data, uint32_t size )
{
	uint8_t i2c_interf, i2c_addr;

	uint8_t temp_flash [ size ];
	uint8_t data_address = XR77129_FLASH_PROGRAM_DATA_INC_ADDRESS;

	uint8_t rx[2];
	uint8_t i = 0x00;
	for ( ; i < size-1 ; i+=2 )
	{
		if( i2c_take_by_chipid( data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

			xI2CMasterWriteRead( i2c_interf, i2c_addr, data_address, rx, sizeof(rx)/sizeof(rx[0]) );

			i2c_give( i2c_interf );
			temp_flash[i] = rx[0];
			temp_flash[i+1] = rx[1];
		}
	}

	for ( i=0 ; i < size ; i++)
	{
		if (temp_flash[i] != flash_data[i])
		{
			return 0;
		}
	}

	return 1;
}



void xr77129_read_status( xr77129_data_t * data )
{
	uint8_t reg;
	for (reg = HOST_STS_REG ; reg < XR77129_STATUS_REGISTERS_COUNT ; reg++)
	{
		xr77129_read_value( data, data->status_regs_addr[reg], &(data->status_regs[reg]) );
	}
}

void xr77129_load_runtimes(void)
{
	// exar 1 na 5 muxie
	// exar 2 na 4 muxie
//	vTaskDelay(100);

//	i2c_tcaselect(5);

//	if (!xr77129_get_power_chip_ready()) {
//		printf("Ready 0, programming\r\n");

	i2c_tcaselect(5);
	printf("load 5 %d\r\n", xr77129_runtime_load(xr77129_afcz_exar1_runtime_p5_cfg, 430));

	i2c_tcaselect(4);
	printf("load 4 %d\r\n", xr77129_runtime_load(xr77129_afcz_exar2_runtime_p4_cfg, 431));
//	}
}

void xr77129_read_value(uint8_t reg_address, uint16_t *read )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t val[2] = { 0 };
    uint8_t buff[2] = { 0 };
    I2C_M_SETUP_Type xfer = { 0 };
    uint8_t retry_count = 10;

    buff[0] = reg_address;

    xfer.sl_addr7bit = 0x28;
	xfer.tx_data = buff;
	xfer.tx_length = 1;
	xfer.rx_data = val;
	xfer.rx_length = 2;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("rt\r\n");break;
		}
	}

	*read = (val[0] << 8) | (val[1]);
}

void xr77129_write_value(uint8_t reg_address, uint16_t value )
{
	uint8_t tx[3] = {
		reg_address,
		(value & 0xFF00) >> 8,
		value & 0xFF
	};

    I2C_M_SETUP_Type xfer = { 0 };
    uint8_t retry_count = 10;

    xfer.sl_addr7bit = 0x28;
	xfer.tx_data = tx;
	xfer.tx_length = 3;
	xfer.rx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("wt\r\n");
			break;
		}
	}
}

void xr77129_runtime_write(uint16_t address, uint8_t data)
{
	uint8_t lb = (address & 0xFF);
	uint8_t hb = (address & 0xFF00) >> 8;
	I2C_M_SETUP_Type xfer = { 0 };
	uint8_t retry_count = 10;

	uint8_t payload[3] = {
		hb,
		lb,
		data
	};

	xfer.sl_addr7bit = 0x28;
	xfer.tx_data = payload;
	xfer.tx_length = 3;
	xfer.rx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("wt\r\n");break;
		}
	}
}

void xr77129_runtime_read(uint16_t address, uint8_t * data)
{
	uint8_t lb = (address & 0xFF);
	uint8_t hb = (address & 0xFF00) >> 8;
	I2C_M_SETUP_Type xfer = { 0 };
	uint8_t retry_count = 10;

	uint8_t payload[3] = {
		hb,
		lb
	};

	xfer.sl_addr7bit = 0x28;
	xfer.tx_data = payload;
	xfer.tx_length = 2;
	xfer.rx_data = data;
	xfer.rx_length = 1;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("rrrrrrrrt\r\n");break;
		}
	}
}

void xr77129_set_power_chip_ready(uint8_t val)
{
	xr77129_write_value(0x0E, val);
}

uint16_t xr77129_get_power_chip_ready(void)
{
	uint16_t val = 0xFF;
	xr77129_read_value(0x0E, &val);

	return (uint8_t) val;
}

void xr77129_power_restart(void)
{
	xr77129_write_value(0x0F, 0x0F00);
}

uint8_t xr77129_runtime_load(xr_data * exar_data, uint32_t len)
{
	xr77129_power_restart();
	delay_ms(200);

	uint16_t revid = { 0 };
	xr77129_read_value(0x01, &revid);
	if (revid != HW_ID_XR77129) {
		printf("Wrong device ID\r\n");
		return 0;
	}

	xr77129_runtime_write(0x8000, 0x22);
	for(int j = 0; j < 1024; j++);

	uint8_t dummy = 0xFF;
	xr77129_runtime_read(0x8000, &dummy);
	if (dummy != 0x22) {
		printf("Saved variable check 1 failed\r\n");
		return 0;
	}

	if (xr77129_get_power_chip_ready() == 1) return 0;

	for (int i = 0; i < len; i++)
	{
		// write
		xr77129_runtime_write(exar_data[i].address, exar_data[i].data);
		for(int j = 0; j < 1024; j++);

		// verify
		uint8_t dummy = 0xFF;
		xr77129_runtime_read(exar_data[i].address, &dummy);
		if (dummy != exar_data[i].data) {
			printf("verify failed %d\r\n", i);
			printf("addr %d %d data %d != %d\r\n", i, exar_data[i].address, exar_data[i].data, dummy);
			return 0;
		}
	}

	dummy = 0xFF;
	xr77129_runtime_read(0x8000, &dummy);
	if (dummy != 0x22) {
		printf("Saved variable check 2 failed\r\n");
		return 0;
	}

	xr77129_set_power_chip_ready(0x01);
	delay_ms(1);

	return 1;
}