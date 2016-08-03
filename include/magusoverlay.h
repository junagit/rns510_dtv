#ifndef _MAGUSOVERLAY_H_
#define _MAGUSOVERLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "type.h"


#define overlay_err(fmt, args...)   printf("------->Overlay err: "fmt, ##args)
#define overlay_dbg(fmt, args...)   //printf("------->Overlay dbg: "fmt,##args)

/* number of floating windows limited by hardware */
#define MAX_FW_NUM			2
/* maximum floating windows width in pixel */
#define MAX_FW_WIDTH		720
/* maximum floating windows height in pixel */
#define MAX_FW_HEIGHT		576
/* maximum floating window memory size 
for floating win3&4, the size should be 1/8*/
#if defined(TRUECOLOR)
#define MAX_FW_MEM_SIZE(n)		((MAX_FW_WIDTH * MAX_FW_HEIGHT<<2)>>(n>1?3:0))
#else
#define MAX_FW_MEM_SIZE(n)		((MAX_FW_WIDTH * MAX_FW_HEIGHT<<1)>>(n>1?3:0))
#endif

#define FLOAT_WIN_ONE		0
#define FLOAT_WIN_TWO		1
#define FLOAT_WIN_THREE         2
#define FLOAT_WIN_FOUR		3


/* magus lcdc register base address */
#define LCDC_REG_BASE		0x04105000
/* magus lcdc register memory size */
#define LCDC_REG_SIZE		0x1000
/* LCDC ID */
#define MAGUS_LCDC_ID		0x03810080


/* the color formats */
#define MAGUS_COLOR_1BPP       0
#define MAGUS_COLOR_2BPP       1
#define MAGUS_COLOR_4BPP       2
#define MAGUS_COLOR_8BPP       3
#define MAGUS_COLOR_RGB565     4
#define MAGUS_COLOR_RGB1555    5
#define MAGUS_COLOR_RGB8888    6


#define MAGUS_CLR_BIT(n)        (~(1 << (n)))
#define MAGUS_SET_BIT(n)        (1 << (n))

#define MAGUS_GET_BITS(val, mask, pos) ((val >> pos) & mask)
#define MAGUS_SET_BITS(val, mask, pos, data) {\
	        val &= ~(mask << pos); \
	        val |= (data & mask) << pos; }

#define WIDEN(x)                ((unsigned long) (x))
#define MERGE(x, y)             ((WIDEN(x) << 16) | WIDEN(y))


//start bit defines, flt1&3:nidx=0; flt2&4:nidx=1
#define FLOAT_WIN_EN(nidx)			(nidx)	
#define FLOAT_WIN_ALPHAEN(nidx)	((nidx<<1)+4)
#define FLOAT_WIN_BPP(nidx)			((nidx<<2)+8)
#define FLOAT_WIN_ALPHA(nidx)		((nidx<<3)+16)

/***************2011-03-01 middle interface info*************/
#define SSLFBIOGET_WIN			0x46F0
#define SSLFBIOSET_WIN			0x46F1
#define SSLFBIOGET_TRAP 		0x46F2
#define SSLFBIOSET_TRAP 		0x46F3
#define SSLFBIOGET_PADDING 	0x46F4
#define SSLFBIOSET_PADDING	0x46F5
#define SSLFBIOGET_ROT 			0x46F6
#define SSLFBIOSET_ROT			0x46F7
#define SSLFBIOGET_PANEL		0x46F8
#define SSLFBIODISABLE_WIN  0x46F9
#define SSLFBIOENABLE_WIN   0x46FA
#define SSLFBIOSETOFFSET_WIN 0x46FB
#define SSLFBIOSETALPHA_WIN 0x46FC
#define SSLFBIOSETSIZE_WIN 0x46FD

typedef struct
{
  uint32_t  addr;           /**< physical frame buffer address */
  uint16_t  width;          /**< framebuffer pixel width */
  uint16_t  height;         /**< framebuffer pixel height */
  uint16_t  panel_w;        /**< window pixel width */
  uint16_t  panel_h;        /**< window pixel height */
}
lcdc_area_t, *lcdc_area_p;

