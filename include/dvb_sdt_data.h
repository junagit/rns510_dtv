/** 
 * @file   dvb_sdt_data.h
 * @author michalm
 *
 * @date   27 czerwiec 2011, 14:32
 */
#include "multiplex.h"
#include "struct_define.h"

#ifndef DVB_SDT_DATA_H
#define	DVB_SDT_DATA_H

#define SDT_DEBUG_T       1
#if SDT_DEBUG_T
        #define SDT_DEBUG(fmt, args...)   printf("[SDT][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define SDT_DEBUG(fmt, args...)
#endif

/**
 * @brief Klasa dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania.Pobiera informacje SDT
 */
class dvb_sdt_data {
public:
    dvb_sdt_data();
    ~dvb_sdt_data();
    uint16_t find_sdt(dvb_multiplex *p);
    

    
private:
    section_usr sbuf;
};

#endif	/* DVB_SDT_DATA_H */

