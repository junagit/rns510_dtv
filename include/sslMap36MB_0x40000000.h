#ifndef __SSLMAP_H__
#define __SSLMAP_H__

#warning "PLATFORM 36MB @ 0x40000000"

//==============================================================================
//Mapm
//==============================================================================
#define SSL_ABB1_BASE                       (0x08000000)
#define SSL_ABB1_SIZE                       (0x100000)      //1 MB
#define SSL_ABB3_BASE			            (0x04100000)    //DV+VPPHD+VPU 
#define SSL_ABB3_SIZE				        (0x201000)      //16 KB (size includes DVHD,VPPHD & VPU)  
#define SSL_SDR_BASE_1			            (0x40000000)    
#define SSL_SDR_SIZE_1			            (0x2380000)     //36MB-512KB
#define SSL_SDR_BASE_2			            (0x48000000)    
#define SSL_SDR_SIZE_2			            (0x8000000)     //128 MB
#define SSL_SDR_BASE			            SSL_SDR_BASE_1    
#define SSL_SDR_SIZE			            SSL_SDR_SIZE_1

/* Prashanth - encoder integration - change this address for duplex use case
 * it would have been better if work, code and para buffers were put at the beginning
 * of sdram than in between the memory as these three will be same when duplex is used */
#define SSL_ENC_SDR_BASE					(SSL_SDR_BASE_1)
//==============================================================================
//Maximus peripheral offset
//==============================================================================
#define GPT2    (0x6000)
#define VPPHD   (0x7000)
#define DVHD    (0x8000)
#define VPU     (0x9000)

//==============================================================================
//Sizes
//- purpose of buffer
//- size of buffer
//==============================================================================
#define SSL_ALIGN64KB(x)                    (((x) + 0xFFFF) & 0xFFFF0000)

//VPU's internal bitstream buffer (maximum capped by VPU_DecOpen() is 16383*1024)
#define	SSL_LARGE_STREAM_BUF_SIZE           (2*1024*1024)
#define	SSL_SMALL_STREAM_BUF_SIZE           (SSL_LARGE_STREAM_BUF_SIZE)

//H264 decoder slice info buffer
#define	SSL_ADDR_SLICE_SAVE_BUFFER_SIZE     (0x080000)

//H264 decoder PS info buffer (single instance)
#define SSL_PS_SAVE_SIZE                    (0x20000)

//H264 decoder PS info buffer (multiple instance)
#define	SSL_ADDR_PS_SAVE_BUFFER_1_SIZE      (0)
#define	SSL_ADDR_PS_SAVE_BUFFER_2_SIZE      (0)
#define	SSL_ADDR_PS_SAVE_BUFFER_3_SIZE      (0)
#define	SSL_ADDR_PS_SAVE_BUFFER_4_SIZE      (0)
#define SSL_PS_SAVE_SIZE_1                  (0)
#define SSL_PS_SAVE_SIZE_2                  (0)
#define SSL_PS_SAVE_SIZE_3                  (0)
#define SSL_PS_SAVE_SIZE_4                  (0)

//VPU's code+work+para buffers
//= CODE_BUF_SIZE + WORK_BUF_SIZE + PARA_BUF_SIZE*2
//#define CODE_BUF_SIZE			    ( 160 * 1024 )
//#define FMO_SLICE_SAVE_BUF_SIZE	( 64 )
//#define WORK_BUF_SIZE			    ( 160 * 1024 ) + (MAX_NUM_INSTANCE*41*1024) // 160K(tempbuff) + index*41K(comom+static)
//#define PARA_BUF2_SIZE			( 2 * 1024 )
//#define PARA_BUF_SIZE			    ( 10 * 1024 )
//#define SSL_ADDR_BIT_WORK_SIZE              (0x100000)
#define SSL_ADDR_BIT_WORK_SIZE              (160*1024 + 160*1024 + 41*1024 + 20*1024)
//prashanth - increased the size from 381kb to 1mb
#define SSL_ENC_ADDR_BIT_WORK_SIZE              (0x100000)

