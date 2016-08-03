
#include <libxml/tree.h>
#include "iconv.h"
#include <libxml/xpath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "xml_read_write.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
/*
#define DEBUG_PRINTF(format, ...) \
        printf(format, __VA_ARGS__);
#define INFO_PRINTF(format, ...) \
        printf(format, __VA_ARGS__);
#define ERROR_PRINTF(format, ...) \
        printf(format, __VA_ARGS__);

#define DEBUG_PRINT(str) \
        printf("%s", str);
#define INFO_PRINT(str) \
        printf("%s", str);
#define ERROR_PRINT(str) \
        printf("%s", str);
*/

#define ENCODE_TYPE     "UTF-8"


//#define DVB_DESC_AAC          0x7c	
//#define DVB_DESC_LOCAL_CHAN   0x83
//	
//
//	
/*
int dvb_get_string(char *dst, size_t dstlen, const uint8_t *src, \
		   const size_t srclen, char *dvb_default_charset);\
		   const size_t srclen, char *dvb_default_charset,dvb_string_conv_t *conv);
	
*/
//int dvb_get_string_with_len(char *dst, size_t dstlen, 
//			    const uint8_t *buf, size_t buflen, char *dvb_default_charset);
//			    const uint8_t *buf, size_t buflen, char *dvb_default_charset,dvb_string_conv_t *conv);
//	
//#define bcdtoint(i) ((((i & 0xf0) >> 4) * 10) + (i & 0x0f))
//
//// Code tables for the variable part of the iso 8859 family (codes 0xa0 to 0xff)
//static uint16_t conv_8859_table[14][96] = {
//  /* ISO-8859-1 */ {
//    /* 0xA0 */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
//    /* 0xB8 */ 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
//    /* 0xC0 */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
//    /* 0xC8 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
//    /* 0xE0 */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
//    /* 0xE8 */ 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
//  },
//  /* ISO-8859-2 */ {
//    /* 0xA0 */ 0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
//    /* 0xB0 */ 0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
//    /* 0xB8 */ 0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
//    /* 0xC0 */ 0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
//    /* 0xC8 */ 0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
//    /* 0xD0 */ 0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
//    /* 0xE0 */ 0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
//    /* 0xE8 */ 0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
//    /* 0xF0 */ 0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
//  },
//  /* ISO-8859-3 */ {
//    /* 0xA0 */ 0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0x0000, 0x0124, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0x0000, 0x017B,
//    /* 0xB0 */ 0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7,
//    /* 0xB8 */ 0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0x0000, 0x017C,
//    /* 0xC0 */ 0x00C0, 0x00C1, 0x00C2, 0x0000, 0x00C4, 0x010A, 0x0108, 0x00C7,
//    /* 0xC8 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x0000, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF,
//    /* 0xE0 */ 0x00E0, 0x00E1, 0x00E2, 0x0000, 0x00E4, 0x010B, 0x0109, 0x00E7,
//    /* 0xE8 */ 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x0000, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9,
//  },
//  /* ISO-8859-4 */ {
//    /* 0xA0 */ 0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF,
//    /* 0xB0 */ 0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7,
//    /* 0xB8 */ 0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B,
//    /* 0xC0 */ 0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
//    /* 0xC8 */ 0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A,
//    /* 0xD0 */ 0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF,
//    /* 0xE0 */ 0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
//    /* 0xE8 */ 0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B,
//    /* 0xF0 */ 0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9,
//  },
//  /* ISO-8859-5 */ {
//    /* 0xA0 */ 0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
//    /* 0xA8 */ 0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
//    /* 0xB0 */ 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
//    /* 0xB8 */ 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
//    /* 0xC0 */ 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
//    /* 0xC8 */ 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
//    /* 0xD0 */ 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
//    /* 0xD8 */ 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
//    /* 0xE0 */ 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
//    /* 0xE8 */ 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
//    /* 0xF0 */ 0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
//    /* 0xF8 */ 0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F,
//  },
//  /* ISO-8859-6 */ {
//    /* 0xA0 */ 0x00A0, 0x0000, 0x0000, 0x0000, 0x00A4, 0x0000, 0x0000, 0x0000,
//    /* 0xA8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x060C, 0x00AD, 0x0000, 0x0000,
//    /* 0xB0 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* 0xB8 */ 0x0000, 0x0000, 0x0000, 0x061B, 0x0000, 0x0000, 0x0000, 0x061F,
//    /* 0xC0 */ 0x0000, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
//    /* 0xC8 */ 0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
//    /* 0xD0 */ 0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
//    /* 0xD8 */ 0x0638, 0x0639, 0x063A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* 0xE0 */ 0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
//    /* 0xE8 */ 0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
//    /* 0xF0 */ 0x0650, 0x0651, 0x0652,
//  },
//  /* ISO-8859-7 */ {
//    /* 0xA0 */ 0x00A0, 0x2018, 0x2019, 0x00A3, 0x20AC, 0x20AF, 0x00A6, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x00A9, 0x037A, 0x00AB, 0x00AC, 0x00AD, 0x0000, 0x2015,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x00B7,
//    /* 0xB8 */ 0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
//    /* 0xC0 */ 0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
//    /* 0xC8 */ 0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
//    /* 0xD0 */ 0x03A0, 0x03A1, 0x0000, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
//    /* 0xD8 */ 0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
//    /* 0xE0 */ 0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
//    /* 0xE8 */ 0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
//    /* 0xF0 */ 0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
//    /* 0xF8 */ 0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE,
//  },
//  /* ISO-8859-8 */ {
//    /* 0xA0 */ 0x00A0, 0x0000, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
//    /* 0xB8 */ 0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x0000,
//    /* 0xC0 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* 0xC8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* 0xD0 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* 0xD8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2017,
//    /* 0xE0 */ 0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
//    /* 0xE8 */ 0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
//    /* 0xF0 */ 0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
//    /* 0xF8 */ 0x05E8, 0x05E9, 0x05EA, 0x0000, 0x0000, 0x200E, 0x200F,
//  },
//  /* ISO-8859-9 */ {
//    /* 0xA0 */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
//    /* 0xA8 */ 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
//    /* 0xB8 */ 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
//    /* 0xC0 */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
//    /* 0xC8 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
//    /* 0xE0 */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
//    /* 0xE8 */ 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF,
//  },
//  /* ISO-8859-10 */ {
//    /* 0xA0 */ 0x00A0, 0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7,
//    /* 0xA8 */ 0x013B, 0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A,
//    /* 0xB0 */ 0x00B0, 0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7,
//    /* 0xB8 */ 0x013C, 0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B,
//    /* 0xC0 */ 0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
//    /* 0xC8 */ 0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x00D0, 0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168,
//    /* 0xD8 */ 0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
//    /* 0xE0 */ 0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
//    /* 0xE8 */ 0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x00F0, 0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169,
//    /* 0xF8 */ 0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138,
//  },
//  /* ISO-8859-11 */ {
//    /* 0xA0 */ 0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07,
//    /* 0xA8 */ 0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
//    /* 0xB0 */ 0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17,
//    /* 0xB8 */ 0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
//    /* 0xC0 */ 0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27,
//    /* 0xC8 */ 0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
//    /* 0xD0 */ 0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
//    /* 0xD8 */ 0x0E38, 0x0E39, 0x0E3A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0E3F,
//    /* 0xE0 */ 0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47,
//    /* 0xE8 */ 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
//    /* 0xF0 */ 0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57,
//    /* 0xF8 */ 0x0E58, 0x0E59, 0x0E5A, 0x0E5B,
//  },
//  /* ISO-8859-13 */ {
//    /* 0xA0 */ 0x00A0, 0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7,
//    /* 0xA8 */ 0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7,
//    /* 0xB8 */ 0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
//    /* 0xC0 */ 0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
//    /* 0xC8 */ 0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
//    /* 0xD0 */ 0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
//    /* 0xE0 */ 0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
//    /* 0xE8 */ 0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
//    /* 0xF0 */ 0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019,
//  },
//  /* ISO-8859-14 */ {
//    /* 0xA0 */ 0x00A0, 0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7,
//    /* 0xA8 */ 0x1E80, 0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
//    /* 0xB0 */ 0x1E1E, 0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56,
//    /* 0xB8 */ 0x1E81, 0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61,
//    /* 0xC0 */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
//    /* 0xC8 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x1E6A,
//    /* 0xD8 */ 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
//    /* 0xE0 */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
//    /* 0xE8 */ 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x1E6B,
//    /* 0xF8 */ 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF,
//  },
//  /* ISO-8859-15 */ {
//    /* 0xA0 */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7,
//    /* 0xA8 */ 0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
//    /* 0xB0 */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7,
//    /* 0xB8 */ 0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
//    /* 0xC0 */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
//    /* 0xC8 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
//    /* 0xD0 */ 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
//    /* 0xD8 */ 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
//    /* 0xE0 */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
//    /* 0xE8 */ 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
//    /* 0xF0 */ 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
//    /* 0xF8 */ 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
//  }
//};
//
//// Code table for ISO6937 codes from 0xa0 to 0xbf and 0xd0 to 0xff
//static const uint16_t iso6937_single_byte[96] = {
//  /* 0xA0 */ 0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x0000, 0x00a5, 0x0000, 0x00a7,
//  /* 0xA8 */ 0x00a4, 0x2018, 0x201c, 0x00ab, 0x2190, 0x2191, 0x2192, 0x2193,
//  /* 0xB0 */ 0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00d7, 0x00b5, 0x00b6, 0x00b7,
//  /* 0xB8 */ 0x00f7, 0x2019, 0x201d, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
//  /* 0xC0 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//  /* 0xC8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//  /* 0xD0 */ 0x2014, 0x00b9, 0x00ae, 0x00a9, 0x2122, 0x266a, 0x00ac, 0x00a6,
//  /* 0xD8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x215b, 0x215c, 0x215d, 0x215e,
//  /* 0xE0 */ 0x2126, 0x00c6, 0x00d0, 0x00aa, 0x0126, 0x0000, 0x0132, 0x013f,
//  /* 0xE8 */ 0x0141, 0x00d8, 0x0152, 0x00ba, 0x00de, 0x0166, 0x014a, 0x0149,
//  /* 0xF0 */ 0x0138, 0x00e6, 0x0111, 0x00f0, 0x0127, 0x0131, 0x0133, 0x0140,
//  /* 0xF8 */ 0x0142, 0x00f8, 0x0153, 0x00df, 0x00fe, 0x0167, 0x014b, 0x00ad,
//};
//
//// Code table for ISO6937 two-byte sequences combining an accent byte.
//// in the range 0xc0 to 0xcf with a space character (0x20)
//static const uint16_t iso6937_lone_accents[16] = {
//  /* 0xC0 */ 0x0000, 0x00B4, 0x0000, 0x0000, 0x0000, 0x00Af, 0x02D8, 0x02D9,
//  /* 0xC8 */ 0x00A8, 0x0000, 0x02DA, 0x00B8, 0x0000, 0x02DD, 0x02DB, 0x02C7,
//};
//
//// Code tables for ISO6937 two-byte sequences combining an accent byte.
//// in the range 0xc1 to 0xcf with a letter in one of the ranges
//// 0x41 to 0x5a (A-Z) or 0x61 to 0x7a (a-z)
//static const uint16_t iso6937_multi_byte[16][52] = {
//  /* 0xC0 */
//  {
//    0x0000,
//  },
//  /* 0xC1 */
//  {
//    /* A-Z */
//    0x00c0, 0x0000, 0x0000, 0x0000, 0x00c8, 0x0000, 0x0000, 0x0000, 0x00cc, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00d2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x00e0, 0x0000, 0x0000, 0x0000, 0x00e8, 0x0000, 0x0000, 0x0000, 0x00ec, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00f2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//  },
//  /* 0xC2 */
//  {
//    /* A-Z */
//    0x00c1, 0x0000, 0x0106, 0x0000, 0x00c9, 0x0000, 0x0000, 0x0000, 0x00cd, 0x0000, 0x0000, 0x0139, 0x0000,
//    0x0143, 0x00d3, 0x0000, 0x0000, 0x0154, 0x015a, 0x0000, 0x00da, 0x0000, 0x0000, 0x0000, 0x00dd, 0x0179,
//    /* a-z */
//    0x00e1, 0x0000, 0x0107, 0x0000, 0x00e9, 0x0000, 0x0000, 0x0000, 0x00ed, 0x0000, 0x0000, 0x013a, 0x0000,
//    0x0144, 0x00f3, 0x0000, 0x0000, 0x0155, 0x015b, 0x0000, 0x00fa, 0x0000, 0x0000, 0x0000, 0x00fd, 0x017a
//  },
//  /* 0xC3 */
//  {
//    /* A-Z */
//    0x00c2, 0x0000, 0x0108, 0x0000, 0x00ca, 0x0000, 0x011c, 0x0124, 0x00ce, 0x0134, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00d4, 0x0000, 0x0000, 0x0000, 0x015c, 0x0000, 0x00db, 0x0000, 0x0174, 0x0000, 0x0176, 0x0000,
//    /* a-z */
//    0x00e2, 0x0000, 0x0109, 0x0000, 0x00ea, 0x0000, 0x011d, 0x0125, 0x00ee, 0x0135, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00f4, 0x0000, 0x0000, 0x0000, 0x015d, 0x0000, 0x00fb, 0x0000, 0x0175, 0x0000, 0x0177, 0x0000
//  },
//  /* 0xC4 */
//  {
//    /* A-Z */
//    0x00c3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0128, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x00d1, 0x00d5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0168, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x00e3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0129, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x00f1, 0x00f5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0169, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xC5 */
//  {
//    /* A-Z */
//    0x0100, 0x0000, 0x0000, 0x0000, 0x0112, 0x0000, 0x0000, 0x0000, 0x012a, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x014c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x0101, 0x0000, 0x0000, 0x0000, 0x0113, 0x0000, 0x0000, 0x0000, 0x012b, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x014d, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016b, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xC6 */
//  {
//    /* A-Z */
//    0x0102, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x011e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x0103, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x011f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016d, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xC7 */
//  {
//    /* A-Z */
//    0x0000, 0x0000, 0x010a, 0x0000, 0x0116, 0x0000, 0x0120, 0x0000, 0x0130, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x017b,
//    /* a-z */
//    0x0000, 0x0000, 0x010b, 0x0000, 0x0117, 0x0000, 0x0121, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x017c
//  },
//  /* 0xC8 */
//  {
//    /* A-Z */
//    0x00c4, 0x0000, 0x0000, 0x0000, 0x00cb, 0x0000, 0x0000, 0x0000, 0x00cf, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00d6, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00dc, 0x0000, 0x0000, 0x0000, 0x0178, 0x0000,
//    /* a-z */
//    0x00e4, 0x0000, 0x0000, 0x0000, 0x00eb, 0x0000, 0x0000, 0x0000, 0x00ef, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x00f6, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00fc, 0x0000, 0x0000, 0x0000, 0x00ff, 0x0000
//  },
//  /* 0xC9 */
//  {
//    0x0000,
//  },
//  /* 0xCA */
//  {
//    /* A-Z */
//    0x00c5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x00e5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xCB */
//  {
//    /* A-Z */
//    0x0000, 0x0000, 0x00c7, 0x0000, 0x0000, 0x0000, 0x0122, 0x0000, 0x0000, 0x0000, 0x0136, 0x013b, 0x0000,
//    0x0145, 0x0000, 0x0000, 0x0000, 0x0156, 0x015e, 0x0162, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x0000, 0x0000, 0x00e7, 0x0000, 0x0000, 0x0000, 0x0123, 0x0000, 0x0000, 0x0000, 0x0137, 0x013c, 0x0000,
//    0x0146, 0x0000, 0x0000, 0x0000, 0x0157, 0x015f, 0x0163, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xCC */
//  {
//    0x0000,
//  },
//  /* 0xCD */
//  {
//    /* A-Z */
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0150, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0170, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0151, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0171, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xCE */
//  {
//    /* A-Z */
//    0x0104, 0x0000, 0x0000, 0x0000, 0x0118, 0x0000, 0x0000, 0x0000, 0x012e, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0172, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//    /* a-z */
//    0x0105, 0x0000, 0x0000, 0x0000, 0x0119, 0x0000, 0x0000, 0x0000, 0x012f, 0x0000, 0x0000, 0x0000, 0x0000,
//    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0173, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//  },
//  /* 0xCF */
//  {
//    /* A-Z */
//    0x0000, 0x0000, 0x010c, 0x010e, 0x011a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x013d, 0x0000,
//    0x0147, 0x0000, 0x0000, 0x0000, 0x0158, 0x0160, 0x0164, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x017d,
//    /* a-z */
//    0x0000, 0x0000, 0x010d, 0x010f, 0x011b, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x013e, 0x0000,
//    0x0148, 0x0000, 0x0000, 0x0000, 0x0159, 0x0161, 0x0165, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x017e
//  }
//};
//

