/** 
 * @file   dtv_record.cpp
 * @author Michał Motyl
 *
 * @date   3 kwiecień 2012, 11:52
 */

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <pthread.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <stdint.h>


#include "multiplex.h"
#include "service_management.h"
#include "eit.h"
#include "dtv_core.h"
#include "crc32.h"
#include "misc_func.h"

#ifdef TSI_LIVE
#include "dvbtcontrol.h"
#include "gpio_ctrl.h"
#include "tsi.h"
#endif

static char                record_file[256];

bool dtv::make_new_pat(dvb_service *service,unsigned char *dst)
{
static uint8_t cc = 0;
uint32_t crc = 0; 
unsigned char *buf = NULL;

   	if(service==NULL) return false;
    
        if(dst==NULL) return false;


        buf = dst;

        buf[0] = 0x47;
        buf[1] = 0x40;
        buf[2] = 0x00;
        buf[3] = 0x10 | cc;
        cc ++;
        cc = cc%16;
        buf[4] = 0x00;
        buf[5] = 0x00;//table id
        buf[6] = 0xb0;
        buf[8] = (service->transport_stream_id>>8) & 0xff;//H TS id
        buf[9] = service->transport_stream_id & 0xff;       //L TS id
        // printf("TS id H:%d TS id L:%d\n",buf[8],buf[9]);
        buf[10]= 0xc5;//version number = 2
        buf[11]= 0x00; //section number
        buf[12]= 0x00; //last section number 
        int i;

        int8_t index = -1;

        buf[13] = (service->getServiceID() & 0xff00)>>8;
        buf[14] =  service->getServiceID() & 0xff;
        buf[15] = (service->getPMTpid() & 0xff00)>>8 | 0xe0;
        buf[16] =  service->getPMTpid() & 0xff;
        index = 17;


        buf[7] = index + 4 - 5 - 3; //index - packet_head_len - 4 + crc 
        crc = _dvb_crc32(&buf[5],index-5);


        buf[index] = (crc>>24)&0xff;
        buf[index+1] = (crc>>16)&0xff;
        buf[index+2] = (crc>>8)&0xff;
        buf[index+3] = crc & 0xff;

        for(i=index+4;i<TS_PACKET_SIZE;i++)
        {
                buf[i]=0xff;
        }
   	
	return true;
}


bool dtv::make_new_pmt(dvb_service *service,unsigned char *dst)
{
    unsigned char * buf;
    dvb_service_pid *tmp;
    
    if(service==NULL) return false;
    
    if(dst==NULL) return false;
    
    if(service->service_pid_list.get_list_count()==0) return false;
    
    

	if(dst!=NULL){
            buf = &dst[TS_PACKET_SIZE];
        }
        
	buf[0] = 0x47;
	buf[1] = 0x40|((service->getPMTpid()>>8)&0x1f);//high 5 bits
	buf[2] = service->getPMTpid() & 0xff;//low 8 bit
	buf[3] = 0x10;
    	//packet data
	buf[4] = 0x00;
	buf[5] = 0x02;//table id
	
	buf[6] = 0xb0;//section length H 4 bits
	//buf[7] = 0x17;//section length L 8 bits
	
	buf[8] = (service->service_id>>8) & 0xff;//H program id
	buf[9] = service->service_id & 0xff;       //L program id
	buf[10]= 0xc5;//version number = 2
   	buf[11]= 0x00; //section number
   	buf[12]= 0x00; //last section number
   	buf[13]= 0xe0 | ((service->getPCRpid()>>8) & 0xff); //PRC H
   	buf[14]= service->getPCRpid() & 0xff;  // PRC L
   	buf[15]= 0xf0;  //program information H
   	buf[16]= 0x00;	//program information L
	int index = 17;	
	
        tmp = get_video_pid_from_list(service);
        
	if(tmp != NULL)//for none video es
	{
		buf[index++]= tmp->service_pid_type;//video type
		buf[index++]= 0xe0|((tmp->pid>>8)&0x1f);//video pid H 5 bits
		buf[index++]= tmp->pid & 0xff; //video pid L 8 bits
		buf[index++]= 0xf0; //ES infor Length H 4 bits
		buf[index++]= 0x00; //ES infor Length L 8 bits
	}
	unsigned int kk = 0,jj=index;
	//printf("a_number:%d\n",a_number);
	for(kk=0;kk<service->service_pid_list.get_list_count();kk++)
	{
            tmp = service->service_pid_list.get_from_list(kk);
            
            if(tmp==NULL) continue;
            
            if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_AUDIO_AAC || tmp->service_pid_type==STREAM_TYPE_AUDIO_AC3 || tmp->service_pid_type==STREAM_TYPE_AUDIO_EAC3)
            {
                
                buf[jj++]=tmp->service_pid_type;//audio type
                buf[jj++]=0xe0|((tmp->pid>>8)&0x1f);//audio pid H 5 bits
                buf[jj++]=tmp->pid & 0xff; //audio pid L 8 bits
                buf[jj++]=0xf0;//ES infor Length H 4 bits
                buf[jj++]=0x00;//ES infor Length L 8 bits
                
            }
	}
	buf[7] = jj + 4 - 8;
	uint32_t crc = 0;
        index = jj;	
	crc = _dvb_crc32(&buf[5],index-5);
	  
	buf[index] = (crc>>24)&0xff;
	buf[index+1] = (crc>>16)&0xff;
	buf[index+2] = (crc>>8)&0xff;
	buf[index+3] = crc & 0xff;
	int i;
	for(i=index+4;i<TS_PACKET_SIZE;i++)
	{
	    buf[i]=0xff;
	}

	return true;
}



