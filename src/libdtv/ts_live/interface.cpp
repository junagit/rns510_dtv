/*
*       File name:      interface.c
*       Source file for different module interface
*       Author: Jimye (jimye@solomon-systech.com)
*       Copyright 2007(c)Solomon Systech Limited
*/


#include <sys/poll.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#ifdef TSI_LIVE
#include "tsi.h"
#include "interface.h"

#define TSI_LIVE                        1
#define TSI_NB_TIMEOUT                  150
#define PID_NUM                         MAX_NUM_PIDS_SUPPORTED
//#define DEMUXER_INTERFACE_DEBUG         1
#define MODE                            0 //DMA = 0; PIO = 8
#define	FT_MODE                         4 //ENALBE  = 4; DISBLE = 0;

volatile uint32_t pid_number = 0;
volatile int32_t pid_val[PID_NUM];
volatile uint32_t fid_bitmap = 0;

//#define DEMUXER_INTERFACE_DEBUG 1

#if DEMUXER_INTERFACE_DEBUG
        #define INTERFACE_DEBUG(fmt, args...)   printf("INTERFACE DEBUG: "fmt, ##args)                /*info function */
#else
        #define INTERFACE_DEBUG(fmt, args...)
#endif


volatile int inited = 0;

static tsi_t init;
static tsi_p test = &init;

static fcfg_t cfg;
static fcfg_p test_cfg = &cfg;

static fen_t en;
static fen_p test_en =&en;     

int tsi_status_t_len = sizeof(tsi_info_t);
//---------------end of tsi global resource ---------------
void enable_filter(uint32_t enable_fid)
{
        if(inited==0) return;
        
	en.fid.fid_0 = ((enable_fid >> 0) & 0x1);       // 1 enable , 0 disable
        en.fid.fid_1 = ((enable_fid >> 1) & 0x1);
        en.fid.fid_2 = ((enable_fid >> 2) & 0x1);
        en.fid.fid_3 = ((enable_fid >> 3) & 0x1);
        en.fid.fid_4 = ((enable_fid >> 4) & 0x1);
        en.fid.fid_5 = ((enable_fid >> 5) & 0x1);
        en.fid.fid_6 = ((enable_fid >> 6) & 0x1);
        en.fid.fid_7 = ((enable_fid >> 7) & 0x1);
        en.fid.fid_8 = ((enable_fid >> 8) & 0x1);
        en.fid.fid_9 = ((enable_fid >> 9) & 0x1);
        en.fid.fid_10 = ((enable_fid >> 10) & 0x1);
        en.fid.fid_11 = ((enable_fid >> 11) & 0x1);
}



int tsi_fill_buffer(stream_t * st)
{
#ifdef TSI_LIVE	
	//printf("st->read_len:%i, st->ops: %i, st->buffer: %p\n", st->read_len, st->ops,  st->buffer);

	tsi_info_t tsi_status_info_for_print;
	
        if(inited==0) return -1;
        
	void *temp_ptr;

	temp_ptr = tsi_retrieve_data (st->read_len, st->time_out, st->buff_idx);
	if (temp_ptr == NULL)
	{
		printf("fetch data err\n");
		return 0;
	}

	memcpy ((void *)&tsi_status_info_for_print,temp_ptr,tsi_status_t_len);
	if((tsi_status_info_for_print.hw_overflow) || (tsi_status_info_for_print.sw_overflow))
	{
		 printf("memcpy retrieve overflow %d %d\n",tsi_status_info_for_print.hw_overflow,tsi_status_info_for_print.sw_overflow);
	}
	st->ops = 0;
	st->out_remaining_len = tsi_status_info_for_print.more_data;
	if((tsi_status_info_for_print.length == 0))
		{

                	//printf("tsi_fill_buffer len = 0;\n");
			return 0;
		}
	//mem_mem_p = (uint32_t *)mapm(tsi_status_info_for_print.start_addr, tsi_status_info_for_print.length);
	memcpy((void *)st->buffer, (void *)tsi_status_info_for_print.start_addr_virt, tsi_status_info_for_print.length);
	//unmapm(mem_mem_p,tsi_status_info_for_print.length);
	return tsi_status_info_for_print.length;
        printf("tsi_status_info_for_print.length:%d\n",tsi_status_info_for_print.length);
#endif
	return 0;
}

