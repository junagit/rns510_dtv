/** 
 * @file   dvb_pat_data.cpp
 * @author Michał Motyl
 *
 * @date   27 czerwiec 2011, 13:16
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

#include "section.h"
#include "dvb_pat_data.h"
#include "struct_define.h"
#include "misc_func.h"
#include "descriptors.h"
#include "crc32.h"

extern FILE *ts_file;


dvb_pat_data::dvb_pat_data() 
{

}



dvb_pat_data::~dvb_pat_data() 
{

    
}

uint16_t dvb_pat_data::exec_pat_find(dvb_multiplex *p)
{
    int res;
    stream_t my_stream;

    descriptor_parser dp;
    uint8_t parse_result;
    

    memset(&sbuf,0,sizeof(section_usr));


    my_stream.buffer = sbuf.databuf;
    my_stream.read_len = 188;
    my_stream.ops = 0;
    my_stream.PID_type = PSI;
    my_stream.buff_idx = TSI_CTRL_BUFFINDEX;

    my_stream.time_out = 100;//set the timeout to 2 seconds
    my_stream.type = 1;

    tsi_add_pid_to_filter(PID_PAT,PSI,my_stream.buff_idx);


    while(1)
    {
       res = find_section_live(PID_PAT,&sbuf,&my_stream,3);
       
       if(res==3)
       {
           PAT_DEBUG("Find PAT timeout...\n");
           if(sbuf.section_bufs!=NULL) 
           {
                free(sbuf.section_bufs);
                sbuf.section_bufs = NULL;
            }

            tsi_remove_pid_from_filter(PID_PAT);

           return 1;
       }

       if(res==0)
       {    
           if(_dvb_crc32(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len))
          {
                //EPG_DEBUG("CRC ERROR sbuf_epg.section_bufs->len:%d\n",sbuf_epg.section_bufs->len);
               printf("%s %d:CRC ERROR!!!\n",__FUNCTION__,__LINE__);
                
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
           printf("####### %s %d\n",__FUNCTION__,__LINE__);
            parse_result = dp.parse_section(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len, PID_PAT, DATA_TYPE_DVB,p);

            section_buf_reset(sbuf.section_bufs);
                   
                if(sbuf.section_bufs!=NULL) {
                    free(sbuf.section_bufs);
                    sbuf.section_bufs = NULL;
                }
            
                
                if(parse_result==0)
                {
                    #ifdef TSI_LIVE
                    tsi_remove_pid_from_filter(PID_PAT);
                    #endif
                    return 0;
                }
                else
                {
                    break;
                }
       }
    }
    
    if(sbuf.section_bufs!=NULL) {
        free(sbuf.section_bufs);
        sbuf.section_bufs = NULL;
    }
    

    tsi_remove_pid_from_filter(PID_PAT);

    return 1;
    
}