typedef struct
{
	lcdc_area_t	a;
	uint16_t	stride;		/**< window pixel stride, 0 = width */
	uint8_t		pixel;		/**< 0-5 for bits per pixel is  2^(pixel) */
}
lcdc_win_t, *lcdc_win_p;

typedef struct
{
	lcdc_win_t	w;
	uint16_t	x;
	uint16_t	y;
	uint16_t	alpha;		/**< 0-no alpha, 255-local alpha, else global */
}
lcdc_flt_t, *lcdc_flt_p;
/*********************2011-03-01 end****************************/

typedef struct _magusFLT
{
        uint32_t        fwaddr[4];        /**< Display Start Address */
        uint32_t        stride; 		/**< Virtual Width in bytes */
        uint32_t        offset; 		/**< XY offsets, LCDC_OFS() */
        uint32_t        size;   		/**< Display Size, LCDC_SIZE() */
}magusFLT;


typedef struct _magusLCDC 
{
       uint32_t		id;			/* 0x0000, lcd device identification */
       uint32_t		res[20];		//04-50, not use
       uint32_t		fwcfg[2];	/*0x0054-0x0058, Floating window 1&2 config*/
       uint32_t   	maddr[4];     /**< Main Window Display Start Address */
       uint32_t   	mstride;        /**< Main Window Virtual Width in bytes */
       uint32_t   	msize;          /**< Main Window Display Size, LCDC_SIZE() */
       magusFLT	fw[4];         	/* floating windows */
}MagusLCDC;

/* magus alpha blending format */
typedef enum {
    DISABLE_ALPHA_BLENDING 	= 1, /* no alpha */
    ENABLE_PIXEL_ALPHA		= 2, /* pixel alpha */
    ENABLE_GLOBAL_ALPHA		= 3, /* global alpha */
} MagusAlpha;

/* magus lcdc registers */
typedef struct _magus {
    MagusLCDC	*lcdc;			/* magus lcdc registers */
    int		fd[MAX_FW_NUM];		/* fb handler */
    CARD32	fbBase[MAX_FW_NUM];	/* fb physical base address */
    CARD32	fbSize[MAX_FW_NUM];	/* fb size */
} Magus;

/* magus floating window information */
typedef struct _magusOverlay {
    CARD8	*fb[MAX_FW_NUM];	/* fb in user space */
    CARD8	alphaValue;		/* alpha value 0 (transparent) ~ 255 (opaque) */
    MagusAlpha	alphaFormat;		/* alpha format: no alpah, pixel alpha or global alpha */
    int		index;			/* floating windwo index: FLOAT_WIN_ONE or FLOAT_WIN_TWO */
    int		bitsPerPixel;		/* bits per pixel */
    int		width;			/* floating window width */
    int		height;			/* floating window height */
    int		stride;			/* floating window stride -- number of bytes in one line*/
    int		xoff;			/* floating window x coordinate to main window */
    int		yoff;			/* floating window y coordinate to main window */
    int		vwidth;			/* if floating window area is out of display, need to resize & remeber the virtal width*/
    int		vheight;			/* if floating window area is out of display, need to resize & remeber the virtal height*/
    void	*driver;		/* magus lcdc hardware driver */
} MagusOverlay;


typedef struct _magus *MagusPtr;
typedef struct _magusOverlay *MagusOverlayPtr;

//#define SetupMagus(s) MagusLCDC *lcdc = (s)->lcdc

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: pOverlay, MagusOverlay structure			*
 * reture value: TRUE if succeed, or FALSE			   	*
 * this function mapp the lcdc registers to user space, and allocate	*
 * memory for the floating windows, it should be the first function to	*
 * call before any other operation to the floating windows and should be*
 * called only once.							*
 * **********************************************************************/

BOOL magusOverlayInit (MagusPtr pMagus, MagusOverlayPtr pOverlay);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: pOverlay, MagusOverlay structure			*
 * this function is used to initialized a floating window specified by 	*
 * index, whose value is either FLOAT_WIN_ONE or FLOAT_WIN_TWO. Before	*
 * calling this function, all the parameters expect fb in pOverlay	*
 * must be filled with some proper values.				*
 * **********************************************************************/