//Reference frame buffer
//- worst case H264 requiring 7 frames at 1080p
//#define SSL_ADDR_FRAME_BASE_SIZE            (1920*1088*3/2*7)
//kahho:: 08-Feb-11, CnM bug temp workaround need 1 more buffer
//#define SSL_ADDR_FRAME_BASE_SIZE            (1920*1088*3/2*8)
//kahho:: 11-Feb-11, FrameBufferInit(), this size include region for MvColBuf which is (FRAME_MAX_X * FRAME_MAX_Y / 4) worst case
#define SSL_MVCOLBUF_SIZE                   (1920*1088/4)
#define SSL_MAX_FRAME_SIZE                  (1920*1088*3/2)
#define SSL_MAX_FRAME_IDX                   (9)
#define SSL_ADDR_FRAME_BASE_SIZE            ((SSL_MAX_FRAME_SIZE + SSL_MVCOLBUF_SIZE)*SSL_MAX_FRAME_IDX)

//Picture parameter buffer
//= SIZE_PARA_BUF + SIZE_USER_BUF + SIZE_MV_DATA + SIZE_MB_DATA + SIZE_FRAME_BUF_STAT + USER_DATA_INFO_OFFSET
#define	SSL_ADDR_PIC_PARA_BASE_ADDR_SIZE    (0x34300)

//kahho:: TODO, make dynamic allocation based on codec requirement
//        - see Coda7542 BW saving xls for sizes
//Sec AXI capable work buffers
#define	SSL_DBKY_INTERNAL_USE_BUF_SIZE      (0x8000)
#define	SSL_DBKC_INTERNAL_USE_BUF_SIZE      (0x8000)
#define	SSL_BIT_INTERNAL_USE_BUF_SIZE       (0x6000)
#define SSL_IPACDC_INTERNAL_USE_BUF_SIZE    (0x4000)
#define	SSL_OVL_INTERNAL_USE_BUF_SIZE       (0x3000)

//Encoder ME buffer
//- searchPa[i].SearchRamSize = ( ( encOP[i].picWidth + 15 ) & ~15 ) * 36 + 2048;
#define SSL_DEFAULT_SEARCHRAM_ADDR_SIZE     (0)

//kahho:: 10-Jan-11, update - not needed for qMatTable and huffTable anymore
//SSL app - scratch memory used for replacing malloc() of qMatTable and huffTable during MJPEG
#define SSL_QMAT_TABLE_SIZE                 (192)
#define SSL_HUFF_TABLE_SIZE                 (432)
//#define SSL_SCRATCH_MEM_BUFF_SIZE           (SSL_QMAT_TABLE_SIZE + SSL_HUFF_TABLE_SIZE)
#define SSL_SCRATCH_MEM_BUFF_SIZE           (0)

//kahho:: TODO, improve
//SSL app - whole file buffer
#define SSL_INFILE_BUFFER_SIZE	            (122*1024*1024)
#define SSL_ENC_INFILE_BUFFER_SIZE	        (30*1024*1024)

//prashanth - used another macro for encoder
//kahho:: TODO, improve
//SSL app - encoder input YUV buffer
#define SSL_YUV_IN_BUFFER_SIZE	            (0)

//kahho:: TODO, improve to support large JPEG images
//SSL app - decoder output YUV buffer
#define SSL_YUV_OUT_BUFFER_SIZE	            (1920*1088*3/2)

//kahho:: TODO, improve
//SSL app - display buffer
#define SSL_DISPLAY_BUFF_SIZE		        (800*480*4)
#define SSL_ENC_DISPLAY_BUFF_SIZE		    (1366*768*4)

//kahho:: TODO, improve
//SSL app - VPP job pool (must be 64KB aligned)
#if (defined(SSL_VPP_PRE_EN) || defined(SSL_VPP_POST_EN))
#define SSL_VPP_JOB_POOL_SIZE               (1*1024)
#else
#define SSL_VPP_JOB_POOL_SIZE               (0)
#endif