//unsigned int out_len = 0;
//
//
//char xml_buf[1024];

//######################################################
// dla konwersji stringów z iso 6937 na utf-t np czechy
//http://en.wikipedia.org/wiki/ISO_6937
const uint16_t tab_c1[2][10]={
    
    {'A','E','I','O','U','a','e','i','o','u'},
    {'À','È','Ì','Ò','Ù','à','è','ì','ò','ù'},
};


const uint16_t tab_c2[2][25]={
    {'A','C','E','I','L','N','O','R','S','U','Y','Z','a','c','e','g','i','l','n','o','r','s','u','y','z'},
    {'Á','Ć','É','Í','Ĺ','Ń','Ó','Ŕ','Ś','Ú','Ý','Ź','á','ć','é','ģ','í','ĺ','ń','ó','ŕ','ś','ú','ý','ź'}
    
};

const uint16_t tab_c3[2][26] = {
    {'A','C','E','G','H','I','J','O','S','U','W','Y','a','c','e','g','h','i','j','o','s','u','w','y'},
    {'Â','Ĉ','Ê','Ĝ','Ĥ','Î','Ĵ','Ô','Ŝ','Û','Ŵ','Ŷ','â','ĉ','ê','ĝ','ĥ','î','ĵ','ô','ŝ','û','ŵ','ŷ'}
};

