/** 
 * @file   multiplex.h
 * @author michalm
 *
 * @date   20 grudzień 2011, 14:08
 */
#include "service_list.h"

#ifndef MULTIPLEX_H
#define	MULTIPLEX_H

/**
 * @brief Struktura dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania
 */
typedef struct 
{
    uint32_t            frequency;                      //Częstotliwość        
    uint16_t            network_id;                     //!<id sieci
    uint16_t            original_network_id;
    uint16_t            transport_stream_id;            //!<id TS
    uint8_t             network_name[128];              //!<nazwa sieci
    dvb_service_list    *service_list;                   //!<lista ze znalezionymi programami
}dvb_multiplex;


#endif	/* MULTIPLEX_H */

