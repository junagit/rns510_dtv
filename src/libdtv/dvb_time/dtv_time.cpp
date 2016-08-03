/** 
 * @file   dtv_time.cpp
 * @author Michał Motyl
 *
 * @date   19 marzec 2012, 08:09
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <pthread.h>
#include <linux/rtc.h>


#include <iostream>
#include <stdint.h>

#include <libucsi/mpeg/descriptor.h>
#include <libucsi/mpeg/section.h>
#include <libucsi/dvb/descriptor.h>
#include <libucsi/dvb/section.h>
#include <libucsi/atsc/descriptor.h>
#include <libucsi/atsc/section.h>
#include <libucsi/transport_packet.h>
#include <libucsi/section_buf.h>
#include <libucsi/dvb/types.h>
#include <libdvbsec/dvbsec_api.h>
#include <libdvbsec/dvbsec_cfg.h>

#include "section.h"
#include "descriptors.h"
#include "crc32.h"

#ifdef TSI_LIVE    
#include "interface.h"
#endif

#include "dtv_time.h"


extern FILE *ts_file;

dtv_time::dtv_time() {
}



dtv_time::~dtv_time() {
}


//time_t dvbdate_to_unixtime2(dvbdate_t dvbdate)
//{
//	int k = 0;
//	struct tm tm;
//	double mjd;
//
//	/* check for the undefined value */
//	if ((dvbdate[0] == 0xff) &&
//	    (dvbdate[1] == 0xff) &&
//	    (dvbdate[2] == 0xff) &&
//	    (dvbdate[3] == 0xff) &&
//	    (dvbdate[4] == 0xff)) {
//		return -1;
//	}
//
//	memset(&tm, 0, sizeof(tm));
//	mjd = (dvbdate[0] << 8) | dvbdate[1];
//
//	tm.tm_year = (int) ((mjd - 15078.2) / 365.25);
//	tm.tm_mon = (int) (((mjd - 14956.1) - (int) (tm.tm_year * 365.25)) / 30.6001);
//	tm.tm_mday = (int) mjd - 14956 - (int) (tm.tm_year * 365.25) - (int) (tm.tm_mon * 30.6001);
//	if ((tm.tm_mon == 14) || (tm.tm_mon == 15)) k = 1;
//	tm.tm_year += k;
//	tm.tm_mon = tm.tm_mon - 2 - k * 12;
//	tm.tm_sec = bcd_to_integer(dvbdate[4]);
//	tm.tm_min = bcd_to_integer(dvbdate[3]);
//	tm.tm_hour = bcd_to_integer(dvbdate[2]);
//        tm.tm_isdst = -1;//bez tego mktime tworzy czas przesuniety do przodu o godzine!!!
//    //    printf("tm.tm_hour:%d  tm.tm_min:%d tm.tm_sec:%d\n",tm.tm_hour,tm.tm_min,tm.tm_sec );
//
//	return mktime(&tm);
//}