int tsi_fill_buffer_nb(stream_t * st)
{
#ifdef TSI_LIVE	

	tsi_info_t tsi_status_info_for_print;
	
        if(inited==0) return -1;
        
	void *temp_ptr;

	temp_ptr = tsi_retrieve_data_nb(st->read_len, st->buff_idx,TSI_NB_TIMEOUT);
	if (temp_ptr == NULL)
	{
		printf("fetch data err\n");
		return 0;
	}

	memcpy ((void *)&tsi_status_info_for_print,temp_ptr,tsi_status_t_len);
	if((tsi_status_info_for_print.hw_overflow) || (tsi_status_info_for_print.sw_overflow))
	{
		 printf("[MW][interface][tsi_fill_buffer]TSI DRIVER ::: OVERFLOW %d %d\n",tsi_status_info_for_print.hw_overflow,tsi_status_info_for_print.sw_overflow);
	}
	st->ops = 0;
	st->out_remaining_len = tsi_status_info_for_print.more_data;
	if((tsi_status_info_for_print.length == 0))
        {
        //    printf("get tsi status err\n");
            return 0;
        }
	//mem_mem_p = (uint32_t *)mapm(tsi_status_info_for_print.start_addr, tsi_status_info_for_print.length);
	memcpy((void *)st->buffer, (void *)tsi_status_info_for_print.start_addr_virt, tsi_status_info_for_print.length);
	//unmapm(mem_mem_p,tsi_status_info_for_print.length);
	return tsi_status_info_for_print.length;
#endif
	return 0;
}


int tsi_fill_buffer_dma(stream_t * st)
{
#ifdef TSI_LIVE	
    
	tsi_info_t tsi_status_info_for_print;
	
        if(inited==0) return -1;
        
	tsi_info_p temp_ptr;

        
      //  printf("@@@@@ before tsi_retrieve_data_dma!!!!1\n");
	temp_ptr = tsi_retrieve_data_dma((uint32_t)st->buffer, st->read_len,st->time_out,st->buff_idx);
        
	if (temp_ptr == NULL)
	{
		printf("fetch data err\n");
		return 0;
	}
        
	memcpy ((void *)&tsi_status_info_for_print,temp_ptr,tsi_status_t_len);
        
	if((tsi_status_info_for_print.hw_overflow) || (tsi_status_info_for_print.sw_overflow))
	{
		 printf("dma retrieve overflow %d %d\n",tsi_status_info_for_print.hw_overflow,tsi_status_info_for_print.sw_overflow);
                 return 0;
	}
        
  //      printf("temp_ptr->start_addr:0x%x  start_addr_virt:0x%x\n",tsi_status_info_for_print.start_addr,tsi_status_info_for_print.start_addr_virt);
	st->ops = 0;
	st->out_remaining_len = tsi_status_info_for_print.more_data;

	if((tsi_status_info_for_print.length == 0) || (tsi_status_info_for_print.length == -1))
        {
             //   printf("get tsi status err %d\n",tsi_status_info_for_print.length);
                return 0;
        }

        

	return tsi_status_info_for_print.length;
#endif
	return 0;
}

/*tbd*/
int tsi_fill_buffer_nb_rec(stream_t * st)
{
#ifdef TSI_LIVE	
	tsi_info_rec_t tsi_record_info;	
	tsi_info_rec_p temp_ptr;
	
	memset(&tsi_record_info,0x00,sizeof(tsi_info_rec_t));
	
	temp_ptr = tsi_retrieve_data_nb_rec(st->read_len,st->buff_idx,st->rec_margin_len);
	if (temp_ptr == NULL)
	{
		printf("fetch data err\n");
		return 0;
	}
	memcpy (&tsi_record_info,temp_ptr,tsi_status_t_len);
	st->ops = 0;
	st->out_remaining_len = tsi_record_info.more_data;
	st->rec_discard_len = tsi_record_info.discard_length;
	st->virt_address = tsi_record_info.start_addr_virt;		
       // printf("Recording : get tsi status err:%d %d\n",tsi_record_info.length,temp_ptr->length);
//	if((tsi_record_info.length == 0) || (tsi_record_info.length == -1))
//	{
//            printf("Recording : get tsi status err:%d %d\n",tsi_record_info.length,temp_ptr->length);
//            return 0;
//	}

	return tsi_record_info.length;
#endif
	return 0;
}



