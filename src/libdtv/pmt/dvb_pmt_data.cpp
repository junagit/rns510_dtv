/* 
 * File:   pmt.cpp
 * Author: Michał Motyl
 * 
 * Created on 6 maj 2011, 12:31
 */
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>

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
#include <time.h>


#include "struct_define.h"
#include "section.h"
#include "dvb_pat_data.h"
#include "dvb_sdt_data.h"
#include "dvb_pmt_data.h"
#include "descriptors.h"
#include "crc32.h"

extern FILE *ts_file;

#define PMT_DEBUG_T       1
#if PMT_DEBUG_T
        #define PMT_DEBUG(fmt, args...)   printf("[PMT][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define PMT_DEBUG(fmt, args...)
#endif

dvb_pmt_data::dvb_pmt_data()
{
    memset(&sbuf,0,sizeof(section_usr));
    sect = NULL;
}

dvb_pmt_data::~dvb_pmt_data() 
{
    if(sbuf.section_bufs!=NULL) {
    free(sbuf.section_bufs);
    sbuf.section_bufs = NULL;  
    }
    
}


uint8_t dvb_pmt_data::find_pmt_for_services(dvb_multiplex *p)
{
   descriptor_parser dp;     
   stream_t my_stream;

    uint16_t j;
    time_t starttime;
    uint16_t service_cnt = p->service_list->get_list_count();
    int res;
    uint8_t parse_result;
    bool got_error = false;
    dvb_service *service_tmp = NULL;
    memset(&sbuf,0,sizeof(section_usr));
    
    for(j = 0;j<service_cnt;j++)
    {
        if(got_error==true) 
        {
                if(sbuf.section_bufs!=NULL) {
                free(sbuf.section_bufs);
                sbuf.section_bufs = NULL;  
                }

                tsi_remove_pid_from_filter(-1);
                return 4;
        }
        
        service_tmp = p->service_list->get_from_list(j);
        
        if(service_tmp==NULL) continue;
        
        starttime = time(NULL);
        memset(&sbuf,0,sizeof(section_usr));
        my_stream.buffer = sbuf.databuf;
        my_stream.read_len = (188*2);
        my_stream.ops = 0;
        my_stream.PID_type = PSI;
        my_stream.buff_idx = TSI_CTRL_BUFFINDEX;
        my_stream.time_out = 100;//set the timeout to 2 seconds
        my_stream.type = 1;
            
        tsi_add_pid_to_filter(service_tmp->getPMTpid(),PSI,my_stream.buff_idx);

        while(1)
        {
            res = find_section_live(service_tmp->getPMTpid(),&sbuf,&my_stream,3);
            
           if(res==0)
           {
               if(_dvb_crc32(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len))
                {
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

                parse_result = dp.parse_section(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len,service_tmp->getPMTpid(), DATA_TYPE_DVB,p);

                section_buf_reset(sbuf.section_bufs);

                  if(sbuf.section_bufs!=NULL) 
                  {
                    free(sbuf.section_bufs);
                    sbuf.section_bufs = NULL;  
                  }
                
                if(parse_result==0)
                {
                    tsi_remove_pid_from_filter(service_tmp->getPMTpid());
                    break; 
                }
           }
           else
           {
               got_error = service_tmp->scan_error = true;
               printf("BŁAD POBIERANIA PMT!!!!!!!!\n");     
               break;
           }
        }
    }
  
    
    if(sbuf.section_bufs!=NULL) {
    free(sbuf.section_bufs);
    sbuf.section_bufs = NULL;  
    }
    
    tsi_remove_pid_from_filter(-1);

    return 0;
}