const uint16_t tab_ca[2][4] = {
    {'A','U','a','u'},
    {'Å','Ů','å','ů'}
};

const uint16_t tab_cf[2][18]={
    {'C','D','E','L','N','R','S','T','Z','c','d','e','l','n','r','s','t','z'},  
    {'Č','Ď','Ě','Ľ','Ň','Ř','Š','Ť','Ž','č','ď','ě','ľ','ň','ř','š','ť','ž'}
};
//######################################################


char *d_ConvertCharset(char *cpEncodeFrom, char *cpEncodeTo,char *cpInput,size_t iInputLen,int * Out_put_len,char*xml_buf)
{
	char *cpOut;
	size_t  iOutLen, iReturn;
	//printf("in_len:%d inbuf:%s\n",iInputLen,cpInput);
	//test_printf(cpInput,iInputLen,2);
	iconv_t c_pt;
	//printf("cpto: %s, cpfrom: %s\n", cpEncodeTo, cpEncodeFrom);
	//printf("cpinput:%s\n", cpInput);
	if ((c_pt = iconv_open(cpEncodeTo, cpEncodeFrom)) == (iconv_t)-1)
	{
      		//printf("iconv_open failed!\n");
        	return NULL;
	}
	iconv(c_pt, NULL, NULL, NULL, NULL);
	iOutLen = 1024;
	cpOut = xml_buf;
	iReturn = iconv(c_pt, &cpInput, &iInputLen, &cpOut,&iOutLen);
	//printf("iReturn:%d\n",iReturn);
	if (iReturn == (size_t)-1)
      	{
            iconv_close(c_pt);
            return NULL;
	}
	iconv_close(c_pt);
	//printf("out_len:%d xml_buf:%s\n",1024-iOutLen,xml_buf);
	xml_buf[1024-iOutLen]=0;
	*Out_put_len = 1024-iOutLen;
	//test_printf(xml_buf,1024-iOutLen,2);
	return xml_buf;
}

void reorgnize_buf(unsigned char * out_buf, unsigned char * in_buf, unsigned int in_len,unsigned int * out_len)
{
	if((out_buf == NULL) ||( in_buf == NULL) || in_len == 0)
	{
		*out_len = 0;
		return;
	}
	if(in_buf[0]== 0)
	{
		*out_len = 0;
		return;
	}
	unsigned int i= 0;
	int j = 0;
	for(; i <= in_len; i++)
	{
		if((in_buf[i]>0 && in_buf[i] <= 0x1f) ||(in_buf[i]>= 0x80 && in_buf[i] <= 0x9f))
		{
			continue;
		}
		else
		{
			out_buf[j++] = in_buf[i];	
		}
	}
	*out_len = j;
	return;
}
 
