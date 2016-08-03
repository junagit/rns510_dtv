/** 
 * @file   descriptors.h
 * @author michalm
 *
 * @date   30 czerwiec 2011, 07:43
 */

#ifndef DESCRIPTORS_H
#define	DESCRIPTORS_H

#include "multiplex.h"

#define DATA_TYPE_MPEG 0
#define DATA_TYPE_DVB 1
#define DATA_TYPE_ATSC 2

#define DESCRIPTORS_DEBUG_T       0
#if DESCRIPTORS_DEBUG_T
        #define DESCRIPTORS_DEBUG(fmt, args...)   printf("[DESCRIPTORS][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define DESCRIPTORS_DEBUG(fmt, args...)
#endif

void parse_descriptor(struct descriptor *d, int indent, int data_type,dvb_multiplex *p);
void parse_section(uint8_t *buf, int len, int pid, int data_type,dvb_multiplex *p);

#define PID_PAT                         0x00
#define PID_NIT                         0x10
#define PID_SDT                         0x11
#define PID_EIT                         0x12    
#define PID_TOT                         0x14
#define PID_TDT                         0x14  
/**
 * @brief Klasa dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania i parsowania danych(deskryptorów) odebranych z sieci
 */
class descriptor_parser {
    
public:
    descriptor_parser()
    {
        curr_parsing_pmt_service = NULL;
        curr_parsing_pmt_service_pid = NULL;
    };
    
    ~descriptor_parser()
    {
        
    };
    
    void parse_descriptor(struct descriptor *d, int indent, int data_type,dvb_multiplex *p);
    uint8_t parse_section(uint8_t *buf, int len, int pid, int data_type,dvb_multiplex *p);
    void parse_dvb_section(uint8_t *buf, int len, int pid, int data_type, struct section *section,dvb_multiplex *p);
    void parse_dvb_descriptor(struct descriptor *d, int indent, int data_type,dvb_multiplex *p);
private:
    dvb_service *curr_parsing_pmt_service;
    dvb_service_pid *curr_parsing_pmt_service_pid;
};

#define NETWORK_NAME_DESCRIPTOR     0x40
#define SERVICE_LIST_DESCRIPTOR     0x41
#define STUFFING_DESCRIPTOR         0x42
#define SATELITE_DELIVERY_SYSTEM_DESCRIPTOR 0x43
#define CABLE_DELIVERY_SYSTEM_DESCRIPTOR 0x44
#define VBI_DATA_DESCRIPTOR         0x45
#define VBI_TELETEXT_DESCRIPTOR     0x46
#define BOUQUET_NAME_DSC            0x47
#define SERVICE_DSC                 0x48
#define COUNTRY_AVAILABILITY_DSC    0x49
#define LINKAGE_DSC                 0x4a
#define NVOD_REFERENCE_DSC          0x4b
#define TIME_SHIFTED_SERVICE_DSC    0x4c
#define SHORT_EVENT_DSC             0x4d
#define EXTENDED_EVENT_DSC          0x4e
#define TIME_SHIFTED_EVENT_DSC      0x4f
#define COMPONENT_DSC               0x50
#define MOSAIC_DSC                  0x51
#define STREAM_IDENTIFIER_DSC       0x52
#define CA_IDETIFIER_DSC            0x53
#define CONTENT_DSC                 0x54
#define PARENTAL_RATING_DSC         0x55
#define TELETEXT_DSC                0x56
#define TELEPHONE_DSC               0x57
#define LOCAL_TIME_OFFSET_DSC       0x58
#define SUBTITLING_DSC              0x59
#define TERESTIAL_DELIVERY_SYSTEM_DSC 0x5a
#define MULTILINGUAL_NETWORK_NAME_DSC 0x5b
#define MULTILINGUAL_BOUQUET_NAME_DSC 0x5c
#define MULTILINGUAL_SERVICE_NAME_DSC 0x5D
#define MULTILINGUAL_COMPONENT_NAME_DSC 0x5E


#endif	/* DESCRIPTORS_H */

