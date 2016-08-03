/* 
 * File:   section.h
 * Author: michal_m
 *
 * Created on 3 maj 2011, 09:18
 */

#ifndef SECTION_H
#define	SECTION_H

#include <libucsi/mpeg/descriptor.h>
#include <libucsi/mpeg/section.h>
#include <libucsi/dvb/descriptor.h>
#include <libucsi/dvb/section.h>
//#include <libucsi/atsc/descriptor.h>
//#include <libucsi/atsc/section.h>
#include <libucsi/transport_packet.h>
#include <libucsi/section_buf.h>
#include <libucsi/dvb/types.h>
#include <libdvbsec/dvbsec_api.h>
#include <libdvbsec/dvbsec_cfg.h>

#ifndef PC_VERSION
#include "interface.h"
#endif

#define DATA_BUF_SIZE           (188*200)

/**
 * @brief Struktura ze zmiennymi wykorzystywanymi zbierania sekcji danych DVB
 */
typedef struct
{
    uint8_t databuf[DATA_BUF_SIZE];
    uint16_t sz;
    uint16_t i;
    uint8_t used;
    uint16_t section_status;
    uint8_t continuities;
    struct section_buf *section_bufs;
    struct transport_packet *tspkt;
    struct transport_values tsvals;

}section_usr,*section_usr_p;

//uint8_t find_section(uint16_t pid,section_usr_p susr,int _dvrfd);
uint8_t find_section(int16_t pid,uint32_t timeout,section_usr_p susr,FILE *_dvrfd);

#ifndef PC_VERSION
uint8_t find_section_live(int16_t pid,section_usr_p susr,stream_t *my_stream,int tout);
uint8_t find_section_live2(int16_t pid,section_usr_p susr,stream_t *my_stream);

void init_section_mutex(void);

void destroy_section_mutex(void);
#endif

#endif	/* SECTION_H */