char * generate_xml_text(unsigned char * buf,unsigned int len)
{
        unsigned int  in_len = 0;
	unsigned char from_code[64];
	unsigned char * from_buf = NULL;
	unsigned char  temp_buf[1024];
	unsigned int temp_len = 0;
        char *txt2 = NULL;
        unsigned int out_len = 0;
        char xml_buf[1024];
        
        if(buf == NULL)
        {
		//DEMUX_XML_DEBUG("buf is null\n");
                return NULL;
        }
	if(len<=0)
	{
		//DEMUX_XML_DEBUG("xml text length <=0!\n");
		xml_buf[0] = 0;
		return NULL;
	}
	//DEMUX_XML_DEBUG("buf[0]is %i\n", buf[0]);
	if(buf[0]<=0x0f && buf[0]>=1)
	{
		int temp_sub = (int)(buf[0]+4); 
		sprintf((char*)from_code,"ISO_8859-%d",temp_sub);
		from_buf = &buf[1];
		in_len = len-1;
		in_len = (in_len> 1024)? 1024: in_len;
		//printf("1.from_code:%s,%p\n",from_code, from_code);
		reorgnize_buf(temp_buf, from_buf, in_len, &temp_len);
		from_buf = temp_buf;
		in_len = temp_len;
	}
	else if(buf[0]>=0x10 && buf[0]<0x1f)
	{
		switch(buf[0])
		{
			case 0x10:
			{
				sprintf((char*)from_code,"ISO-8859-%d",buf[2]);
			//	printf("2.from_code:%s\n",from_code);
				from_buf = &buf[3];
				in_len = len - 3;
				break;
			}
			case 0x11:
			{
				strcpy((char*)from_code,"ISO-10646-UCS-2");
			//	printf("3.from_code:%s\n",from_code);
                                from_buf = &buf[1];
                                in_len = len -1;
				break;			
			}
			case 0x12:
			{
				return 0;
			}
			case 0x13:
			{
				strcpy((char*)from_code,"GB2312");
			//	printf("4.from_code:%s\n",from_code);
				from_buf = &buf[1];
				in_len = len -1;
				break;
			}
			case 0x14:
			{
				strcpy((char*)from_code,"ISO-10646-UCS-2");//it should be BIG5,but there has err when using BIG5 in TaiWan TS
				//strcpy(from_code,"BIG5");
			//	printf("5.from_code:%s\n",from_code);
				from_buf = &buf[1];
				in_len = len -1;
				break;
			}
			default:
			{
				return 0;
				break;
			}
		}
	}
	else 
	{
            
            memset(xml_buf,0, sizeof(char)*1024);
            
             if(buf[0]>0x20 && buf[0]<0xff){
                
            //     printf("po huj tu wszedlem?????????\n");
                unsigned int cc,dd;
            
                
                txt2 = (char*)malloc(sizeof(char)*len+1); 
                memset(txt2,0,(len+1));
                
                for(cc = 0,dd=0;cc<len;cc++,dd++)
                {
                    if(buf[cc]>=0xc1 && buf[cc]<=0xcf)
                    {
                        uint8_t znak[2];
                        znak[0] = 0;
                        znak[1] = 0;
                        int p;
                        
                        if(buf[cc]==0xc1)
                        {
                            
                            for(p = 0;p<10;p++)
                            {
                                if(tab_c1[0][p]==buf[cc+1])
                                {
                                   znak[0] = (tab_c1[1][p] & 0xff00)  >> 8;
                                   znak[1] = (tab_c1[1][p] & 0x00ff);
                                }
                            }
                        }
                        
                        if(buf[cc]==0xc2)
                        {
                            
                            for(p = 0;p<25;p++)
                            {
                                if(tab_c2[0][p]==buf[cc+1])
                                {
                                    znak[0] = (tab_c2[1][p] & 0xff00)  >> 8;
                                        znak[1] = (tab_c2[1][p] & 0x00ff);
                                }
                            }
                        }
                        
                        if(buf[cc]==0xc3)
                        {
                            
                            for(p = 0;p<26;p++)
                            {
                                if(tab_c3[0][p]==buf[cc+1])
                                {
                                    znak[0] = (tab_c3[1][p] & 0xff00)  >> 8;
                                        znak[1] = (tab_c3[1][p] & 0x00ff);
                                }
                            }
                        }
                        
                        if(buf[cc]==0xca)
                        {
                            
                            for(p = 0;p<4;p++)
                            {
                                if(tab_ca[0][p]==buf[cc+1])
                                {
                                    znak[0] = (tab_ca[1][p] & 0xff00)  >> 8;
                                        znak[1] = (tab_ca[1][p] & 0x00ff);
                                }
                            }
                        }
                        
                        if(buf[cc]==0xcf)
                        {
                            
                            for(p = 0;p<18;p++)
                            {
                                if(tab_cf[0][p]==buf[cc+1])
                                {
                                    znak[0] = (tab_cf[1][p] & 0xff00)  >> 8;
                                        znak[1] = (tab_cf[1][p] & 0x00ff);
                                }
                            }
                        }
                        
                       txt2[dd] = znak[0];
                       txt2[dd+1] = znak[1];
                       cc+=1;
                       dd++;
                    }
                    else
                    {
                        txt2[dd] = buf[cc];
                    }
                    

                }
                
         //       printf("txt2:%s\n",txt2);
                memcpy(xml_buf,txt2,dd);
                
                if(txt2!=NULL)free(txt2);
                
                return xml_buf;
            }
            else
            {
		strcpy((char*)from_code,"ISO_8859-2");

		from_buf = buf;
		in_len = len;
            }
	}
	memset(xml_buf,0, sizeof(char)*1024);

	char tmp_type[128];
	sprintf(tmp_type,"%s//IGNORE",ENCODE_TYPE);
	//printf("from_code: %s, %p\n", from_code, from_code);
        
	if(d_ConvertCharset((char*)from_code,tmp_type,(char*)from_buf,in_len,(int*)&out_len,xml_buf)==NULL) return NULL;

	from_buf = NULL;
	return xml_buf;
	
}

