/** 
 * @file   misc_func.h
 * @author michalm
 *
 * @date   27 czerwiec 2011, 13:42
 */

#include <iostream>
#include <stdint.h>

#include "eit.h"
#include "service_management.h"

#ifndef MISC_FUNC_H
#define	MISC_FUNC_H


    
//multiplex *create_multiplex(void);
//void destroy_multiplex(multiplex *p);
dvb_service *create_new_service(void);
void parseMJD(uint32_t mjd,dvb_ts_time *t);
int msleep(unsigned long milisec);




#endif	/* MISC_FUNC_H */

