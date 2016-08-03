/** 
 * @file   dvb_network_info.h
 * @author michalm
 *
 * @date   30 czerwiec 2011, 08:01
 */

#ifndef DVB_NETWORK_INFO_H
#define	DVB_NETWORK_INFO_H


#define NETWORK_INFO_DEBUG_T       1
#if NETWORK_INFO_DEBUG_T
        #define NETWORK_INFO_DEBUG(fmt, args...)   printf("[NETWORK_INFO][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define NETWORK_INFO_DEBUG(fmt, args...)
#endif

#include <stdint.h>

#include "struct_define.h"

/**
 * @brief Klasa dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania.Pobiera informacje o sieci
 */
class dvb_network_info {
public:
    dvb_network_info();
    ~dvb_network_info();
    
    uint16_t exec_network_info_find(dvb_multiplex *p);
private:
    section_usr sbuf;
};

#endif	/* DVB_NETWORK_INFO_H */