//static int initialized = 0;
//
//static iconv_t iconvcd[0x24];
//
///* Note: The DVB character set standard seems to get violated
//   quite often, for example on Astra 19.2 E, 10847000/V/22000000:
//   these channels use spanish accents without a proper DVB character
//   set; these characters would for example match ISO-8859-15, so maybe
//   it's a good idea to fall back to that charset, or maybe we should
//   introduce a parameter for that... */
//
//int dvbchar_init() {
//
//  /* We try to support the character sets specified by
//     ETSI EN 300 468 V1.5.1 Annex A, but please note that this is 
//     largely untested because not all sets are used on Astra... */
//
//  /* Internally we work with UTF-8 because libxml2 also gives us UTF-8 from
//     the config files, so everybody can use his specific character set in the
//     config files by using the xml encoding attribute */
//
//  /* The DVB standard says if the first byte of a string is >= 0x20, then
//     the string is coded according to ISO 6937, otherwise the first byte
//     specifies the character table as follows: */
//  iconvcd[0x00] = iconv_open("UTF-8", "ISO6937"); /* default */
//  iconvcd[0x01] = iconv_open("UTF-8", "ISO8859-5");
//  iconvcd[0x02] = iconv_open("UTF-8", "ISO8859-6");
//  iconvcd[0x03] = iconv_open("UTF-8", "ISO8859-7");
//  iconvcd[0x04] = iconv_open("UTF-8", "ISO8859-8");
//  iconvcd[0x05] = iconv_open("UTF-8", "ISO8859-9");
//  iconvcd[0x06] = iconv_open("UTF-8", "ISO8859-10");
//  iconvcd[0x07] = iconv_open("UTF-8", "ISO8859-11");
//  iconvcd[0x08] = (iconv_t)(-1); /* ISO 8859-12, possibly reserved for Indian */
//  iconvcd[0x09] = iconv_open("UTF-8", "ISO8859-13");
//  iconvcd[0x0a] = iconv_open("UTF-8", "ISO8859-14");
//  iconvcd[0x0b] = iconv_open("UTF-8", "ISO8859-15");
//  iconvcd[0x0c] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x0d] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x0e] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x0f] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x10] = (iconv_t)(-1); /* Specifies the ISO 8859 table in two further bytes */
//  iconvcd[0x11] = iconv_open("UTF-8", "ISO-10646/UCS2"); /* TODO: needs testing */
//  iconvcd[0x12] = iconv_open("UTF-8", "EUCKR"); /* Compatible with KSC5601? TODO: needs testing */
//  iconvcd[0x13] = iconv_open("UTF-8", "GB2312"); /* TODO: needs testing */
//  iconvcd[0x14] = iconv_open("UTF-8", "ISO-10646/UCS2"); /* TODO: needs testing */
//  iconvcd[0x15] = (iconv_t)(-1); /* Already UTF-8 :-) */
//  iconvcd[0x16] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x17] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x18] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x19] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1a] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1b] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1c] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1d] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1e] = (iconv_t)(-1); /* reserved for future use */
//  iconvcd[0x1f] = (iconv_t)(-1); /* reserved for future use */
//
//  /* The following tables can't be specified by the first byte directly, but
//     the DVB standard allows to address ISO 8859 tables by putting 0x10
//     into the first byte and the table number into the following two bytes */
//  iconvcd[0x20] = iconv_open("UTF-8", "ISO8859-1");
//  iconvcd[0x21] = iconv_open("UTF-8", "ISO8859-2");
//  iconvcd[0x22] = iconv_open("UTF-8", "ISO8859-3");
//  iconvcd[0x23] = iconv_open("UTF-8", "ISO8859-4");
//
//  if (iconvcd[0x00] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support the default DVB character set (ISO6937)\n");
//  }
//  if (iconvcd[0x01] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x01 (ISO8859-5)\n");
//  }
//  if (iconvcd[0x02] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x02 (ISO8859-6)\n");
//  }
//  if (iconvcd[0x03] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x03 (ISO8859-7)\n");
//  }
//  if (iconvcd[0x04] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x04 (ISO8859-8)\n");
//  }
//  if (iconvcd[0x05] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x05 (ISO8859-9)\n");
//  }
//  if (iconvcd[0x06] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x06 (ISO8859-10)\n");
//  }
//  if (iconvcd[0x07] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x07 (ISO8859-11)\n");
//  }
//  if (iconvcd[0x09] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x09 (ISO8859-13)\n");
//  }
//  if (iconvcd[0x0a] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x0a (ISO8859-14)\n");
//  }
//  if (iconvcd[0x0b] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x0b (ISO8859-15)\n");
//  }
//  if (iconvcd[0x11] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x11 (ISO-10646)\n");
//  }
//  if (iconvcd[0x11] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x12 (EUCKR)\n");
//  }
//  if (iconvcd[0x13] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x13 (GB2312)\n");
//  }
//  if (iconvcd[0x14] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x14 (ISO-10646)\n");
//  }
//  if (iconvcd[0x20] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x10/1 (ISO8859-1)\n");
//  }
//  if (iconvcd[0x21] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x10/2 (ISO8859-2)\n");
//  }
//  if (iconvcd[0x22] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x10/3 (ISO8859-3)\n");
//  }
//  if (iconvcd[0x23] == (iconv_t)(-1)) {
//    ERROR_PRINT("Warning: iconv does not seem to support DVB character set 0x10/4 (ISO8859-4)\n");
//  }
//
//  initialized = 1;
//
//  return 0;
//}
//
//void dvbchar_exit() {
//  int i;
//
//  if (!initialized) {
//    return;
//  }
//
//  for (i = 0; i < 0x24; i++) {
//    if (iconvcd[i] != (iconv_t)(-1)) {
//      iconv_close(iconvcd[i]);
//      iconvcd[i] = (iconv_t)(-1);
//    }
//  }
//}
//
///*
// * - Supports the DVB character tables defined in
// *   ETSI EN 300 468 V1.5.1 Annex A
// * - Removes the control characters (0x80 - 0x9f)
// * - Converts the string to UTF-8
// * - In the case of a multi byte character set at *from,
// *   length specifies the number of bytes, not characters
// *
// * Attention: The buffer at *to must be large enough to store
// *            4 * length + 1 bytes, because UTF-8 uses up to 4 bytes
// *            per character, and the destination string will be '\0'
// *            terminated
// */
//void dvbchar_convert_to_utf8(char *to, unsigned char *from, int length) {
//  char buf[length];
//  int table = 0;
//  int i = 0, j = 0;
//  char *inbuf, *outbuf;
//  size_t inbytesleft, outbytesleft;
//
//  /* The first byte *may* indicate the charset */
//  if (from[0] >= 0x01 && from[0] <= 0x1f) {
//    table = from[0];
//    i++;
//
//    if (table == 0x10) {
//      /* The next two bytes specify the used ISO 8859 table */
//      table = (from[1] << 8) | from[2];
//      i += 2;
//
//      if (table >= 5 && table <= 15) {
//        /* ISO 8859-5 to ISO 8859-15 are mapped to their direct ID */
//        table -= 4;
//      } else if (table >= 1 && table <= 4) {
//        /* ISO 8859-1 to ISO 8859-4 are mapped two our extended
//           internal table (see above) */
//        table += 0x1f;
//      }
//
//    }
//  }
//
//  if (table <= 0x10) {
//    /* Skip the control characters (single byte character tables) */
//    for (; i < length; i++) {
//      if (from[i] < 0x80 || from[i] > 0x9f) {
//        buf[j++] = from[i];
//      } else if (from[i] == 0x8a) {
//        /* 0x8a means new line */
//        buf[j++] = '\n';
//      }
//    }
//  } else {
//    /* Skip the control characters (two byte character tables) */
//    /* TODO: needs testing */
//    for (; i < (length - 1); i += 2) {
//      if (from[i] != 0xe0 || from[i + 1] < 0x80 || from[i + 1] > 0x9f) {
//        buf[j++] = from[i];
//        buf[j++] = from[i + 1];
//      } else if (from[i] == 0xe0 && from[i + 1] == 0x8a) {
//        /* 0xe0,0x8a means new line */
//        /* TODO: Find out how two byte character tables represent a new line */
//        buf[j++] = 0;
//        buf[j++] = '\n';
//      }
//    }
//  }
//
//  length = j;
//
//  if (iconvcd[table] == (iconv_t)(-1)) {
//    ERROR_PRINTF("No iconv support for DVB character table 0x%02x "
//                 "available, blindly copying the received string\n",
//                 table);
//    strncpy((char *)to, buf, length);
//    to[length] = '\0';
//    return;
//  }
//
//  /* Reset the iconv conversion state */
//  iconv(iconvcd[table], NULL, NULL, NULL, NULL);
//
//  inbytesleft = (size_t)length;
//  outbytesleft = (size_t)(length * 4);
//  inbuf = buf;
//  outbuf = to;
//  if (iconv(iconvcd[table], &inbuf, &inbytesleft, &outbuf, &outbytesleft) ==
//      (size_t)(-1)) {
//    ERROR_PRINTF("iconv() failed for DVB character table 0x%02x, "
//                 "blindly copying the received string (%s)\n",
//                 table, strerror(errno));
//    strncpy(to, buf, length);
//    to[length] = '\0';
//    return;
//  }
//
//  if (inbytesleft > 0) {
//    ERROR_PRINT("iconv() left inbytesleft at >0 without reporting "
//                "an error, blindly copying the received string\n");
//    strncpy(to, buf, length);
//    to[length] = '\0';
//    return;
//  }
//
//  *outbuf = '\0';
//}
//
//
//
//
//
//static int convert_iso_8859[16] = {
//  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, 11, 12, 13
//};
//#define convert_utf8   14
//#define convert_iso6937 15
//
/*
#define PL_FIX_CHARS \
"\xA1\xA3\xA6\xAC\xAF\xB1\xB3\xB6\xBC\xBF"\
"\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xCA\xCB\xCD\xCE\xCF"\
"\xD1\xD3\xE6\xE8\xEA\xF1\xF3\xF8"
 * */
