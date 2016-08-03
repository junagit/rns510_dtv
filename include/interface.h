/*
*       File name:      interface.h
*       Hane file for interface
*       Author: Jimye (jimye@solomon-systech.com)
*       Copyright 2007(c)Solomon Systech Limited
*/

#ifndef  _INTERFACE_H
#define  _INTERFACE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
void burst8(char *dst, char *src, int len);
void ssl_memcpy(void *dst, void *src, uint32_t size);
#define PID_PAT                         0x00
#define PID_NIT                         0x10
#define PID_SDT                         0x11
#define PID_EIT                         0x12    
#define PID_TOT                         0x14
#define PID_TDT                         0x14    
    
    
#define READ_LEN                        188
#define TS_PACKET_SIZE                  188

enum PSI_TYPE{AUDIO,VIDEO,SUB_TITLE,PSI};

/* this number is restriction from hardware point of view */
#define MAX_NUM_PIDS_SUPPORTED          (12)

/* #defines for buffer lengths */
#define TSI_CTRL_BUFFLEN		(50)
#define TSI_MEDIADATA_BUFFLEN		(8000)
#define TSI_SUBTITLE_BUFFLEN		(50)
#define TSI_EPG_BUFFLEN                 (50)
#define TSI_TIME_BUFFLEN                (50)



/* #defines for packets per interrupt */
#define TSI_CTRL_PKTSPERINTR		(1)
#define TSI_MEDIADATA_VID_PKTSPERINTR   (5) //(20)
#define TSI_MEDIADATA_AUD_PKTSPERINTR   (5) //(10)
#define TSI_SUBTITLE_PKTSPERINTR	(1)

/* #defines for TSI buffer indexes */
#define TSI_CTRL_BUFFINDEX		(0)//uzywany podczas skanowania i pobierania epg schedule 
#define TSI_MEDIA_DATA_BUFFINDEX	(1)//uzywany dla pakietow audio-video
#define TSI_SUBTITLE_BUFFINDEX          (2)
#define TSI_EPG_BUFFINDEX		(3)//używany do pobierania epg present following 
#define TSI_TIME_BUFFINDEX		(4)//uzywany do pobierania czasu



typedef struct stream_st
{
	int(*fill_buffer)(struct stream_st *s);
	int				dev;
	long int		ops;
	int8_t			type;
	int8_t			PID_type;
	uint32_t		current_ops;
	uint32_t		read_len;
	unsigned char           *buffer;
	//added by santiego
	uint32_t 		phy_address;
	uint32_t 		mmap_len;
	uint32_t 		virt_address;
	/*<prashanth> latest tsi integration */
	uint32_t 		out_read_len; /* useful in case of non blocking */
	uint32_t 		out_remaining_len; /* remaining length in the buffer */
	uint32_t 		buff_overflow; /* buffer overflow, */
	uint32_t 		time_out; /* timeout in msec */
	uint32_t 		buff_idx; /* buffer index */
	uint32_t 		rec_margin_len; /* margin between write and read for recorder */
	uint32_t 		rec_discard_len; /* discarded length due to unerrun */
}stream_t;

/* added blocking, non blocking and dma in fill buffer. changed interface of init and add pid apis */



int tsi_fill_buffer(stream_t *st);
int8_t tsi_initial_interface();
int8_t tsi_add_pid_to_filter(int pid,int8_t type,int8_t buff_idx);
int8_t tsi_remove_pid_from_filter(int pid);
int8_t tsi_release_all_resource();
int tsi_fill_buffer_nb(stream_t *st);
int tsi_fill_buffer_dma(stream_t *st);
int tsi_fill_buffer_nb_rec(stream_t *st);
int tsi_check_pid_status(int pid);
/* other than 0 is error */
int8_t tsi_if_clear_buffer(int8_t buff_idx);

#ifdef __cplusplus
}
#endif

#endif