//==============================================================================
//Physical addresses
//- direct replacement of original MACRO in VpuApi.h
//- look for tag SSL_BUFFER_REPLACEMENT
//==============================================================================
#define	SSL_ADDR_BIT_STREAM                 SSL_SDR_BASE_1      //bitstream buffer

//kahho:: TODO, define for ADDR_BIT_STREAM_* and STREAM_BUF_SIZE_* required in multi-instance

#define	SSL_ADDR_SLICE_SAVE_BUFFER          (SSL_ADDR_BIT_STREAM + SSL_LARGE_STREAM_BUF_SIZE)
#define	SSL_ADDR_PS_SAVE_BUFFER             (SSL_ADDR_SLICE_SAVE_BUFFER + SSL_ADDR_SLICE_SAVE_BUFFER_SIZE)
#define	SSL_ADDR_PS_SAVE_BUFFER_1           (SSL_ADDR_PS_SAVE_BUFFER)
#define	SSL_ADDR_PS_SAVE_BUFFER_2           (SSL_ADDR_PS_SAVE_BUFFER_1 + SSL_ADDR_PS_SAVE_BUFFER_1_SIZE)
#define	SSL_ADDR_PS_SAVE_BUFFER_3           (SSL_ADDR_PS_SAVE_BUFFER_2 + SSL_ADDR_PS_SAVE_BUFFER_2_SIZE)
#define	SSL_ADDR_PS_SAVE_BUFFER_4           (SSL_ADDR_PS_SAVE_BUFFER_3 + SSL_ADDR_PS_SAVE_BUFFER_3_SIZE)
#define SSL_ADDR_BIT_WORK                   (SSL_ADDR_PS_SAVE_BUFFER_4 + SSL_ADDR_PS_SAVE_BUFFER_4_SIZE)
#define SSL_ADDR_FRAME_BASE                 (SSL_ADDR_BIT_WORK + SSL_ADDR_BIT_WORK_SIZE)
#define SSL_MAX_FRAME_BASE                  (SSL_ADDR_FRAME_BASE + SSL_ADDR_FRAME_BASE_SIZE)
#define	SSL_ADDR_PIC_PARA_BASE_ADDR         (SSL_MAX_FRAME_BASE)
#define	SSL_DBKY_INTERNAL_USE_BUF           (SSL_ADDR_PIC_PARA_BASE_ADDR + SSL_ADDR_PIC_PARA_BASE_ADDR_SIZE)
#define	SSL_DBKC_INTERNAL_USE_BUF           (SSL_DBKY_INTERNAL_USE_BUF + SSL_DBKY_INTERNAL_USE_BUF_SIZE)
#define	SSL_BIT_INTERNAL_USE_BUF            (SSL_DBKC_INTERNAL_USE_BUF + SSL_DBKC_INTERNAL_USE_BUF_SIZE)
#define SSL_IPACDC_INTERNAL_USE_BUF         (SSL_BIT_INTERNAL_USE_BUF + SSL_BIT_INTERNAL_USE_BUF_SIZE)
#define	SSL_OVL_INTERNAL_USE_BUF            (SSL_IPACDC_INTERNAL_USE_BUF + SSL_IPACDC_INTERNAL_USE_BUF_SIZE)
#define SSL_DEFAULT_SEARCHRAM_ADDR          (SSL_OVL_INTERNAL_USE_BUF + SSL_OVL_INTERNAL_USE_BUF_SIZE)
#define SSL_SCRATCH_MEM_BUFF		        (SSL_DEFAULT_SEARCHRAM_ADDR + SSL_DEFAULT_SEARCHRAM_ADDR_SIZE)
#define SSL_SDR_END_1                       (SSL_SCRATCH_MEM_BUFF + SSL_SCRATCH_MEM_BUFF_SIZE)