//
//static inline int encode_utf8(unsigned int c, char *outb, int outleft)
//{
//  if (c <= 0x7F && outleft >= 1) {
//    *outb = c;
//    return 1;
//  } else if (c <= 0x7FF && outleft >=2) {
//    *outb++ = ((c >>  6) & 0x1F) | 0xC0;
//    *outb++ = ( c        & 0x3F) | 0x80;
//    return 2;
//  } else if (c <= 0xFFFF && outleft >= 3) {
//    *outb++ = ((c >> 12) & 0x0F) | 0xE0;
//    *outb++ = ((c >>  6) & 0x3F) | 0x80;
//    *outb++ = ( c        & 0x3F) | 0x80;
//    return 3;
//  } else if (c <= 0x10FFFF && outleft >= 4) {
//    *outb++ = ((c >> 18) & 0x07) | 0xF0;
//    *outb++ = ((c >> 12) & 0x3F) | 0x80;
//    *outb++ = ((c >>  6) & 0x3F) | 0x80;
//    *outb++ = ( c        & 0x3F) | 0x80;
//    return 4;
//  } else {
//    return -1;
//  }
//}
//
//static inline size_t conv_utf8(const uint8_t *src, size_t srclen,
//                              char *dst, size_t *dstlen)
//{
//  while (srclen>0 && (*dstlen)>0) {
//    *dst = (char) *src;
//    srclen--; (*dstlen)--;
//    src++; dst++;
//  }
//  if (srclen>0) {
//    errno = E2BIG;
//    return -1;
//  }
//  return 0;
//}
//
//static inline size_t conv_8859(int conv,
//                              const uint8_t *src, size_t srclen,
//                              char *dst, size_t *dstlen)
//{
//  uint16_t *table = conv_8859_table[conv];
//
//  while (srclen>0 && (*dstlen)>0) {
//    uint8_t c = *src;
//    if (c <= 0x7f) {
//      // lower half of iso-8859-* is identical to utf-8
//      *dst = (char) *src;
//      (*dstlen)--;
//      dst++;
//    } else if (c <= 0x9f) {
//      // codes 0x80 - 0x9f (control codes) are mapped to ' '
//      *dst = ' ';
//      (*dstlen)--;
//      dst++;
//    } else {
//      // map according to character table, skipping
//      // unmapped chars (value 0 in the table)
//      uint16_t uc = table[c-0xa0];
//      if (uc != 0) {
//        int len = encode_utf8(uc, dst, *dstlen);
//        if (len == -1) {
//          errno = E2BIG;
//          return -1;
//        } else {
//          (*dstlen) -= len;
//          dst += len;
//        }
//      }
//    }
//    srclen--;
//    src++;
//  }
//  if (srclen>0) {
//    errno = E2BIG;
//    return -1;
//  }
//  return 0;
//}
//
//static inline size_t conv_6937(const uint8_t *src, size_t srclen,
//                              char *dst, size_t *dstlen)
//{
//  while (srclen>0 && (*dstlen)>0) {
//    uint8_t c = *src;
//    if (c <= 0x7f) {
//      // lower half of iso6937 is identical to utf-8
//      *dst = (char) *src;
//      (*dstlen)--;
//      dst++;
//    } else if (c <= 0x9f) {
//      // codes 0x80 - 0x9f (control codes) are mapped to ' '
//      *dst = ' ';
//      (*dstlen)--;
//      dst++;
//    } else {
//      uint16_t uc;
//      if (c >= 0xc0 && c <= 0xcf) {
//        // map two-byte sequence, skipping illegal combinations.
//        if (srclen<2) {
//          errno = EINVAL;
//          return -1;
//        }
//        srclen--;
//        src++;
//        uint8_t c2 = *src;
//        if (c2 == 0x20) {
//          uc = iso6937_lone_accents[c-0xc0];
//        } else if (c2 >= 0x41 && c2 <= 0x5a) {
//          uc = iso6937_multi_byte[c-0xc0][c2-0x41];
//        } else if (c2 >= 0x61 && c2 <= 0x7a) {
//          uc = iso6937_multi_byte[c-0xc0][c2-0x61+26];
//        } else {
//          uc = 0;
//        }
//      } else {
//        // map according to single character table, skipping
//        // unmapped chars (value 0 in the table)
//        uc = iso6937_single_byte[c-0xa0];
//      }
//      if (uc != 0) {
//        int len = encode_utf8(uc, dst, *dstlen);
//        if (len == -1) {
//          errno = E2BIG;
//          return -1;
//        } else {
//          (*dstlen) -= len;
//          dst += len;
//        }
//      }
//    }
//    srclen--;
//    src++;
//  }
//  if (srclen>0) {
//    errno = E2BIG;
//    return -1;
//  }
//  return 0;
//}
//
//
//
//
//
//
//static inline size_t dvb_convert(int conv,const uint8_t *src, size_t srclen,char *dst, size_t *dstlen)
//{
//  switch (conv) {
//    case convert_utf8: return conv_utf8(src, srclen, dst, dstlen);
//    case convert_iso6937: return conv_6937(src, srclen, dst, dstlen);
//    default: return conv_8859(conv, src, srclen, dst, dstlen);
//  }
//}
//
//
///*
// * DVB String conversion according to EN 300 468, Annex A
// * Not all character sets are supported, but it should cover most of them
// */
//
//int dvb_get_string(char *dst, size_t dstlen, const uint8_t *src, size_t srclen, char *dvb_default_charset, dvb_string_conv_t *conv)
//{
//  int ic;
//  size_t len, outlen;
//  int i;
//
//  if(srclen < 1) {
//    *dst = 0;
//    return 0;
//  }
//
//  /* Check custom conversion */
//  while (conv && conv->func) {
//    if (conv->type == src[0])
//      return conv->func(dst, &dstlen, src, srclen);
//    conv++;
//  }
//
//  switch(src[0]) {
//  case 0:
//    return -1;
//
//  case 0x01 ... 0x0b:
//    // NOTE: some Polish channels using ISO6937 while specifying ISO8859-4
//    if ((src[0] + 4) == 5 && strpbrk((const char*)src, PL_FIX_CHARS))
//    {
//      ic = convert_iso6937;
//    }
//    else
//      ic = convert_iso_8859[src[0] + 4];
//    src++; srclen--;
//    break;
//
//  case 0x0c ... 0x0f:
//    return -1;
//
//  case 0x10: /* Table A.4 */
//    if(srclen < 3 || src[1] != 0 || src[2] == 0 || src[2] > 0x0f)
//      return -1;
//
//    ic = convert_iso_8859[src[2]];
//    src+=3; srclen-=3;
//    break;
//    
//  case 0x11 ... 0x14:
//    return -1;
//
//  case 0x15:
//    ic = convert_utf8;
//    break;
//  case 0x16 ... 0x1f:
//    return -1;
//
//  default:
//    if (dvb_default_charset != NULL && sscanf(dvb_default_charset, "ISO8859-%d", &i) > 0) {
//      if (i > 0 && i < 16) {
//        ic = convert_iso_8859[i];
//      } else {
//        ic = convert_iso6937;
//      }
//    } else {
//      // Note: some Polish channels using ISO8859-2 while specifying no encoding
//      if (strpbrk((const char*)src, PL_FIX_CHARS))
//      {
//        ic = convert_iso_8859[2];
//      }
//      else
//        ic = convert_iso6937;
//    }
//    break;
//  }
//
//  if(srclen < 1) {
//    *dst = 0;
//    return 0;
//  }
//
//  if(ic == -1)
//    return -1;
//
//  outlen = dstlen - 1;
//
//  if (dvb_convert(ic, src, srclen, dst, &outlen) == -1) {
//    return -1;
//  }
//
//  len = dstlen - outlen - 1;
//  dst[len] = 0;
//  return 0;
//}








char *default_charset = "ISO-6937";
char *output_charset = "UTF-8";
#define CS_OPTIONS "//TRANSLIT"
struct charset_conv {
	unsigned len;
	unsigned char  data[3];
};

