/*******************************************************************************
Solomon Systech Pte Ltd.

No part of this document can be copied, or reproduced, or translated, or
stored on electronic storage, or in other forms, without the written agreement
of Solomon Systech Pte Ltd.
********************************************************************************
File:    sslLcdcApi.h


Descriptions:
This header file defines the external APIs and parameters to control LCDC frame buffer and output size.

Notes:

Modification History

*******************************************************************************/
#ifndef SSLLCDCAPI_H
#define SSLLCDCAPI_H



#ifdef __cplusplus
extern "C"
{
#endif
typedef enum
{
        LCDC_PANEL_TFT_SEQUEN,
        LCDC_PANEL_TFT_FILTER,
        LCDC_PANEL_SMART,
        LCDC_PANEL_EBOOK,
}
lcdc_panel_e;

typedef struct
{
        /* panel interface */
        unsigned char		panel;
        unsigned char          pins;                  	/**< TFT:9,12,18,24 */

        /* panel timing */
        unsigned short         hdisp_pos;			/**<  Hsync-den time = Hsync period - display period - front proch */
        unsigned short         hdisp_len;			/**< Hsync period */
        unsigned short         vdisp_pos;			/**< Vsync-den time = Vsync period - display period - front proch */
        unsigned short         vdisp_len;			/**< Vsync period */
        unsigned short         hsync_pos;		/**< HSYNC pixel position */
        unsigned short         hsync_sub;		/**< HSYNC sub pixel position, it should be 0 if no specific requirement from the display device */
        unsigned short         hsync_len;			/**< HSYNC total pixels */
        unsigned short         vsync_pos;		/**< VSYNC line position */
        unsigned short         vsync_len;			/**< VSYNC total lines */
	/*These 4 vaules as below should be 0 if no specific requirement from the display device */
        unsigned short         vsync_start;		/**< VSYNC pixel position */
        unsigned short         vsync_end;		/**< VSYNC pixel end position */
        unsigned short         lshift_pos;			/**< LSHIFT pixel position */
        unsigned short         lshift_len;			/**< LSHIFT total pixels */
		
	/* panel display size */
	//unsigned short 	   panel_width;		/**< Display width in pixels*/
	//unsigned short 	   panel_height;		/**< Display height in pixels */
		
	/* display refresh speed */
	//unsigned char 	    fps;				/**< Frame Per Second */
		
        /* panel signalling & storage flags */
        unsigned char          hsync_high : 1;		/**< 0-1, HSYNC aviable high, 1 is for active high */
        unsigned char          vsync_high : 1;		/**< 0-1, VSYNC aviable high, 1 is for active high */
        unsigned char          lshift_rise : 1;		/**< 0-1, LSHIFTP polarity, 1 is for rising edge sampling */
        unsigned char          den_high : 1;		/**< 0-1, DATA signal aviable high, 1 is for active high */
        
        unsigned short 	   panel_width;/**< Display width in pixels*/
        unsigned short 	   panel_height;/**< Display height in pixels */
        unsigned char 	    fps;				/**< Frame Per Second */

}
ssl_panel_t, *ssl_panel_p;


/**********Example Parameter Set for 720P HDMI ***********************/
//static ssl_panel_t lcdc_720p =
//{
//	.panel = LCDC_PANEL_TFT_FILTER,
//	.pins = 24,					// 24-bit data line
//	.hdisp_pos = 260,
//	.hdisp_len = 1650,
//	.vdisp_pos = 25,
//	.vdisp_len = 750,
//	.hsync_pos = 0,
//	.hsync_sub = 0,
//	.hsync_len = 40,
//	.vsync_pos = 0,
//	.vsync_len = 5,
//	.vsync_start = 0,
//	.vsync_end = 0,
//	.lshift_pos = 0,
//	.lshift_len = 0,
//	.hsync_high = 0,
//	.vsync_high = 0,
//	.lshift_rise = 1,
//	.den_high = 1,
//	.panel_width = 1280,
//	.panel_height = 720,
//	.fps = 67,
//};

/**********Example Parameter Set for 10800P HDMI ***********************/
//static ssl_panel_t lcdc_1080p =
//{
//	.panel = LCDC_PANEL_TFT_FILTER,
//	.pins = 24,					// 24-bit data line
//	.hdisp_pos = 192,
//	.hdisp_len = 2200,
//	.vdisp_pos = 41,
//	.vdisp_len = 1125,
//	.hsync_pos = 0,
//	.hsync_sub = 0,
//	.hsync_len = 44,
//	.vsync_pos = 0,
//	.vsync_len = 5,
//	.vsync_start = 0,
//	.vsync_end = 0,
//	.lshift_pos = 0,
//	.lshift_len = 0,
//	.hsync_high = 0,
//	.vsync_high = 0,
//	.lshift_rise = 0,
//	.den_high = 1,
//	.panel_width = 1920,
//	.panel_height = 1080,
//	.fps = 34,
//};

/*********Example Parameter Set 7" 800x480 LCD panel **********************/



/*******************************************************************************
Function   : sslLcdcFrameBufSet()
Description: Set lcdc Framebuffer (/dev/fb0) width, and height.
Note       : The configurable width and height must be less than the Max frame buffer size, E.g. 1280x800
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/
int sslLcdcFrameBufSet(
	unsigned int FbWidth,        // frame buffer width in pixels
	unsigned int FbHeight);     // frame buffer height in pixels
	

/*******************************************************************************
Function   : sslLcdcDisplayOutputSet()
Description: Set lcdc output display panel parameters.
Note       : The panel timing and other parameters must be aligned to panel's specs.
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/

int sslLcdcDisplayOutputSet(
	ssl_panel_p PanelParams); // output panel parameters including width and height
	

/*******************************************************************************
Function   : sslLcdcDisplayOutputEnable()
Description: Enable or Disable the lcdc output.
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/
int sslLcdcDisplayOutputEnable(
	unsigned char enable_flag);  // 1: enable lcdc output, 0: disable lcdc output


#ifdef __cplusplus
}
#endif
#endif  //SSLLCDCAPI_H


