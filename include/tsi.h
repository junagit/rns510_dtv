#ifndef _TSI_H
#define _TSI_H

/**< 
@file		tsi.h	- API for transport stream interface module
@author		chenkai@solomon-systech.com
@version	0.1
*/
#ifdef __cplusplus
extern "C"
{
#endif
    
#define TS_MODE_DMA	0	 /**< enable DMA mode */
#define TS_MODE_PIO	0x08	 /**< enable PIO mode*/
#define TS_FLTR_EN	0x04	/**< enable filter mode */
#define TS_FLTR_DIS	0	/**< disable filter mode*/

#define TS_188		0
#define TS_204		1
#define TS_208		2

#define MAX_PACKAGE_IN_NODE     60
#define CIRBUF_MAX_NO		5 /**< support 3 circular buffer, buf index 0,1,2*/

typedef enum
{
	TSI_ERR_NONE = 0,		/**<  sucessful */
	TSI_ERR_HW = -1,			/**<  hardware error */
	TSI_ERR_PARM = -2,		/**<  parameter error */
	TSI_ERR_CFG = -3,		/**<  configuration error */
	TSI_ERR_MEM = -4,		/**<  memory allocation error */
	TSI_ERR_SUPP = -5		/**<  not supported err */
}
tsi_err_e;


typedef struct  
{
	uint32_t	fid;			  /**<  filter id, 0 ~ (MAX_FID-1) */
	uint32_t	pid;			  /**<  pid */
	uint32_t	pkts_per_intr;  /**<  Packets number in each interrupt for this channel. 1 ~ (MAX_PACKAGE_IN_NODE) */
	uint32_t	buffer_index;   /**<  0 ~ (CIRBUF_MAX_NO-1) */	
} 
fcfg_t,*fcfg_p;


typedef struct  
{
	uint32_t	fid_0   	:1 ;	/**<filter id 1  enable = 1, disable = 0*/
	uint32_t	fid_1   	:1 ;	/**< filter id 2 */
	uint32_t	fid_2   	:1 ;	/**< filter id 3 */
	uint32_t	fid_3   	:1 ;	/**< filter id 4 */
	uint32_t	fid_4   	:1 ;	/**< filter id 5 */
	uint32_t	fid_5   	:1 ;	/**< filter id 6  */
	uint32_t	fid_6   	:1 ;	/**< filter id 7 */
	uint32_t	fid_7   	:1 ;	/**< filter id 8 */
	uint32_t	fid_8   	:1 ;	/**< filter id 9 */
	uint32_t	fid_9   	:1 ;	/**< filter id 10 */
	uint32_t	fid_10   	:1 ;	/**< filter id 11 */
	uint32_t	fid_11  	:1 ;	/**< filter id 12 */
	uint32_t	reserved  	:20 ;	
} 
fid_en_t;



typedef struct  
{
	fid_en_t	fid;		 /**< to enable/disable pids */
	uint32_t	reserved; /**< reserved*/	
} 
fen_t,*fen_p;


typedef struct _tsi_t
{
	void		*reg;		            /**< base address of registers */	
	uint32_t	pkt_sz;		            /**< TS_188, TS_204 or TS_208 */
	uint32_t	mode;			    /**< pio=TS_MODE_PIO,dma=TS_MODE_DMA */
	uint32_t	filter;			    /**< enable = TS_FLTR_EN, disable = TS_FLTR_DIS; */
	uint32_t	pio_buffer_length;     /**< only available for PIO mode, user need to copy data once buffer full */    
	uint32_t	circular_buffer_length[CIRBUF_MAX_NO];   /**< In TS packet, the value will be multipied by packet size in driver */	
	uint32_t   reserved;
}
tsi_t, *tsi_p;


typedef struct _tsi_info_t 
{
	uint32_t 	length;   				/**< prepared data length, in BYTE */
	uint32_t   start_addr;			/**< prepared data starting address */
	uint32_t   start_addr_virt;		/**< prepared data virtual starting address */
	uint16_t   hw_overflow;			/**< flag to indicate that hw overflows*/	
	uint16_t   sw_overflow;			/**< flag to indicate that cicular buffer overflows*/
	uint32_t	timeout;           		/**<  Timeout to return. in Second */
	uint32_t	more_data;			/**< flag to indicate how many packets left in this circular buffer, in BYTE*/
	uint8_t	buf_index;			/**< used for defensive check, the value should be the same as input index */
} 
tsi_info_t, *tsi_info_p;

typedef struct _tsi_info_rec_t 
{
	uint32_t 	length;   				/**< prepared data length, in BYTE */
	uint32_t	margin_sz;			/**< Margin size,in BYTES always tries to keep the margin between wt ptr and rd ptr */	
	uint32_t   start_addr;			/**< prepared data starting address */
	uint32_t   start_addr_virt;		/**< prepared data virtual starting address */
	uint32_t discard_length;		/**< length to indicate that discard length*/	
	uint32_t	more_data;			/**< flag to indicate how many packets left in this circular buffer for recording, in BYTE*/
	uint8_t	buf_index;			/**< used for defensive check, the value should be the same as input index */
} 
tsi_info_rec_t, *tsi_info_rec_p;






typedef struct _tsi_bitrate_t 
{
	uint32_t 	index;   
	uint32_t   bitrate;		
} tsi_bitrate_t, *tsi_bitrate_p;


typedef struct _tsi_bitrate_int_t 
{
	uint32_t 	index;
	uint32_t 	packets;   
	uint32_t   jiffies;		
	uint32_t	hz;
} tsi_bitrate_int_t, *tsi_bitrate_int_p;
/**<
For internal usage, float calculation need to be done in user space
*/



tsi_err_e tsi_init (tsi_p t);
/**<
Tsi initialization
@param[in]	t		tsi context
@return 	tsi_err_e
*/

tsi_err_e tsi_filter_cfg (fcfg_t *cfg);
/**< 
Tsi configaration. 
Users need to specify the 1)FID to be used; 2)the PID attached to the FID;3)Packets number in an interrupt for this channel. 4)Circular buffer index.
@param[in]	fcfg_t
@return 	tsi_err_e
@@@Please NOTE: This API call will leads to all circular buffers to be cleared.
*/

tsi_err_e tsi_filter_en (fen_p fen);
/**<
Tsi switcher. 
Users need to specify the the FIDs they want to enable/disable.
@param[in]	fen_p
@return 	tsi_err_e
@@@Please NOTE: The FID must be configured before it is enabled, otherwise the request will be rejected.
*/

tsi_info_p tsi_retrieve_data (uint32_t length, uint32_t timeout, uint8_t buf_index);
/**<
Tsi reader. 
Users need to specify the the length they want to read, and from which circular buffer. Timeout is in second and this API is a blocking call.
@param[in]	length, In BYTE
@param[in]	timeout, in Second
@param[in]	buf_index
@return 	tsi_info_p
@@@Please NOTE: User need to do memcpy after getting the data info.
*/

tsi_info_p tsi_retrieve_data_nb (uint32_t length,uint8_t buf_index,uint32_t timeout);
/**<
Tsi reader. 
Users need to specify the the length they want to read, and from which circular buffer. This API is a non-blocking call.
@param[in]	length, In BYTE
@param[in]	buf_index
@return 	tsi_info_p
@@@Please NOTE: User need to do memcpy after getting the data info.
*/

tsi_info_p tsi_retrieve_data_dma (uint32_t dest_addr, uint32_t length,uint32_t timeout,uint8_t buf_index);
/**<
Tsi DMA reader. 
Users need to specify the the length/dest_addr they want to transfer by DMA, and from which circular buffer. 
@param[in]	dest_addr
@param[in]	length, in BYTE
@param[in]	buf_index
@param[in]	timeout, in Second
@return 	tsi_info_p
@@@Please NOTE: This API is a blocking call.
*/

tsi_info_rec_p tsi_retrieve_data_nb_rec(uint32_t length,uint8_t buf_index,uint32_t margin);
/**<
Tsi recording reader. 
Users need to specify the the length, buffer index and the margin they want to keep between write ptr and recording read ptr.
@param[in]	length, in BYTE
@param[in]	buf_index
@param[in]	margin, in BYTE
@return 	tsi_info_rec_p
@@@Please NOTE: This API is a non-blocking call.
*/


tsi_err_e tsi_clear_buffer (uint32_t buffer_index);
/**<
Tsi clear specified circular buffer. 
Users need to specify the the index of circular buffer. 
@param[in]	buffer_index
@return 	tsi_err_e
*/

tsi_err_e tsi_retrieve_bitrate (tsi_bitrate_p p_bitrate_st);
/**<
To retrieve FID channel bitrate. 
Users need to specify the the index of circular buffer. 
@param[in/out]	p_bitrate_st.  index is to specify the fid index and bitrate will be returned.
@return 	tsi_err_e
@@@ Please NOTE: The statistics starts once tsi_filter_en is called. 
*/


tsi_err_e tsi_exit (void);
/**<
free resource and exit
*/

tsi_err_e tsi_restart(void);

unsigned long tsi_get_alloc_dma_phys_addr(void);

void init_gpio(void);
#ifdef __cplusplus
}
#endif

#endif

