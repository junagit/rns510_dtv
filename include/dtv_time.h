/** 
 * @file   dtv_time.h
 * @author michalm
 *
 * @date   19 marzec 2012, 08:09
 */

#include "multiplex.h"

#include "struct_define.h"
#include "section.h"

#ifndef DTV_TIME_H
#define	DTV_TIME_H




class dtv_time {
    
    friend class dtv;
public:
    dtv_time();
    ~dtv_time();
    uint8_t get_tot(time_t & t,time_t & t_off);
private:
    
    void clean_time_data(void);
    
    section_usr sbuf;

};

#endif	/* DTV_TIME_H */

