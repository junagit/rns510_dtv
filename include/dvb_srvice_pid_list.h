/** 
 * @file   dvb_srvice_pid_list.h
 * @author michalm
 *
 * @date   21 grudzień 2011, 07:27
 */


#include <stdint.h>
#include "dvb_service.h"

#ifndef DVB_SRVICE_PID_LIST_H
#define	DVB_SRVICE_PID_LIST_H


//0x0	= Reserved	
//0x1	= MPEG-1 Video	
//0x2	= MPEG-2 Video	
//0x3	= MPEG-1 Audio	
//0x4	= MPEG-2 Audio	
//0x5	= ISO 13818-1 private sections	
//0x6	= ISO 13818-1 PES private data	
//0x7	= ISO 13522 MHEG	
//0x8	= ISO 13818-1 DSM-CC	
//0x9	= ISO 13818-1 auxiliary	
//0xa	= ISO 13818-6 multi-protocol encap	
//0xb	= ISO 13818-6 DSM-CC U-N msgs	
//0xc	= ISO 13818-6 stream descriptors	
//0xd	= ISO 13818-6 sections	
//0xe	= ISO 13818-1 auxiliary	
//0xf	= MPEG-2 AAC Audio	
//0x10	= MPEG-4 Video	
//0x11	= MPEG-4 LATM AAC Audio	
//0x12	= MPEG-4 generic
//0x13	= ISO 14496-1 SL-packetized
//0x14	= ISO 13818-6 Synchronized Download Protocol
//0x1b	= H.264 Video
//0x80	= DigiCipher II Video
//0x81	= A52/AC-3 Audio 
//0x82	= HDMV DTS Audio
//0x83	= LPCM Audio
//0x84	= SDDS Audio
//0x85	= ATSC Program ID
//0x86	= DTS-HD Audio
//0x87	= E-AC-3 Audio  
//0x8a	= DTS Audio
//0x91	= A52b/AC-3 Audio
//0x92	= DVD_SPU vls Subtitle
//0x94	= SDDS Audio
//0xa0	= MSCODEC Video
//0xea	= Private ES (VC-1)


#define STREAM_TYPE_VIDEO_MPEG1         0x01 
#define STREAM_TYPE_VIDEO_MPEG2         0x02 
#define STREAM_TYPE_AUDIO_MPEG1         0x03 
#define STREAM_TYPE_AUDIO_MPEG2         0x04 
#define STREAM_TYPE_PRIVATE_SECTION     0x05 
#define STREAM_TYPE_PRIVATE_DATA        0x06 
#define STREAM_TYPE_AUDIO_AAC           0x0f 
#define STREAM_TYPE_AUDIO_AC3           0x81 
#define STREAM_TYPE_AUDIO_EAC3          0x87
#define STREAM_TYPE_VIDEO_MPEG4         0x10 
#define STREAM_TYPE_VIDEO_H264          0x1b 
#define STREAM_TYPE_DVB_SUBTITLES       0xEE 
#define STREAM_TYPE_DVB_TELETEXT        0xEF

/**
 * @brief Klasa listy ze stacjami DVB
 */
class dvb_srvice_pid_list {
public:
    dvb_srvice_pid_list();
    ~dvb_srvice_pid_list();
    uint8_t     add_to_list(dvb_service_pid *data);
    dvb_service_pid *   get_from_list(uint16_t pos);
    uint32_t    get_list_count(void);
    void        destroy_list(void);
private:
    typedef struct dvb_service_pid_list_node{

  dvb_service_pid                  *data;
  struct dvb_service_pid_list_node      *next;

}__attribute__((packed)) dvb_service_pid_list_node_t;

    dvb_service_pid_list_node_t *list_head;
    uint32_t    list_cnt;

};


#endif	/* DVB_SRVICE_PID_LIST_H */

