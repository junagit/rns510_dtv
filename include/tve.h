#ifndef _TVE_KUGAN_
#define _TVE_KUGAN_

/**
@file 		tve.h
@author		kuganv@solomon-systech.com
@version	0.1
@todo		
*/
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/** TV Out API returns */
typedef enum
{
	TVE_ERR_NONE 	=  0,	/**< successful */
	TVE_ERR_HW 	= -1,	/**< hardware error */
	TVE_ERR_PARM 	= -2,	/**< parameter error */
	TVE_ERR_MEM 	= -3,	/**< insufficient memory */
	TVE_ERR_CFG 	= -4,	/**< configuration error */
}
tve_err_e;

/** TVENC Input Formats */
typedef enum
{
	TVE_FMT_24BIT_YUV444	 	= 0x0,	/**< 24 Bit YUV444 Format */
	TVE_FMT_16BIT_YUV422 		= 0x1,	/**< 16 Bit YUV422 Format*/
	TVE_FMT_8BIT_YUV422 		= 0x2,	/**< 8  Bit YUV422 Format*/
	TVE_FMT_8BIT_BT656 		= 0x3,	/**< 8  Bit BT656  Format*/
}
tve_vfmt_e;


/** TV-Out Output Signal Formats */
typedef enum
{
	TVE_NTSC 		= 0x0,	/**< NTSC  */
	TVE_PAL 		= 0x1,	/**< PAL B/D/G/H/I */
	TVE_PAL_M 		= 0x2,	/**< PAL M */
	TVE_PAL_Nc	 	= 0x3,	/**< PAL Nc */
}
tve_vstandard_e;



/** TVENC  Input mode */
typedef struct
{
	tve_vfmt_e	input_format; 	/**< Input video format */
	int 		clk_sel;	/**< If 0, internal clock else external clock */ 
	int 		csc_disable;	/**< Disable internal color space conversion */
	int		bs_enable;	/**< Enable bluscreen output */
	int		mon_enable;	/**< Enable monochrome output */
	int 		icb_enable;	/**< Enable internal color bar generation */	
}
tve_inp_mode_t;

/** TVENC  Output mode */
typedef struct
{
	tve_vstandard_e	video_standard;	/**< Output video standard */
	int 		pedestal_ctl;
	int 		video_out_fmt;
	int 		dac0_pd;
	int 		dac1_pd;
	int 		dac_bg_pd;
	int 		dac_clk_inv;
}
tve_out_mode_t;

/** TVENC  Video Processing description */
typedef struct
{
	int 	luminance_filter;
	int 	chrominance_filter;
	int 	clipping_disable;
	int 	luma_delay;
	int 	sch_phase_reset_mode;
}
tve_vprocessing_t;


/** TVENC  Video Timing */
typedef struct
{
	int 	hsync_delay;
	int 	video_timing_mode;
	int 	field_polarity;
	int 	vsync_polarity;
	int 	hsync_polarity;
	int 	blank_control;
}
tve_vtiming_t;


typedef struct 
{
	uint16_t y;
	uint16_t u;
	uint16_t v;
}
tve_cgain_t;


typedef struct
{
	uint16_t	sync;
	uint16_t	blank;
	uint16_t	pedestal;
	uint16_t	burst;
}
tve_singal_t;


/** TVENC  Context */
typedef struct
{
	uint32_t		base_addr;
	tve_inp_mode_t 		imode;
	tve_out_mode_t 		omode;
	uint32_t 		subcarrier_freq;
	uint32_t 		subcarrier_phase;
	tve_vprocessing_t 	processing;
	tve_vtiming_t		timing;
	tve_cgain_t		cgain;
	tve_singal_t		signal;
}
tve_t;

tve_err_e tve_init (tve_t * tve);
/**<
initialize 	TVE & context
@param[in] 	tve	context
@return		tve_err_t
*/

tve_err_e tve_exit (tve_t * tve);
/**<
Exit and reset 	TVE
@param[in] 	tve	context
@return		tve_err_t
*/

tve_err_e tve_cfg (tve_t * tve);
/**<
Configure  TVE
@param[in] 	tve	context
@return		tve_err_t
*/

#endif	/* _TVE_KUGAN_ */
