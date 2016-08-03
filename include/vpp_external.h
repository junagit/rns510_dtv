
/**
@file		vpp_external.h 
@author		kelintu@solomon-systech.com
@brief external interface to user
@date 2010-11-29
@version	0.1

*/
/* overlay feature not exist in VPP-L/VPPHD */
/* only one channel in VPPHD */

#ifndef VPP_EXTERNAL_H
#define VPP_EXTERNAL_H

#define VPP_F_ALL	~0x0
#define VPP_F_INADDR	1		/**< re-configure input image buffer address */
#define VPP_F_OUTADDR	1<<1		/**< re-configure output image buffer address */
#define VPP_F_ADDR	3
#define VPP_F_IN	1<<2		/**< re-configure input video only */
#define VPP_F_OUT	1<<3		/**< re-configure output video only */

#define VPP_F_MODULES	1<<6			/**< manual enable/disable of each module */

#define VPP_F_AT 		1<<9		/**< anti-tearing configuration */

#define VPP_KERNEL_API_ENABLE

/* enumeration */

typedef enum
{
	VPP_ERR_NONE = 0,		/**< sucessful */
	VPP_ERR_HW = -1,		/**< hardware error */
	VPP_ERR_PARM = -2,		/**< parameter error */
	VPP_ERR_CFG = -3,		/**< configuration error */
	VPP_ERR_MEM = -4,		/**< memory allocation error */
	VPP_ERR_SUPP = -5,		/**< not supported err */

	VPP_ERR_JOB = -6,		/**< adding job err */
	VPP_ERR_TRIGGER = -7,		/**< trigger job err */

	VPP_ERR_RESET = -8,		/**< reset failed */

}
vpp_err_e;

typedef enum
{
	VPP_PIC_YUV444P	= 0,		/**< YUV444 planar */
	VPP_PIC_YUV422P	= 1,		/**< YUV422 planar */
	VPP_PIC_YUV420P	= 2,		/**< YUV420 planar */
	VPP_PIC_YUV411P	= 3,		/**< YUV411 planar */
	VPP_PIC_YUV400P	= 4,		/**< YUV400 planar */
	VPP_PIC_YUV444I	= 5,		/**< YUV444 interleave */
	VPP_PIC_YUV422I_YUYV = 6,	/**< YUV422 interleave (YUYV) */
	VPP_PIC_YUV422I_UYVY = 7,	/**< YUV422 interleave (UYVY) */
	VPP_PIC_YpUVi444 =8,
	VPP_PIC_YpUVi422 =9,
	VPP_PIC_YpUVi420 =10,

	VPP_PIC_RGB32	= 16,		/**< RGB 32bpp */

	VPP_PIC_RGB24	= 17,		/**< RGB 24bpp packed*/
	VPP_PIC_RGBp8	= 18,		/**< RGB plannar 8bit */

	VPP_PIC_RGB16	= 19,		/**< RGB 16bpp */

}
vpp_pic_e;


typedef enum
{
	VPP_DISPLAY_FIT	= 0,	/*default setting*/		/**< fit output to width and height */

	VPP_DISPLAY_CROP = 1,							/**< crop the input to maintain the aspect ratio */
	VPP_DISPLAY_PAD	= 2								/**< pad the output to maintain the aspect ratio */
}
vpp_display_e;


typedef enum
{
	VPP_MODE_SCA	= 0,		/**< SCA only */
	VPP_MODE_CSC	= 1, 		/**< CSC only */
	VPP_MODE_SCA_CSC = 2,		/**<  SCA->CSC */
	VPP_MODE_CSC_SCA = 3		/**<  CSC->SCA */
}
vpp_mode_e;

typedef enum
{
	VPP_TRIG_POLL	= 3		/**< polling mode (need to manually enable the channel for each frame to be processed) */
}
vpp_trigger_e;

typedef enum
{
	VPP_HV_ALIGN_0 = 0,												/**< set h/v alignment = 1 */
	VPP_HV_ALIGN_1 = 1,		/* default setting*/					/**< set h/v alignment = 0 */
}
vpp_hv_align_e;