#define SSL_INFILE_BUFFER		            SSL_SDR_BASE_2
#define SSL_YUV_IN_BUFFER		            (SSL_INFILE_BUFFER + SSL_INFILE_BUFFER_SIZE)
#define SSL_YUV_OUT_BUFFER		            (SSL_YUV_IN_BUFFER + SSL_YUV_IN_BUFFER_SIZE)
#define SSL_DISPLAY_BUFF			        (SSL_YUV_OUT_BUFFER + SSL_YUV_OUT_BUFFER_SIZE)
#define SSL_VPP_JOB_POOL                    SSL_ALIGN64KB(SSL_DISPLAY_BUFF + SSL_DISPLAY_BUFF_SIZE)
#define SSL_SDR_END_2                       (SSL_VPP_JOB_POOL + SSL_VPP_JOB_POOL_SIZE)

/*****************************************************************************/
/*				Encoder related memory allocation							 */
/* prashanth - encoder integration - max input frame size will be 720p       */
/* please note that changes are required to this layer when duplex support is*/
/* added later                                                               */
/*****************************************************************************/
//SSL app - encoder input YUV buffer
#define SSL_ENC_YUV_IN_BUFFER_SIZE	        (1280*720*3/2)
#define SSL_ENC_MAX_FRAME_WIDTH				(1280)
#define SSL_ENC_MAX_FRAME_HEIGHT			(720)
#define SSL_ENC_MAX_FRAME_SIZE				(SSL_ENC_YUV_IN_BUFFER_SIZE)//always considering yuv420 as input
#define SSL_ENC_MVCOLBUF_SIZE				(1280*720/4)
#define SSL_ENC_MAX_RECON_FRAMES			(8)
#define SSL_ENC_ADDR_FRAME_BASE_SIZE        ((SSL_ENC_MAX_FRAME_SIZE + SSL_ENC_MVCOLBUF_SIZE)*SSL_ENC_MAX_RECON_FRAMES)

//definitions with ENC string
#define SSL_ENC_BIT_STREAM_SIZE				(3*1024*1024)//using 3 mb of memory as output bit stream buffer
//- searchPa[i].SearchRamSize = ( ( encOP[i].picWidth + 15 ) & ~15 ) * 36 + 2048;
#define SSL_ENC_DEFAULT_SEARCHRAM_ADDR_SIZE     (1280*36+2048)


//various memory locations used for encoder - change the enc base address when duplex use case is required
#define	SSL_ENC_ADDR_BIT_STREAM             	(SSL_ENC_SDR_BASE)      //bitstream buffer - change the starting address for duplex use case

#define	SSL_ENC_ADDR_SLICE_SAVE_BUFFER          (SSL_ENC_ADDR_BIT_STREAM + SSL_ENC_BIT_STREAM_SIZE)
#define	SSL_ENC_ADDR_PS_SAVE_BUFFER             (SSL_ENC_ADDR_SLICE_SAVE_BUFFER + SSL_ADDR_SLICE_SAVE_BUFFER_SIZE)
#define	SSL_ENC_ADDR_PS_SAVE_BUFFER_1           (SSL_ENC_ADDR_PS_SAVE_BUFFER)
#define	SSL_ENC_ADDR_PS_SAVE_BUFFER_2           (SSL_ENC_ADDR_PS_SAVE_BUFFER_1 + SSL_ADDR_PS_SAVE_BUFFER_1_SIZE)
#define	SSL_ENC_ADDR_PS_SAVE_BUFFER_3           (SSL_ENC_ADDR_PS_SAVE_BUFFER_2 + SSL_ADDR_PS_SAVE_BUFFER_2_SIZE)
#define	SSL_ENC_ADDR_PS_SAVE_BUFFER_4           (SSL_ENC_ADDR_PS_SAVE_BUFFER_3 + SSL_ADDR_PS_SAVE_BUFFER_3_SIZE)
#define SSL_ENC_ADDR_BIT_WORK                   (SSL_ENC_ADDR_PS_SAVE_BUFFER_4 + SSL_ADDR_PS_SAVE_BUFFER_4_SIZE)
#define SSL_ENC_ADDR_FRAME_BASE                 (SSL_ENC_ADDR_BIT_WORK + SSL_ENC_ADDR_BIT_WORK_SIZE)
#define SSL_ENC_MAX_FRAME_BASE                  (SSL_ENC_ADDR_FRAME_BASE + SSL_ENC_ADDR_FRAME_BASE_SIZE)