uint8_t dtv_time::get_tot(time_t & t,time_t & t_off)
{
#ifdef TSI_LIVE    
stream_t my_stream;
#endif
uint16_t real_read_len;
struct dvb_tot_section *tot;
struct descriptor *curd;
time_t dvbtime;
struct section *section = NULL;
int retry_counter = 0;
int res,time_zone;

    memset(&sbuf,0,sizeof(section_usr));

    real_read_len = READ_LEN;

    my_stream.buffer = sbuf.databuf;
    my_stream.read_len = 188;
    my_stream.ops = 0;
    my_stream.PID_type = PSI;
    my_stream.buff_idx = TSI_TIME_BUFFINDEX;

    my_stream.time_out = 100;//set the timeout to 2 seconds
    my_stream.type = 1;

    tsi_add_pid_to_filter(PID_TOT,PSI,my_stream.buff_idx);

    
    while(1)
    {
        pthread_testcancel();
        
        res = find_section_live(PID_TOT,&sbuf,&my_stream,3);
        
       if(res==0)
       {
         //  printf("%s: %d sbuf.section_bufs->len:%d\n",__FUNCTION__,__LINE__,sbuf.section_bufs->len);
          if(_dvb_crc32(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len))
          {
              
          //    printf("get_tot crc error!!!!\n");
                sbuf.continuities = 0;
                sbuf.i = 0;
                sbuf.section_status = 0;
                sbuf.sz = 0;
                memset(&sbuf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf.databuf,0,DATA_BUF_SIZE);
                sbuf.used = 0;

                if(sbuf.section_bufs!=NULL) section_buf_reset(sbuf.section_bufs);
                
                continue;
          }
           
           section = section_codec(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len);

            if (section == NULL) {
                section_buf_reset(sbuf.section_bufs);

                if(sbuf.section_bufs!=NULL)
                {
                   free(sbuf.section_bufs);
                   sbuf.section_bufs = NULL;
                }
                
		continue;
            }


           if(section->table_id!=0x73)
           {
     //       section_buf_reset(sbuf.section_bufs);
                sbuf.continuities = 0;
                sbuf.i = 0;
                sbuf.section_status = 0;
                sbuf.sz = 0;
                memset(&sbuf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf.databuf,0,DATA_BUF_SIZE);
                sbuf.used = 0;

                if(sbuf.section_bufs!=NULL) section_buf_reset(sbuf.section_bufs);
                
                continue;
           }
           
           if(section->syntax_indicator==1)
           {
              section_buf_reset(sbuf.section_bufs);
              continue; 
           }
                
		if ((tot = dvb_tot_section_codec(section)) == NULL) {
			fprintf(stderr, "SCT XXXX TOT section decode error\n");

                    if(sbuf.section_bufs!=NULL)
                    {
                        free(sbuf.section_bufs);
                        sbuf.section_bufs = NULL;
                    }
                    
                    continue;
		}
          
		dvbtime = dvbdate_to_unixtime(tot->utc_time);
		dvb_tot_section_descriptors_for_each(tot, curd) {
                    
                struct dvb_local_time_offset_descriptor *dx;
		struct dvb_local_time_offset *cur;

		dx = dvb_local_time_offset_descriptor_codec(curd);
                
		if (dx == NULL) 
                {
                    fprintf(stderr, "DSC XXXX dvb_local_time_offset_descriptor decode error\n");

                    if(sbuf.section_bufs!=NULL)
                    {
                        free(sbuf.section_bufs);
                        sbuf.section_bufs = NULL;
                    }
                    continue;
		}
                
                
                
		dvb_local_time_offset_descriptor_offsets_for_each(dx, cur) 
                {
//                    printf("DSC country_code:%.3s country_region_id:%i "
//                            "local_time_offset_polarity:%i local_time_offset:%i "
//                            "time_of_change:%i next_time_offset:%d\n",
//                            cur->country_code, cur->country_region_id,
//                            cur->local_time_offset_polarity,
//                            dvbhhmm_to_seconds(cur->local_time_offset),
//                            (int)dvbdate_to_unixtime(cur->time_of_change),
//                            dvbhhmm_to_seconds(cur->next_time_offset));

                            time_t offset = dvbhhmm_to_seconds(cur->local_time_offset);
                            //dvbtime+=offset;

                            t = dvbtime;
                            t += offset;
                            
                            
                            time_zone = (offset/3600);
                            t_off = offset;
                         //   printf("offset:%d  time_zone:%d\n",offset,time_zone);

                            if(sbuf.section_bufs!=NULL)
                            {
                               free(sbuf.section_bufs);
                               sbuf.section_bufs = NULL;
                            }

                            tsi_remove_pid_from_filter(PID_TOT);
                            return 0;
                    }
            }

       }
       else
       {
           retry_counter++;
       }
       
       
       if(retry_counter==3) break;
       
 
    }


    tsi_remove_pid_from_filter(PID_TOT);
    
    if(sbuf.section_bufs!=NULL)
    {
       free(sbuf.section_bufs);
       sbuf.section_bufs = NULL;
    }
    
    return 1;
}


void dtv_time::clean_time_data(void)
{
    tsi_remove_pid_from_filter(PID_TOT);
    
    if(sbuf.section_bufs!=NULL)
    {
       free(sbuf.section_bufs);
       sbuf.section_bufs = NULL;
    }
}