void magusFloatWinInit (MagusPtr pMagus, MagusOverlayPtr pOverlay);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: enable, if TRUE the floatin window index is visible	*
 * 		   or it's invisible.					*
 * parameter [in]: index, specify which floating window to enable or	*
 * 		   disable						*
 * this function enables or disable a floating window specified by index*
 * if enabled, the floating window is visible, or it's invisible	*
 * **********************************************************************/

void magusFloatWinEnable (MagusPtr pMagus, BOOL enable, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: x, the x coordination of a floating window in	*
 * 		   relation to the main window (in pixel)		*
 * parameter [in]: y, the y coordination of a floating window in        *
 *                 relation to the main window (in pixel)		*
 * parameter [in]: index, specify which floating window to be moved	*
 * this functions moves a floating window specified by index to (x, y)	*
 * **********************************************************************/

void magusSetFloatWinCoordinate (MagusPtr pMagus, int x, int y, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: alpah, the global alpha value (0 <= alpha <=	0xff	*
 * 		   0, transparent, 0xff, opaque.			*
 * parameter [in]: index, specify which floating window's alpha value to*
 * 		   to be changed					*
 * this function sets the global alpha value of a floating window	*
 * specified by index							*
 * **********************************************************************/

void magusSetGlobalAlpha (MagusPtr pMagus, CARD8 alpha, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: index, specify which floating window's alpha value to*
 * 		   get							*
 * this function gets the global alpha value of a floating window 	*
 * specified by index							*
 * **********************************************************************/

CARD8 magusGetGlobalAlpha (MagusPtr pMagus, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: pOverlay, MagusOverlay structure			*
 * this function unmaps the registers and memory allocated before	*
 * it should be the last function to call before exiting from		*
 * applications								*
 * **********************************************************************/

void magusOverlayFini (MagusPtr pMagus, MagusOverlayPtr pOverlay);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: alphaFormat, the alpha format: DISABLE_ALPHA_BLENDING*
		   ENABLE_PIXEL_ALPHA or ENABLE_GLOBAL_ALPHA		*
 * parameter [in]: index, floating window index				*
 * it sets the alpha format of floating window index.			*
 ************************************************************************/

void magusSetAlphaFormat (MagusPtr pMagus, MagusAlpha alphaFormat, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: index, floating window index				*
 * it returns the alpha format of the floating window index		*
 * **********************************************************************/

MagusAlpha magusGetAlphaFormat (MagusPtr pMagus, int index);

/************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer	*
 * parameter [in]: width, floating window width				*
 * parameter [in]: height, floating window height			*
 * parameter [in]: floating window stride				*
 * parameter [in]: floating window index				*
 * it sets/resizes the size of a floating window and changes the stride	*
 * of it accordingly							*
 ************************************************************************/


void magusSetFloatWinSize (MagusPtr pMagus, int width, int height, int stride, int index);

/***************************Dayton 20110921*********************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer *
 * parameter [in]: x, floating window x             *
 * parameter [in]: y, floating window y           *
 * parameter [in]: floating window index                *
 ************************************************************************/
void magusSetFloatWinOffset (MagusPtr pMagus, int x, int y, int index);


/*************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer
 * parameter [in]: index, the floating window index
 * it returns the color format of a floating window (i.e. RGB1555, RGB565, etc
 *************************************************************************/

CARD8 magusGetColorFormat (MagusPtr pMagus, int index);

/*************************************************************************
 * parameter [in]: pMagus, the mapped lcdc registers struct pointer
 * parameter [in]: index, the floating window index
 * parameter [in]: colorFormat, the color format (i.e. MAGUS_COLOR_RGB1555,
 *                 MAGUS_COLOR_RGB565, MAGUS_COLOR_RGB8888, etc)
 * it changes the color format of a floating window (i.e. RGB1555, RGB565, etc
 *************************************************************************/

void magusSetColorFormat (MagusPtr pMagus, CARD8 format, int index);

#ifdef __cplusplus
}
#endif

#endif /* _MAGUSOVERLAY_H_ */