bool dtv::ts_start_record(char * record_file)
{
static struct timeval tv;

        if(make_new_pat(current_service,pat_pmt)==false)
        {
            DTV_CORE_ERROR_DEBUG("make_new_pat error!!!!1\n");
            return false;
        }


        if(make_new_pmt(current_service,pat_pmt)==false)
        {
            DTV_CORE_ERROR_DEBUG("make_new_pmt error!!!!1\n");
            return false;
        }


	do
	{
		if(make_ts == 1)
		{
			break;
		}
		if(record_file == NULL)
		{
			break;
		}
		
		ts_f	= NULL;
		st_f 	= NULL;
		me_f    = NULL;
		ts_f = fopen(record_file,"wb+");			
		
                if(ts_f == NULL)
                {
                    if(ENOSPC == errno)
                    {
                        //no_space_cb(user_data_);
                        //DEMUXER_ERROR = DEMUXER_NOSPACE_ERR;
                        return false;
                    }
                    break;
                }

	
		printf("free space: %i\n", free_space(fileno(ts_f)));
                
		if(free_space(fileno(ts_f))  < 1500)
		{
			//DEMUXER_DEBUG("JAKAY debug 2\n");
			//no_space_cb(user_data_);
			//DEMUXER_ERROR = DEMUXER_NOSPACE_ERR;
			return false;
		}

                printf("write the first two 188 pat_pmt to the file\n");
                fwrite(get_pat_pmt_copy(),sizeof(char),(2*TS_PACKET_SIZE),ts_f);	
		fclose(ts_f);
                sync();
                
		gettimeofday(&tv, NULL);
		record_total_time = 0;
		record_start_time = tv.tv_sec*1000 + tv.tv_usec/1000;

		ts_stop_flag = 0;
                dvb_player->record_start(record_file);
//		if(pthread_create(&record_ThreadId, NULL,&dtv::record_thread,this)!= 0) //create record thread
//                {
//                    printf("Create record pthread error!\n");
//                    break;
//                }

	  	if (make_ts == 0)
		  {
			make_ts = 1;
		  }

		return true;
	}while(0);
        
        
	return false;	
}