/* This table is the Latin 00 table. Basically ISO-6937 + Euro sign */
struct charset_conv en300468_latin_00_to_utf8[256] = {
	{ 1, {0x00, } },
	{ 1, {0x01, } },
	{ 1, {0x02, } },
	{ 1, {0x03, } },
	{ 1, {0x04, } },
	{ 1, {0x05, } },
	{ 1, {0x06, } },
	{ 1, {0x07, } },
	{ 1, {0x08, } },
	{ 1, {0x09, } },
	{ 1, {0x0a, } },
	{ 1, {0x0b, } },
	{ 1, {0x0c, } },
	{ 1, {0x0d, } },
	{ 1, {0x0e, } },
	{ 1, {0x0f, } },
	{ 1, {0x10, } },
	{ 1, {0x11, } },
	{ 1, {0x12, } },
	{ 1, {0x13, } },
	{ 1, {0x14, } },
	{ 1, {0x15, } },
	{ 1, {0x16, } },
	{ 1, {0x17, } },
	{ 1, {0x18, } },
	{ 1, {0x19, } },
	{ 1, {0x1a, } },
	{ 1, {0x1b, } },
	{ 1, {0x1c, } },
	{ 1, {0x1d, } },
	{ 1, {0x1e, } },
	{ 1, {0x1f, } },
	{ 1, {0x20, } },
	{ 1, {0x21, } },
	{ 1, {0x22, } },
	{ 1, {0x23, } },
	{ 1, {0x24, } },
	{ 1, {0x25, } },
	{ 1, {0x26, } },
	{ 1, {0x27, } },
	{ 1, {0x28, } },
	{ 1, {0x29, } },
	{ 1, {0x2a, } },
	{ 1, {0x2b, } },
	{ 1, {0x2c, } },
	{ 1, {0x2d, } },
	{ 1, {0x2e, } },
	{ 1, {0x2f, } },
	{ 1, {0x30, } },
	{ 1, {0x31, } },
	{ 1, {0x32, } },
	{ 1, {0x33, } },
	{ 1, {0x34, } },
	{ 1, {0x35, } },
	{ 1, {0x36, } },
	{ 1, {0x37, } },
	{ 1, {0x38, } },
	{ 1, {0x39, } },
	{ 1, {0x3a, } },
	{ 1, {0x3b, } },
	{ 1, {0x3c, } },
	{ 1, {0x3d, } },
	{ 1, {0x3e, } },
	{ 1, {0x3f, } },
	{ 1, {0x40, } },
	{ 1, {0x41, } },
	{ 1, {0x42, } },
	{ 1, {0x43, } },
	{ 1, {0x44, } },
	{ 1, {0x45, } },
	{ 1, {0x46, } },
	{ 1, {0x47, } },
	{ 1, {0x48, } },
	{ 1, {0x49, } },
	{ 1, {0x4a, } },
	{ 1, {0x4b, } },
	{ 1, {0x4c, } },
	{ 1, {0x4d, } },
	{ 1, {0x4e, } },
	{ 1, {0x4f, } },
	{ 1, {0x50, } },
	{ 1, {0x51, } },
	{ 1, {0x52, } },
	{ 1, {0x53, } },
	{ 1, {0x54, } },
	{ 1, {0x55, } },
	{ 1, {0x56, } },
	{ 1, {0x57, } },
	{ 1, {0x58, } },
	{ 1, {0x59, } },
	{ 1, {0x5a, } },
	{ 1, {0x5b, } },
	{ 1, {0x5c, } },
	{ 1, {0x5d, } },
	{ 1, {0x5e, } },
	{ 1, {0x5f, } },
	{ 1, {0x60, } },
	{ 1, {0x61, } },
	{ 1, {0x62, } },
	{ 1, {0x63, } },
	{ 1, {0x64, } },
	{ 1, {0x65, } },
	{ 1, {0x66, } },
	{ 1, {0x67, } },
	{ 1, {0x68, } },
	{ 1, {0x69, } },
	{ 1, {0x6a, } },
	{ 1, {0x6b, } },
	{ 1, {0x6c, } },
	{ 1, {0x6d, } },
	{ 1, {0x6e, } },
	{ 1, {0x6f, } },
	{ 1, {0x70, } },
	{ 1, {0x71, } },
	{ 1, {0x72, } },
	{ 1, {0x73, } },
	{ 1, {0x74, } },
	{ 1, {0x75, } },
	{ 1, {0x76, } },
	{ 1, {0x77, } },
	{ 1, {0x78, } },
	{ 1, {0x79, } },
	{ 1, {0x7a, } },
	{ 1, {0x7b, } },
	{ 1, {0x7c, } },
	{ 1, {0x7d, } },
	{ 1, {0x7e, } },
	{ 1, {0x7f, } },
	{ 2, {0xc2, 0x80, } },
	{ 2, {0xc2, 0x81, } },
	{ 2, {0xc2, 0x82, } },
	{ 2, {0xc2, 0x83, } },
	{ 2, {0xc2, 0x84, } },
	{ 2, {0xc2, 0x85, } },
	{ 2, {0xc2, 0x86, } },
	{ 2, {0xc2, 0x87, } },
	{ 2, {0xc2, 0x88, } },
	{ 2, {0xc2, 0x89, } },
	{ 2, {0xc2, 0x8a, } },
	{ 2, {0xc2, 0x8b, } },
	{ 2, {0xc2, 0x8c, } },
	{ 2, {0xc2, 0x8d, } },
	{ 2, {0xc2, 0x8e, } },
	{ 2, {0xc2, 0x8f, } },
	{ 2, {0xc2, 0x90, } },
	{ 2, {0xc2, 0x91, } },
	{ 2, {0xc2, 0x92, } },
	{ 2, {0xc2, 0x93, } },
	{ 2, {0xc2, 0x94, } },
	{ 2, {0xc2, 0x95, } },
	{ 2, {0xc2, 0x96, } },
	{ 2, {0xc2, 0x97, } },
	{ 2, {0xc2, 0x98, } },
	{ 2, {0xc2, 0x99, } },
	{ 2, {0xc2, 0x9a, } },
	{ 2, {0xc2, 0x9b, } },
	{ 2, {0xc2, 0x9c, } },
	{ 2, {0xc2, 0x9d, } },
	{ 2, {0xc2, 0x9e, } },
	{ 2, {0xc2, 0x9f, } },
	{ 2, {0xc2, 0xa0, } },
	{ 2, {0xc2, 0xa1, } },
	{ 2, {0xc2, 0xa2, } },
	{ 2, {0xc2, 0xa3, } },
	{ 3, { 0xe2, 0x82, 0xac,} },		/* Euro sign. Addition over the ISO-6937 standard */
	{ 2, {0xc2, 0xa5, } },
	{ 0, {} },
	{ 2, {0xc2, 0xa7, } },
	{ 2, {0xc2, 0xa4, } },
	{ 3, {0xe2, 0x80, 0x98, } },
	{ 3, {0xe2, 0x80, 0x9c, } },
	{ 2, {0xc2, 0xab, } },
	{ 3, {0xe2, 0x86, 0x90, } },
	{ 3, {0xe2, 0x86, 0x91, } },
	{ 3, {0xe2, 0x86, 0x92, } },
	{ 3, {0xe2, 0x86, 0x93, } },
	{ 2, {0xc2, 0xb0, } },
	{ 2, {0xc2, 0xb1, } },
	{ 2, {0xc2, 0xb2, } },
	{ 2, {0xc2, 0xb3, } },
	{ 2, {0xc3, 0x97, } },
	{ 2, {0xc2, 0xb5, } },
	{ 2, {0xc2, 0xb6, } },
	{ 2, {0xc2, 0xb7, } },
	{ 2, {0xc3, 0xb7, } },
	{ 3, {0xe2, 0x80, 0x99, } },
	{ 3, {0xe2, 0x80, 0x9d, } },
	{ 2, {0xc2, 0xbb, } },
	{ 2, {0xc2, 0xbc, } },
	{ 2, {0xc2, 0xbd, } },
	{ 2, {0xc2, 0xbe, } },
	{ 2, {0xc2, 0xbf, } },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 3, {0xe2, 0x80, 0x94, } },
	{ 2, {0xc2, 0xb9, } },
	{ 2, {0xc2, 0xae, } },
	{ 2, {0xc2, 0xa9, } },
	{ 3, {0xe2, 0x84, 0xa2, } },
	{ 3, {0xe2, 0x99, 0xaa, } },
	{ 2, {0xc2, 0xac, } },
	{ 2, {0xc2, 0xa6, } },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 0, {} },
	{ 3, {0xe2, 0x85, 0x9b, } },
	{ 3, {0xe2, 0x85, 0x9c, } },
	{ 3, {0xe2, 0x85, 0x9d, } },
	{ 3, {0xe2, 0x85, 0x9e, } },
	{ 3, {0xe2, 0x84, 0xa6, } },
	{ 2, {0xc3, 0x86, } },
	{ 2, {0xc3, 0x90, } },
	{ 2, {0xc2, 0xaa, } },
	{ 2, {0xc4, 0xa6, } },
	{ 0, {} },
	{ 2, {0xc4, 0xb2, } },
	{ 2, {0xc4, 0xbf, } },
	{ 2, {0xc5, 0x81, } },
	{ 2, {0xc3, 0x98, } },
	{ 2, {0xc5, 0x92, } },
	{ 2, {0xc2, 0xba, } },
	{ 2, {0xc3, 0x9e, } },
	{ 2, {0xc5, 0xa6, } },
	{ 2, {0xc5, 0x8a, } },
	{ 2, {0xc5, 0x89, } },
	{ 2, {0xc4, 0xb8, } },
	{ 2, {0xc3, 0xa6, } },
	{ 2, {0xc4, 0x91, } },
	{ 2, {0xc3, 0xb0, } },
	{ 2, {0xc4, 0xa7, } },
	{ 2, {0xc4, 0xb1, } },
	{ 2, {0xc4, 0xb3, } },
	{ 2, {0xc5, 0x80, } },
	{ 2, {0xc5, 0x82, } },
	{ 2, {0xc3, 0xb8, } },
	{ 2, {0xc5, 0x93, } },
	{ 2, {0xc3, 0x9f, } },
	{ 2, {0xc3, 0xbe, } },
	{ 2, {0xc5, 0xa7, } },
	{ 2, {0xc5, 0x8b, } },
	{ 2, {0xc2, 0xad, } },
};