#define	SSL_ENC_ADDR_PIC_PARA_BASE_ADDR         (SSL_ENC_MAX_FRAME_BASE)
#define	SSL_ENC_DBKY_INTERNAL_USE_BUF           (SSL_ENC_ADDR_PIC_PARA_BASE_ADDR + SSL_ADDR_PIC_PARA_BASE_ADDR_SIZE)
#define	SSL_ENC_DBKC_INTERNAL_USE_BUF           (SSL_ENC_DBKY_INTERNAL_USE_BUF + SSL_DBKY_INTERNAL_USE_BUF_SIZE)
#define	SSL_ENC_BIT_INTERNAL_USE_BUF            (SSL_ENC_DBKC_INTERNAL_USE_BUF + SSL_DBKC_INTERNAL_USE_BUF_SIZE)
#define SSL_ENC_IPACDC_INTERNAL_USE_BUF         (SSL_ENC_BIT_INTERNAL_USE_BUF + SSL_BIT_INTERNAL_USE_BUF_SIZE)
#define	SSL_ENC_OVL_INTERNAL_USE_BUF            (SSL_ENC_IPACDC_INTERNAL_USE_BUF + SSL_IPACDC_INTERNAL_USE_BUF_SIZE)
#define SSL_ENC_DEFAULT_SEARCHRAM_ADDR          (SSL_ENC_OVL_INTERNAL_USE_BUF + SSL_OVL_INTERNAL_USE_BUF_SIZE)
#define SSL_ENC_SCRATCH_MEM_BUFF		        (SSL_ENC_DEFAULT_SEARCHRAM_ADDR + SSL_ENC_DEFAULT_SEARCHRAM_ADDR_SIZE)

#define SSL_ENC_INFILE_BUFFER		            SSL_SDR_BASE_2
#define SSL_ENC_YUV_IN_BUFFER		            (SSL_ENC_INFILE_BUFFER + SSL_ENC_INFILE_BUFFER_SIZE)
#define SSL_ENC_YUV_OUT_BUFFER		            (SSL_ENC_YUV_IN_BUFFER + SSL_YUV_IN_BUFFER_SIZE)
#define SSL_ENC_DISPLAY_BUFF			        (SSL_ENC_YUV_OUT_BUFFER + SSL_YUV_OUT_BUFFER_SIZE)
#define SSL_ENC_VPP_JOB_POOL                    SSL_ALIGN64KB(SSL_ENC_DISPLAY_BUFF + SSL_ENC_DISPLAY_BUFF_SIZE)
#define SSL_ENC_SDR_END_2                       (SSL_ENC_VPP_JOB_POOL + SSL_VPP_JOB_POOL_SIZE)


#if ((SSL_SDR_END_2 - SSL_SDR_BASE_2) > SSL_SDR_SIZE_2)
#error SSL_SDR_SIZE_2 insufficient
#else
#endif
//encoder sanity check
#if ((SSL_ENC_SDR_END_2 - SSL_SDR_BASE_2) > SSL_SDR_SIZE_2)
#error encoder SSL_SDR_SIZE_2 insufficient
#else
#endif

#define SSL_SDR_END                         SSL_SDR_END_1

#define SSL_SRAM_ADDR   (0xFFFF0000)
#define SSL_SRAM_SIZE   (64*1024)

#define SSL_PRIVATE_DATA_SIZE               (sizeof(int)*8)
#define SSL_SEC_AXI_SIZE                    (SSL_SRAM_SIZE - SSL_PRIVATE_DATA_SIZE)
#define SSL_PRIVATE_DATA                    (-SSL_PRIVATE_DATA_SIZE)

#endif  //__SSLMAP_H__