int8_t tsi_initial_interface()
{
    if(inited==1) return 0;
#ifdef TSI_LIVE	
	tsi_err_e err;
	memset(&init, 0, sizeof(tsi_t));
	memset(&en, 0, sizeof(fen_t));
	memset(&cfg, 0, sizeof(fcfg_t));
	memset((void*)pid_val,-1,sizeof(uint32_t)*PID_NUM);
	//memset(packet_number_pre_interrupt,0,sizeof(uint32_t)*PID_NUM);
	fid_bitmap = 0;
	//memset(node_len,0,sizeof(uint8_t)*12);
	init.reg = (void *)0x04200000;//register address
        init.pkt_sz = TS_188;

        init.mode = MODE;
        init.filter = FT_MODE;

	init.circular_buffer_length[TSI_CTRL_BUFFINDEX] = TSI_CTRL_BUFFLEN;
	init.circular_buffer_length[TSI_MEDIA_DATA_BUFFINDEX] = TSI_MEDIADATA_BUFFLEN;
	init.circular_buffer_length[TSI_SUBTITLE_BUFFINDEX] = TSI_SUBTITLE_BUFFLEN;
        init.circular_buffer_length[TSI_EPG_BUFFINDEX] = TSI_EPG_BUFFLEN;
        init.circular_buffer_length[TSI_TIME_BUFFINDEX] = TSI_TIME_BUFFLEN;
        
       // init.pio_buffer_length = (188*16); //only in pio mode : PACKAGE SIZE * NUMBER OF PACKAGE....for number of package, please assign a multiple of 16 for now..
	init.pio_buffer_length = 0;

        
        err = tsi_init(test);
	if(err <0)
	{	
		printf("init: tsi_init(test) err: %i\n", err);
		return err;
	}

        enable_filter(fid_bitmap);
        inited = 1;
        
        tsi_remove_pid_from_filter(-1);
#endif
	return 0;
}
int8_t tsi_remove_pid_from_filter(int pid)
{
	int index;
	tsi_err_e err;

        if(inited==0) return -1;
        
	if(pid>=0)
	{
		if(pid_number<=0)
		{
			return -1;
		}

		for(index=0;index<MAX_NUM_PIDS_SUPPORTED;index++)
		{
			/* check for the index to be free */
			if(pid == pid_val[index])
			{
				INTERFACE_DEBUG("remove pid_val[%d]:%d\n",index,pid_val[index]);
				pid_val[index] = -1;
				break;
			}
		}
		
		if(MAX_NUM_PIDS_SUPPORTED == index)
		{
			INTERFACE_DEBUG("remove pid - MAX pid's reached :%d\n",pid);
			for(index=0;index<MAX_NUM_PIDS_SUPPORTED;index++)
				INTERFACE_DEBUG("%d ",pid_val[index]);

			return -1;
		}
		
		/* update the fid number */
		fid_bitmap &= (~(1<<index));

		pid_number--;
		INTERFACE_DEBUG("remove pid:remove tsi_exit %d \n",fid_bitmap);

		enable_filter(fid_bitmap);
		err=tsi_filter_en(test_en);
	}
	else
	{
		INTERFACE_DEBUG("remove pid:remove all pid\n");
		pid_number = 0;
		memset((void*)pid_val,-1,sizeof(uint32_t)*PID_NUM);
		//memset(packet_number_pre_interrupt,0,sizeof(int)*PID_NUM);
		fid_bitmap = 0;
		INTERFACE_DEBUG("remove pid:remove tsi_exit %d \n",fid_bitmap);

		enable_filter(fid_bitmap);

		err=tsi_filter_en(test_en);

		INTERFACE_DEBUG("remove pid:tsi_clear %d \n",fid_bitmap);
		tsi_clear_buffer(TSI_CTRL_BUFFINDEX);
		tsi_clear_buffer(TSI_MEDIA_DATA_BUFFINDEX);
                tsi_clear_buffer(TSI_EPG_BUFFINDEX);
                tsi_clear_buffer(TSI_TIME_BUFFINDEX);
	}

	return err;

}


