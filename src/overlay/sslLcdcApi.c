/*******************************************************************************

SSL LCDC Interface

*******************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "sslLcdcApi.h"
    

#define SSLFBIOGET_TRAP 		0x46F2
#define SSLFBIOSET_TRAP 		0x46F3
#define SSLFBIOGET_PADDING 	0x46F4
#define SSLFBIODISABLE_WIN  0x46F9
#define SSLFBIOENABLE_WIN   0x46FA
#define SSLFBIOSETSIZE_WIN  0x46FD
#define SSLFBIOSETPANNEL_WIN 0x46FE
#define SSLFBIOENABLEOUTPUT_WIN 0x46FF

typedef struct
{
    unsigned short top;
    unsigned short bottom;
    unsigned short height;
    unsigned long padding;
}
sslfb_trap_t;

ssl_panel_t lcdc_AT070TN93 =
{
        .panel_width = 720,
        //.panel_height = 564,
         .panel_height = 576,
	.panel = LCDC_PANEL_TFT_FILTER,	// panel type
        .pins = 24,					// 24-bit data line
        .hdisp_pos = 76,//210,			// Hsync-den time = Hsync period - display period - front proch = 525 - 480 - 2
        .hdisp_len = 1056,//1056,			// Hsync period
        .vdisp_pos = 23,//22,			// Vsync-den time = Vsync period - display period - front proch = 286 - 272 - 2
        .vdisp_len = 625,//525,			// Vsync period
        .hsync_pos = 0,
        .hsync_sub = 0,
        .hsync_len = 64,//20,				// set 0-2
        .vsync_pos = 0,
        .vsync_len = 6,//10,				// set 0-2
        .vsync_start = 0,
        .vsync_end = 0,
        .lshift_pos = 0,
        .lshift_len = 0,
        .hsync_high = 0,
        .vsync_high = 0,
        .lshift_rise = 1,
        .den_high = 1,
        .fps = 50,
        
               
};

/*******************************************************************************
Function   : sslLcdcFrameBufSet()
Description: Set lcdc Framebuffer (/dev/fb0) width, and height.
Note       : The configurable width and height must be less than the Max frame buffer size, E.g. 1280x800
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/
int sslLcdcFrameBufSet(
	unsigned int FbWidth,        // frame buffer width in pixels
	unsigned int FbHeight)     // frame buffer height in pixels
{
	int dev_fd,ret;
	dev_fd = open("/dev/fb0",O_RDWR);
	if(dev_fd < 0)
		return dev_fd;
	unsigned int wh = ((unsigned short)FbHeight)<<16 | (unsigned short)FbWidth;
/*
	printf("wh=0x%x.\n",wh);
*/
	ret = ioctl(dev_fd,SSLFBIOSETSIZE_WIN,&wh);
  close(dev_fd);
	return ret;
}


/*******************************************************************************
Function   : sslLcdcDisplayOutputSet()
Description: Set lcdc output display panel parameters.
Note       : The panel timing and other parameters must be aligned to panel's specs.
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/

int sslLcdcDisplayOutputSet(
	ssl_panel_p PanelParams)    // output panel parameters including width and height
{
	sslfb_trap_t k;
	unsigned long padding;
	int dev_fd,ret;
	dev_fd = open("/dev/fb0",O_RDWR);
	if(dev_fd < 0)
		return dev_fd;
	k.top = PanelParams->panel_width;
	k.bottom = PanelParams->panel_width;
	k.height = PanelParams->panel_height;
	ioctl(dev_fd,SSLFBIOGET_PADDING,&padding);
	k.padding = padding;
	
	if(ioctl(dev_fd,SSLFBIOSETPANNEL_WIN,PanelParams) != 0)
		goto fauil;
	if(ioctl(dev_fd,SSLFBIOSET_TRAP,&k) != 0)
		goto fauil;
		
	close(dev_fd);
	return 0;
fauil:
	close(dev_fd);
	return -1;
	
}


/*******************************************************************************
Function   : sslLcdcDisplayOutputEnable()
Description: Enable or Disable the lcdc output.
Arguments  : 
Return     : 0 on success, else error code.
*******************************************************************************/
int sslLcdcDisplayOutputEnable(
	unsigned char enable_flag)  // 1: enable lcdc output, 0: disable lcdc output
{
	int dev_fd,ret;
	dev_fd = open("/dev/fb0",O_RDWR);
	if(dev_fd < 0)
		return dev_fd;
		
	if(ioctl(dev_fd,SSLFBIOENABLEOUTPUT_WIN,&enable_flag) != 0){
		close(dev_fd);
		return -1;
	}
	close(dev_fd);
	return 0;	
}