void descriptorcpy(char **dest, const unsigned char *src, size_t len)
{
	size_t destlen, i;
	char *p, *type = default_charset;
	unsigned char *tmp = NULL;
	const unsigned char *s;
	int emphasis = 0, need_conversion = 1;

//        unsigned char *ff = (unsigned char*)src;
//        for(int z = 0;z<len;z++)
//        {
//                printf("@@@@@@@ %c  0x%x\n",ff[z],ff[z]);
//
//        }
        
	if (*dest) {
		free (*dest);
		*dest = NULL;
	}
	if (!len)
		return;


	if (*src < 0x20) {
		switch (*src) {
		case 0x00:	type = "ISO-6937";		break;
		case 0x01:	type = "ISO-8859-5";		break;
		case 0x02:	type = "ISO-8859-6";		break;
		case 0x03:	type = "ISO-8859-7";		break;
		case 0x04:	type = "ISO-8859-8";		break;
		case 0x05:	type = "ISO-8859-9";		break;
		case 0x06:	type = "ISO-8859-10";		break;
		case 0x07:	type = "ISO-8859-11";		break;
		case 0x09:	type = "ISO-8859-13";		break;
		case 0x0a:	type = "ISO-8859-14";		break;
		case 0x0b:	type = "ISO-8859-15";		break;
		case 0x11:	type = "ISO-10646";		break;
		case 0x12:	type = "ISO-2022-KR";		break;
		case 0x13:	type = "GB2312";		break;
		case 0x14:	type = "BIG5";			break;
		case 0x15:	type = "ISO-10646/UTF-8";	break;
		case 0x10: /* ISO8859 */
			if ((*(src + 1) != 0) || *(src + 2) > 0x0f)
				break;
			src+=2;
			len-=2;
			switch(*src) {
			case 0x01:	type = "ISO-8859-1";		break;
			case 0x02:	type = "ISO-8859-2";		break;
			case 0x03:	type = "ISO-8859-3";		break;
			case 0x04:	type = "ISO-8859-4";		break;
			case 0x05:	type = "ISO-8859-5";		break;
			case 0x06:	type = "ISO-8859-6";		break;
			case 0x07:	type = "ISO-8859-7";		break;
			case 0x08:	type = "ISO-8859-8";		break;
			case 0x09:	type = "ISO-8859-9";		break;
			case 0x0a:	type = "ISO-8859-10";		break;
			case 0x0b:	type = "ISO-8859-11";		break;
			case 0x0d:	type = "ISO-8859-13";		break;
			case 0x0e:	type = "ISO-8859-14";		break;
			case 0x0f:	type = "ISO-8859-15";		break;
			}
		}
		src++;
		len--;
	}

	/*
	 * Destination length should be bigger. As the worse case seems to
	 * use 3 chars for one code, use it for destlen
	 */
	destlen = len * 3;

	*dest = (char*)malloc(destlen + 1);

	/* Remove special chars */
	if (!strncasecmp(type, "ISO-8859", 8) || !strcasecmp(type, "ISO-6937")) {
            
         //   printf("ISO-8859 ISO-6937\n");
		/*
		 * Handles the ISO/IEC 10646 1-byte control codes
		 * (EN 300 468 v1.11.1 Table A.1)
		 * Emphasis will be represented as: *emphased*
		 */
		tmp = (unsigned char*)malloc(len + 2);
		p = (char *)tmp;
		s = src;
		for (i = 0; i < len; i++, s++) {
			if (*s >= 0x20 && (*s < 0x80 || *s > 0x9f))
				*p++ = *s;
			else if (*s == 0x86) {
				*p++ = '*';
				emphasis = 1;
			} else if (*s == 0x87 && emphasis) {
				*p++ = '*';
				emphasis = 0;
			}
		}
		if (emphasis)
			*p++ = '*';
		*p = '\0';
	} else {
		/*
		 * FIXME: need to handle the ISO/IEC 10646 2-byte control codes
		 * (EN 300 468 v1.11.1 Table A.2)
		 */
	}

	if (tmp)
		s = tmp;
	else
		s = src;

	p = *dest;
        

        
	if (!strcasecmp(type, "ISO-6937")) {
		unsigned char *p1, *p2;

            //    printf("ISO-6937\n");
		/* Convert charset to UTF-8 using Code table 00 - Latin */
		for (p1 = (unsigned char *)s; p1 < s + len; p1++)
			for (p2 = en300468_latin_00_to_utf8[*p1].data;
			     p2 < en300468_latin_00_to_utf8[*p1].data + en300468_latin_00_to_utf8[*p1].len;
			     p2++)
				*p++ = *p2;
		*p = '\0';

		/* If desired charset is not UTF-8, prepare for conversion */
		if (strcasecmp(output_charset, "UTF-8")) {
			if (tmp) free(tmp);
                        
  
			tmp = (unsigned char *)*dest;
        
			len = p - *dest;

  
			*dest = (char*)malloc(destlen + 1);

			type = "UTF-8";
			s = tmp;
		} else
			need_conversion = 0;

	}

	/* Convert from original charset to the desired one */
	if (need_conversion) {
		char out_cs[strlen(output_charset) + 1 + sizeof(CS_OPTIONS)];

		p = *dest;
		strcpy(out_cs, output_charset);
		strcat(out_cs, CS_OPTIONS);

		iconv_t cd = iconv_open(out_cs, type);
		if (cd == (iconv_t)(-1)) {
			memcpy(p, s, len);
			p[len] = '\0';
			//printf("Conversion from %s to %s not supported\n",type, output_charset);
		} else {
			iconv(cd, (char **)&s, &len, &p, &destlen);
			iconv_close(cd);
			*p = '\0';
		}
	}

	if (tmp)
		free(tmp);
}