int8_t tsi_add_pid_to_filter(int pid,int8_t type,int8_t buff_idx)
{
    int32_t index;
    tsi_err_e err;

    if(inited==0) return -1;

    for(index=0;index<MAX_NUM_PIDS_SUPPORTED;index++)
    {
            /* check for the index to be free */
            if(-1 == pid_val[index])
            {
                    INTERFACE_DEBUG("pid_val[%d]:%d\n",index,pid_val[index]);
                    pid_val[index] = pid;
                    break;
            }
    }

    if(MAX_NUM_PIDS_SUPPORTED == index)
    {
            INTERFACE_DEBUG("add pid - MAX pid's reached :%d\n",pid);
            for(index=0;index<MAX_NUM_PIDS_SUPPORTED;index++)
                    INTERFACE_DEBUG("%d ",pid_val[index]);

            return -1;
    }

//    printf("tsi_add_pid_to_filter:%d\n",pid);
    /* add bit into fid bitmap */
    fid_bitmap |= (1<<index);
    pid_val[index] = pid;
    pid_number++;

    cfg.buffer_index  = buff_idx;

    if(PSI == type)
    {
            cfg.pkts_per_intr = TSI_CTRL_PKTSPERINTR;

    }
    else if(AUDIO == type)
    {
            cfg.pkts_per_intr = TSI_MEDIADATA_AUD_PKTSPERINTR;	

    }
    else if(VIDEO == type)
    {
            cfg.pkts_per_intr = TSI_MEDIADATA_VID_PKTSPERINTR;	

    }
    else if(SUB_TITLE == type)
    {
            cfg.pkts_per_intr = TSI_SUBTITLE_PKTSPERINTR;	

    }
    else
    {
            /* should not enter here */
            INTERFACE_DEBUG("invalid type inp params %d %d\n",buff_idx,pid);
            return -1;
    }

    INTERFACE_DEBUG("add pid to filter config %d %d %d %d %d %d %d\n",cfg.fid,cfg.pid,cfg.pkts_per_intr,cfg.buffer_index,fid_bitmap,pid_val[index],index);
    cfg.fid = index;
    cfg.pid = pid_val[index];
    err = tsi_filter_cfg(test_cfg);

    if(err)
    {
            INTERFACE_DEBUG("add pid into filter config error %d\n",err);
            return err;
    }

    tsi_clear_buffer(buff_idx);
    enable_filter(fid_bitmap);	

    //INTERFACE_DEBUG("cfg.fid:%d cfg.pid:%d interrupt:%d\n",cfg.fid,cfg.pid, packet_number_pre_interrupt[index]);
    //tsi_clear_buffer(buff_idx);

    err=tsi_filter_en(test_en);


    return err;
}


/**
 * @brief Funkcja sprawdzająca czy dany PID jest załadowany do filtru w module TSI
 * @param pid
 * @return 0 - pid nie zaladowany,1 - zaladowany
 */
int tsi_check_pid_status(int pid)
{
    for(int index=0;index<MAX_NUM_PIDS_SUPPORTED;index++)
    {
        if(pid_val[index]==pid) return 0;
    }
    
    return 1;
}
/* free all the pid's and memory */

int8_t tsi_release_all_resource()
{
	tsi_err_e	tsi_status_enum = TSI_ERR_NONE;
        
        if(inited==0) return -1;
	/* call tsi exit of tsi lib */
        tsi_status_enum = tsi_exit();//make sure exit
	fid_bitmap = 0;
        inited = 0;
	return tsi_status_enum;
}

/* API to clear circular buffer. Returns error in case of incorrect buffer index */
int8_t tsi_if_clear_buffer(int8_t buff_idx)
{
	tsi_err_e tsi_status;
        
        if(inited==0) return -1;
        
	tsi_status = tsi_clear_buffer(buff_idx);
	
	return(tsi_status);
}


#endif