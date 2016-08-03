#ifndef _BITMAP_H_
#define _BITMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "type.h"

/* bitmap file identifier 'BM'*/
#define BITMAP_FORMAT_ID	0x4d42

/* bitmap file header */
#pragma pack(1)
typedef struct _BitmapFileHeader {
    uint16_t	bfType;
    uint32_t	bfSize;
    uint16_t	bfReserved1;
    uint16_t	bfReserved2;
    uint32_t	bfOffBits;
} BitmapFileHeader, *BitmapFileHeaderPtr;
/*#pragma pack()*/

/* bitmap information header */
typedef struct _BitmapInfoHeader {
    uint32_t	biSize;
    uint32_t	biWidth;
    uint32_t	biHeight;
    uint16_t	biPlanes;
    uint16_t	biBitCount;
    uint32_t	biCompression;
    uint32_t	biSizeImage;
    uint32_t	biXPelsPerMeter;
    uint32_t	biYPelSPerMeter;
    uint32_t	biClrUsed;
    uint32_t	biClrImportant;
} BitmapInfoHeader, *BitmapInfoHeaderPtr;

/* bitmap RGB color for 1/2/4/8 bpp mode */
typedef struct _RGBQuad {
    uint8_t	blue;
    uint8_t	green;
    uint8_t	red;
    uint8_t reserved;
} RGBQuad, *RGBQuadPtr;
#pragma pack()

/****************************************************************
 * parameter [in]: path, bitmap file name, including path	*
 * parameter [in]: pFileHeader, bitmap file header structure	*
 * parameter [in]: pInforHeader, bitmap information structure	*
 * return value: a file stream pointer if succeeds, or NULL	*
 * this function open a bitmap file and read the header infor-	*
 * mation into the two given structures, this is the first func-*
 * tion to call before decoding.
 * **************************************************************/

FILE *BitmapInit (char *path,
		  BitmapFileHeaderPtr pFileHeader,
		  BitmapInfoHeaderPtr pInfoHeader);

/****************************************************************
 * parameter [in]: f, file stream pointer of a open file	*
 * return value: 0 if succeed, or non-zero			*
 * this function close an open file, this is the last function	*
 * to call after decoding					*
 * **************************************************************/

int32_t BitmapExit (FILE *f);

/****************************************************************
 * parameter [in]: BitmapFileHeader structure pointer		*
 * this function returns the file size of a bitmap file		*
 * **************************************************************/

uint32_t BitmapGetFileSize (BitmapFileHeaderPtr pFileHeader);

/****************************************************************
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * this function returns the image data  size of a bitmap file	*
 * **************************************************************/

uint32_t BitmapGetImageSize (BitmapInfoHeaderPtr pInfoHeader);

/****************************************************************
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * this function returns the image width			*
 * **************************************************************/

uint32_t BitmapGetImageWidth (BitmapInfoHeaderPtr pInfoHeader);

/****************************************************************
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * this function returns the image height			*
 * **************************************************************/

uint32_t BitmapGetImageHeight (BitmapInfoHeaderPtr pInfoHeader);

/****************************************************************
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * this function returns the bits per pixel of the image	*
 * **************************************************************/

uint16_t BitmapGetImageBitCount (BitmapInfoHeaderPtr pInfoHeader);

/****************************************************************
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * this function returns the offset image color data from the 	*
 * beginning of the bitmap file					*
 * **************************************************************/

uint32_t BitmapGetImageOffset (BitmapFileHeaderPtr pFileHeader);

/****************************************************************
 * parameter [in]: f, file stream pointer			*
 * parameter [in]: BitmapFileHeader structure pointer		*
 * parameter [in]: BitmapInfoHeader structure pointer		*
 * parameter [in]: destination address to store the decoded data*
 * parameter [in]: destination color format, bits per pixel	*
 * **************************************************************/

void BitmapDecodeImage (FILE *f,
			BitmapFileHeaderPtr pFileHeader,
			BitmapInfoHeaderPtr pInfoHeader,
			void *dst,
			uint16_t dstStride,
			uint16_t dstBpp);

/****************************************************************
 * parameter [in]: f, file stream pointer returned by BitmapInit*
 * parameter [in]: rgbQuad[], an RGBQuad structure pointer array*
 * parameter [in]: nRGB, number of colors in an image		*
 * **************************************************************/

void BitmapGetImagePalette (FILE *f,
			    RGBQuadPtr rgbQuad,
			    int16_t nRGB);

#ifdef __cplusplus
}
#endif

#endif