typedef enum
{

	VPP_MODULE_SYSTEM,	/* default setting*/	/**< driver decide whether to enable/disable the module; if user LUT exist then use user LUT, else use default LUT. */
	VPP_MODULE_DEFAULT,							/**< driver decide whether to enable/disable the module; use default LUT. */
	VPP_MODULE_ENABLE,							/**< force enable the module; if user LUT exist then use user LUT, else use default LUT. */
	VPP_MODULE_DISABLE,							/**< force disable the module */
}
vpp_module_state_e;


typedef enum
{
	VPP_TRIGGER_0	= 0,  /**< software trigger,default */
	VPP_TRIGGER_1,		  /**< DVHD/trigger_eof0_bank0 */
	VPP_TRIGGER_2,		  /**< DVHD/trigger_eof0_bank1*/
	VPP_TRIGGER_3,		  /**< DVHD/trigger_eof1_bank0*/
	VPP_TRIGGER_4,		  /**< DVHD/trigger_eof1_bank1*/
	VPP_TRIGGER_5,		  /**< DVHD/trigger_eos */
	VPP_TRIGGER_6,		  /**< VPU*/
	VPP_TRIGGER_7,		  /**< GPIO*/

	VPP_TRIGGER_ALL,
}
vpp_trigger_src_e;

typedef enum
{
	VPP_THROTTLING_0	= 0,  /**< software trigger,default */
	VPP_THROTTLING_1,  /**< LCDC1 */
	VPP_THROTTLING_2,  /**< LCDC2 */
	VPP_THROTTLING_3,  /**< TVOUT */
	VPP_THROTTLING_4,  /**< reserved */
	VPP_THROTTLING_5,  /**< reserved */
	VPP_THROTTLING_6,  /**< reserved */
	VPP_THROTTLING_7,  /**< reserved */


}
vpp_throttling_src_e;

typedef enum
{
	VPP_MODULE_CSC0 = 0,	/**< input csc */
	VPP_MODULE_CSC1,		/**< output csc */
	VPP_MODULE_LPF0,		/**< low pass filter for component 0 */
	VPP_MODULE_LPF1,		/**< low pass filter for component 1 */
	VPP_MODULE_LPF2,		/**< low pass filter for component 2 */
	VPP_MODULE_HSCA0,		/**< h scalar for component 0  */
	VPP_MODULE_HSCA1,		/**< h scalar for component 1  */
	VPP_MODULE_HSCA2,		/**< h scalar for component 2  */
	VPP_MODULE_VSCA0,		/**< v scalar for component 0  */
	VPP_MODULE_VSCA1,		/**< v scalar for component 1  */
	VPP_MODULE_VSCA2,		/**< v scalar for component 2  */
}
vpp_module_e;

typedef enum
{
	/* slice mode*/
	VPP_NON_SLICE_MODE,
	VPP_SLICE_MODE,

}
vpp_smode_e;

typedef enum
{
	/* job mode */
	VPP_JMODE_BATCH,
	VPP_JMODE_SINGLE,

}
vpp_jmode_e;

typedef enum
{
	/* trigger mode */
	VPP_TMODE_ONESHOT,
	VPP_TMODE_CONTINUOUS,

}
vpp_tmode_e;

typedef enum
{
	/* job type, first_job_flag / last_job_flag */
	VPP_JOB_FIRST, /* 1 / 0 */
	VPP_JOB_LAST, /* 0 / 1 */
	VPP_JOB_MIDDLE, /* 0 / 0 */
	VPP_JOB_POINT_TO_FIRST, /* 0 / 0, JNEXT = JFIRST */
	VPP_JOB_FIRST_LAST, /* 1 / 1 */
}
vpp_jtype_e;

typedef enum
{
	/* vpp call blocking method */
	VPP_NONBLOCKING = 0,
	VPP_BLOCKING,
}
vpp_block_method_e;


