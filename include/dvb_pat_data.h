/** 
 * @file   dvb_pat_data.h
 * @author michalm
 *
 * @date   27 czerwiec 2011, 13:16
 */

#include <stdint.h>
#include "multiplex.h"

#include "struct_define.h"
#include "section.h"


#ifndef DVB_PAT_DATA_H
#define	DVB_PAT_DATA_H

#define PAT_DEBUG_T       1
#if PAT_DEBUG_T
        #define PAT_DEBUG(fmt, args...)   printf("[PAT][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define PAT_DEBUG(fmt, args...)
#endif


/**
 * @brief Klasa dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania.Pobiera informacje PAT sieci
 */
class dvb_pat_data {
public:
    dvb_pat_data();
    ~dvb_pat_data();
    uint16_t exec_pat_find(dvb_multiplex *p);
private:
    section_usr sbuf;

};

#endif	/* DVB_PAT_DATA_H */

