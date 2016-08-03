/** 
 * @file   dvb_sdt_data.cpp
 * @author Michał Motyl
 *
 * @date   27 czerwiec 2011, 14:32
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
#include "dvb_sdt_data.h"
#include "misc_func.h"
#include "descriptors.h"
#include "crc32.h"
extern FILE *ts_file;


dvb_sdt_data::dvb_sdt_data() {
    
}



dvb_sdt_data::~dvb_sdt_data() {
    
}



uint16_t dvb_sdt_data::find_sdt(dvb_multiplex *p)
{
    stream_t my_stream;
    descriptor_parser dp;
    uint8_t parse_result = 0;
    
    memset(&sbuf,0,sizeof(section_usr));
  
    my_stream.buffer = sbuf.databuf;
    my_stream.read_len = 188;
    my_stream.ops = 0;
    my_stream.PID_type = PSI;
    my_stream.buff_idx = TSI_CTRL_BUFFINDEX;
    my_stream.time_out = 100;//set the timeout to 2 seconds
    my_stream.type = 1;

    tsi_add_pid_to_filter(PID_SDT,PSI,my_stream.buff_idx);
    
    while(1)
    {
        int res = find_section_live(PID_SDT,&sbuf,&my_stream,3);
        printf("####### %s %d\n",__FUNCTION__,__LINE__);
       if(res==3)
       {
           SDT_DEBUG("Find SDT timeout...\n");
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
               printf("%s %d:CRC ERROR!!!\n",__FUNCTION__,__LINE__);
                
                memset(&sbuf,0,sizeof(section_usr));

                if(sbuf.section_bufs!=NULL) {
                    section_buf_reset(sbuf.section_bufs);
                    free(sbuf.section_bufs);
                    sbuf.section_bufs = NULL;
                }
                
                continue;
          }
            parse_result = dp.parse_section(section_buf_data(sbuf.section_bufs),sbuf.section_bufs->len, PID_SDT, DATA_TYPE_DVB,p); 

            section_buf_reset(sbuf.section_bufs);
            
                if(sbuf.section_bufs!=NULL) {
                    free(sbuf.section_bufs);
                    sbuf.section_bufs = NULL;
                }
         
                if(parse_result==0)
                {
                    tsi_remove_pid_from_filter(PID_SDT);
                    return 0;    
                }
                else
                break;
                
       }
    }	

    if(sbuf.section_bufs!=NULL) {
        free(sbuf.section_bufs);
        sbuf.section_bufs = NULL;
    }
  
    tsi_remove_pid_from_filter(PID_SDT);

    return 0;
}