typedef struct
{
	//refer to VPPHD Module Spec for description
	uint32_t		jd_id;
	uint32_t		jd_cfg; 

	uint32_t	jd_c0isize;	
	uint32_t	jd_c0osize;
	uint32_t	jd_c1isize;	
	uint32_t	jd_c1osize;
	uint32_t	jd_c2isize;	
	uint32_t	jd_c2osize;

	uint32_t		jd_ic0;
	uint32_t		jd_ic1;
	uint32_t		jd_ic2;
	uint32_t		jd_ic3;
	uint32_t		jd_ic4;
	uint32_t		jd_ic5;
	uint32_t		jd_ic6;
	uint32_t		jd_ic7;

	uint32_t		jd_oc0;
	uint32_t		jd_oc1;
	uint32_t		jd_oc2;
	uint32_t		jd_oc3;
	uint32_t		jd_oc4;
	uint32_t		jd_oc5;
	uint32_t		jd_oc6;
	uint32_t		jd_oc7;

	uint32_t		jd_c0l0;
	uint32_t		jd_c0l1;
	uint32_t		jd_c1l0;
	uint32_t		jd_c1l1;
	uint32_t		jd_c2l0;
	uint32_t		jd_c2l1;

	uint32_t		jd_c0h0;
	uint32_t		jd_c0h1;
	uint32_t	jd_c0hl0;
	uint32_t	jd_c0hl1;
	uint32_t	jd_c0hl2;
	uint32_t	jd_c0hl3;
	uint32_t	jd_c0hl4;
	uint32_t	jd_c0hl5;
	uint32_t	jd_c0hl6;
	uint32_t	jd_c0hl7;
	uint32_t	jd_c0hl8;
	uint32_t	jd_c0hl9;

	uint32_t		jd_c1h0;
	uint32_t		jd_c1h1;
	uint32_t	jd_c1hl0;
	uint32_t	jd_c1hl1;
	uint32_t	jd_c1hl2;
	uint32_t	jd_c1hl3;
	uint32_t	jd_c1hl4;
	uint32_t	jd_c1hl5;
	uint32_t	jd_c1hl6;
	uint32_t	jd_c1hl7;
	uint32_t	jd_c1hl8;
	uint32_t	jd_c1hl9;

	uint32_t		jd_c2h0;
	uint32_t		jd_c2h1;
	uint32_t	jd_c2hl0;
	uint32_t	jd_c2hl1;
	uint32_t	jd_c2hl2;
	uint32_t	jd_c2hl3;
	uint32_t	jd_c2hl4;
	uint32_t	jd_c2hl5;
	uint32_t	jd_c2hl6;
	uint32_t	jd_c2hl7;
	uint32_t	jd_c2hl8;
	uint32_t	jd_c2hl9;

	uint32_t		jd_c0v0;
	uint32_t		jd_c0v1;
	uint32_t	jd_c0vl0;
	uint32_t	jd_c0vl1;
	uint32_t	jd_c0vl2;
	uint32_t	jd_c0vl3;
	uint32_t	jd_c0vl4;
	uint32_t	jd_c0vl5;
	uint32_t	jd_c0vl6;
	uint32_t	jd_c0vl7;
	uint32_t	jd_c0vl8;
	uint32_t	jd_c0vl9;

	uint32_t		jd_c1v0;
	uint32_t		jd_c1v1;
	uint32_t	jd_c1vl0;
	uint32_t	jd_c1vl1;
	uint32_t	jd_c1vl2;
	uint32_t	jd_c1vl3;
	uint32_t	jd_c1vl4;
	uint32_t	jd_c1vl5;
	uint32_t	jd_c1vl6;
	uint32_t	jd_c1vl7;
	uint32_t	jd_c1vl8;
	uint32_t	jd_c1vl9;

	uint32_t		jd_c2v0;
	uint32_t		jd_c2v1;
	uint32_t	jd_c2vl0;
	uint32_t	jd_c2vl1;
	uint32_t	jd_c2vl2;
	uint32_t	jd_c2vl3;
	uint32_t	jd_c2vl4;
	uint32_t	jd_c2vl5;
	uint32_t	jd_c2vl6;
	uint32_t	jd_c2vl7;
	uint32_t	jd_c2vl8;
	uint32_t	jd_c2vl9;

	uint32_t		jd_dma0;
	uint32_t		jd_dma1;
	uint32_t		jd_dma2;
	uint32_t		jd_dma3;
	uint32_t		jd_dma4;
	uint32_t		jd_dma5;
	uint32_t	jd_dma0c;
	uint32_t	jd_dma1c;
	uint32_t	jd_dma2c;
	uint32_t	jd_dma3c;
	uint32_t	jd_dma4c;
	uint32_t	jd_dma5c;

	uint32_t	status; /* 0= this jd slot is free, 1 = jd slot in use */
	uint32_t	next;   /*mapped address of next job descriptor in job list  */

	//total 464 bytes
}
vpp_job_descriptor_t, *vpp_job_descriptor_p;