int8_t dtv::ts_stop_record(char * record_file)
{
	static struct timeval tv;
        
        printf("enter in ts_stop_record\n");
        
	gettimeofday(&tv, NULL);
	record_total_time += (tv.tv_sec*1000 + tv.tv_usec/1000 - record_start_time);
	uint32_t rec_ttl_time = (uint32_t)(record_total_time / 1000);
	printf("rec_ttl_time is %i\n", rec_ttl_time);
        

	make_ts = 0;
	ts_stop_flag = 1;
        dvb_player->record_stop();
        sync();
	usleep(100000);
	//pthread_join(record_ThreadId, NULL);
	
	make_ts = 0;
        ts_f = fopen(record_file,"a+");
	if(ts_f)
	{
		
		fseek(ts_f, 0, SEEK_END);

		long filesize = ftell(ts_f);
		int crash = filesize % 188;
		if(crash != 0)
		{
			fseek(ts_f, -crash -188, SEEK_END);
			int i;
			char cff = 0xff;
			for(i = 0; i < crash; i++)
			{
				fwrite(&cff, sizeof(char), 1, ts_f);
				//fflush(ts_f);
			}
		}
		else
		{
                        fseek(ts_f, -188, SEEK_END);
		}
		int32_t i0 = -1;
    
		fwrite(&i0, sizeof(i0), 1, ts_f);
  
		fwrite(&rec_ttl_time,sizeof(rec_ttl_time),1,ts_f);

		int i;
		i0 = 0;
		for(i = 0; i < 45; i++)
		{
			fwrite(&i0, sizeof(i0), 1, ts_f);
		}
		//fflush(ts_f);
		fclose(ts_f);
		record_total_time = 0;
		rec_ttl_time = 0;
		if(crash != 0)
		{
			printf("record file crash\n");
			//return -1;
		}
	}

	return 0;
}

int dtv::free_space(int file)
{
	struct statfs stbuf;
	if(fstatfs(file, &stbuf) < 0)
		return -1;
	return stbuf.f_bfree * stbuf.f_bsize / 1024;/*allbytes in kilo*/
}


void *dtv::record_thread(void *p)
{
    dtv *parent_p;
    parent_p = ((dtv*)p);
    
    if(p==NULL) parent_p->ts_stop_flag = 1;

	int len = 0;
	int wlen = 0;	
	parent_p->ts_stop_flag = 0;
	stream_t  rec_st;


	printf("write the first two 188 pat_pmt to the file\n");
	fwrite(parent_p->get_pat_pmt_copy(),sizeof(char),(2*TS_PACKET_SIZE), parent_p->ts_f);	
	


	while(!parent_p->ts_stop_flag)
	{		
		/*record av data*/		
		rec_st.read_len = 300*188;
		rec_st.buff_idx = TSI_MEDIA_DATA_BUFFINDEX;		
		rec_st.rec_margin_len = 300*188;	
		len = tsi_fill_buffer_nb_rec(&rec_st);
		
		/*check paused or not*/
		if(1 == parent_p->make_ts)
		{
			if(len > 0)
			{
				//printf("len is %i\n", len);
#ifdef REC_TIME
				if(rec_time_i < 1994)
				{
					gettimeofday(&tv, NULL);
					rec_time[rec_time_i++] = tv.tv_sec; 
					rec_time[rec_time_i++] = tv.tv_usec; 
				}
#endif
				
				wlen = fwrite((unsigned int *)rec_st.virt_address,sizeof(char),len, parent_p->ts_f);			
				
			}	
			/* len check */
			if(wlen < len)
			{
				printf("JAKAY debug 7\n");
				
				if(parent_p->free_space(fileno(parent_p->ts_f))  < 1000)
				{
					parent_p->make_ts = 0;
					parent_p->ts_stop_flag = 1;
					//DEMUXER_ERROR = DEMUXER_NOSPACE_ERR;
					//no_space_cb(user_data_);
				}
			}
	
		}

		if(parent_p->ts_stop_flag == 1)
		{	
			printf("ts_stop_flag = 1, break\n");
			break;
		}

		usleep(5000);
               // msleep(5);
  	}	

	return NULL; 
}



bool dtv::DTV_Start_Record(char *file)
{
    if(is_playing==false) return false;
    
    memset(record_file,0,256);
    strcpy(record_file,file);

    if(ts_start_record(file)==false)
    {
        DTV_CORE_ERROR_DEBUG("Blad nagrywania!\n");
        return false;
    }
    
    is_recording = true;
    return true;
}
    

bool dtv::DTV_Stop_Record()
{
    if(is_recording==false) return false;
    
    ts_stop_record(record_file);
    is_recording = false;
    
    return true;
}