/* structure */

typedef struct
{
	uint16_t	x;		/**< x coordinate */
	uint16_t	y;		/**< y coordinate */
}
vpp_offset_t;

typedef struct
{
	uint16_t	r;		/**< Red, Cr */
	uint16_t	g;		/**< Green, Y */
	uint16_t	b;		/**< Blue, Cb */
}
vpp_coffset_t;


typedef struct
{
	uint16_t	w;		/**< width */
	uint16_t	h;		/**< height */
}
vpp_size_t;


typedef struct
{
	vpp_size_t	size;		/**< video picture size in pixel*/
	uint16_t	stride_y;	/**< page width for Y in bytes, must in multiple of 8-bytes, 0 means same as width */
	uint16_t	stride_cb;	/**< page width for Cb in bytes, must in multiple of 8-bytes, 0 means same as width */
	uint16_t	stride_cr;	/**< page width for Cr in bytes, must in multiple of 8-bytes, 0 means same as width */
	vpp_pic_e	format;		/**< picture format */
	vpp_size_t	pixel;		/**< pixel aspect ratio */
	uint32_t	ybuf;	/**< physical address for component0 */
	uint32_t	cbbuf;	/**< physical address for component1 */
	uint32_t	crbuf;	/**< physical address for component2 */
}
vpp_video_t;


typedef struct
{
	/* available modules for configuration: */
	vpp_module_state_e csc0; /**< input csc */
	vpp_module_state_e csc1; /**< output csc */

	vpp_module_state_e lpf0; /**< low pass filter for component 0 */
	vpp_module_state_e lpf1; /**< low pass filter for component 1 */
	vpp_module_state_e lpf2; /**< low pass filter for component 2 */

	vpp_module_state_e hsca0; /**< h scalar for component 0  */
	vpp_module_state_e hsca1; /**< h scalar for component 1  */
	vpp_module_state_e hsca2; /**< h scalar for component 2  */
	vpp_module_state_e vsca0; /**< v scalar for component 0  */
	vpp_module_state_e vsca1; /**< v scalar for component 1  */
	vpp_module_state_e vsca2; /**< v scalar for component 2  */

	//scaling alignment for each component
	vpp_hv_align_e	hsca0_align;
	vpp_hv_align_e	hsca1_align;
	vpp_hv_align_e	hsca2_align;
	vpp_hv_align_e	vsca0_align;
	vpp_hv_align_e	vsca1_align;
	vpp_hv_align_e	vsca2_align;

	//step start for each component
	int hsca0_start;
	int hsca1_start;
	int hsca2_start;
	int vsca0_start;
	int vsca1_start;
	int vsca2_start;
}
vpp_module_t;



typedef struct
{
	int b_enable; //0=disable, 1=enable
	int mode; /**< 0 = start at init_linenum+1, 1 = start when > init_linenum */
	vpp_throttling_src_e src;   /**< display throttling select */
	int init_linenum;           /**< 0 ~ 4095 */
}
vpp_at_cfg_t, *vpp_at_cfg_p;


typedef struct
{
	uint32_t	flag;		/**< VPP_F_XXX */
	vpp_trigger_e	trigger;	/**< processing trigger mode, parameter for compatitive (with old vpp/vpp-l interface) purpose only*/
	vpp_display_e	display;	/**< display format: fit, crop, pad*/
	vpp_video_t	input;		/**< input video for a channel */
	vpp_video_t	output;		/**< output video for this channel */

	vpp_at_cfg_t	anti_tearing;	/**< anti tearing setting */	

	vpp_module_t	modules;

}
vpp_cfg_t, *vpp_cfg_p;



typedef struct
{
	void		*reg;		/**< base address of registers. obsolete parameter in VPPHD*/	
	
	void		*jd_addr;		/**< job descriptor buffer base address */
	int			jd_size;			/**< job descriptor buffer total size, in bytes */
	
	int 		vpp_fd;			/**< file descriptor */
	
	int 		instance;       /**< instance NO. of current instance */
	
	int (*callback)(void);	/**< callback for signal handler */

	void		*priv;	/**< point to vpp driver global data structure */

}
vpp_t, *vpp_p;


typedef struct
{
	int instance;	
	vpp_smode_e smode;
	vpp_jmode_e jmode;
	vpp_tmode_e tmode;

	int		b_EOJ_signal;  //tell driver whether to enable EOJ interrupt and signal user

}
vpp_trigger_cfg_t, *vpp_trigger_cfg_p;

typedef struct
{
	int instance;	
	vpp_trigger_src_e trig;

	int job_id;
	vpp_jtype_e jtype;

}
vpp_job_t, *vpp_job_p;


typedef struct
{
	int instance;
	vpp_trigger_src_e trig;

	vpp_job_descriptor_t full_job;

}
vpp_job_full_t, *vpp_job_full_p;

typedef struct
{
	int instance;
	vpp_trigger_src_e trig;

	vpp_block_method_e block;

}
vpp_trigger_t, *vpp_trigger_p;

typedef struct
{
	int instance;
	int size;
	vpp_module_e module;
	char lut_buf[255];

}
vpp_user_lut_t, *vpp_user_lut_p;


typedef struct
{
	int instance;		/**< instance num */
	vpp_trigger_src_e trig;

	int job_id;
	int is_busy;
	int is_trigger_miss;

}
vpp_status_t, *vpp_status_p;




/************************ old APIs ***********************/

/**<
initialize vpp
*/
vpp_err_e vpp_init(vpp_p t);

/**<
start vpp processing, call after vpp_cfg
*/
vpp_err_e vpp_start(vpp_p t, uint8_t channel);

/**<
stop/suspend vpp processing; 
*/
vpp_err_e vpp_stop(vpp_p t, uint8_t channel);

/**<
configure vpp channel, except settings for sca lpf and cubic
*/
vpp_err_e vpp_cfg(vpp_p t, vpp_cfg_p cfg, uint8_t channel);


/**<
module exit, free internal memory
*/
vpp_err_e vpp_exit(vpp_p t);


/**<
new added blocking API
start vpp processing, call after vpp_cfg, vpp_lpf or vpp_cubic 
*/
vpp_err_e vpp_start_blocking(vpp_p t, uint8_t channel);



/************************ new APIs ***********************/

vpp_err_e vpphd_init(vpp_p vpp);
vpp_err_e vpphd_exit(vpp_p vpp);
vpp_err_e vpphd_trigger_cfg(vpp_p t,  vpp_smode_e smode, vpp_jmode_e jmode, vpp_tmode_e tmode, int b_EOJ_signal);
vpp_err_e vpphd_generic_cfg(vpp_p t,  vpp_cfg_p cfg);
vpp_err_e vpphd_add_job( vpp_p t, vpp_trigger_src_e trig_num, int job_id, vpp_jtype_e jtype);
vpp_err_e vpphd_add_job_full( vpp_p t, vpp_trigger_src_e trig_num, vpp_job_descriptor_p pJob);
vpp_err_e vpphd_trigger(vpp_p t, vpp_trigger_src_e trig_num, vpp_block_method_e block);
vpp_err_e vpphd_reset(vpp_p t, vpp_trigger_src_e trig_num);
vpp_err_e vpphd_user_LUT_cfg(vpp_p t, vpp_module_e module, const int *lut, int size);
vpp_err_e vpphd_stop(vpp_p t);
vpp_status_t vpphd_status(vpp_p t, vpp_trigger_src_e trig_num);




#endif
