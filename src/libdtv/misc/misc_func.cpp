/** 
 * @file   misc_func.cpp
 * @author Michał Motyl
 *
 * @date   27 czerwiec 2011, 13:38
 */

#include <iostream>
#include <stdint.h>
#include <string.h>
#include "multiplex.h"
#include "misc_func.h"


dvb_multiplex *create_multiplex(void)
{
    dvb_multiplex *temp = new dvb_multiplex;
    
    if(temp!=NULL)temp->service_list = new dvb_service_list();
    
    return temp;
}


dvb_service *create_new_service(void)
{
    dvb_service *temp = new dvb_service;
    
    if(temp!=NULL) memset(temp,0,sizeof(dvb_service));
    
    return temp;
}




void parseMJD(uint32_t mjd,dvb_ts_time *t)
{   
	int year = (int) ((mjd - 15078.2) / 365.25);
	int month = (int) ((mjd - 14956.1 - (int) (year * 365.25)) / 30.6001);
	int day = mjd - 14956 - (int) (year * 365.25) - (int) (month * 30.6001);
	int i = (month == 14 || month == 15) ? 1 : 0;
	year += i ;
	month = month - 1 - i * 12;
	t->year= year;
	t->month = month;
	t->day = day;
	return;
}

//int __nsleep(const struct timespec *req, struct timespec *rem)
//{
//    struct timespec temp_rem;
//    if(nanosleep(req,rem)==-1)
//        __nsleep(rem,&temp_rem);
//    else
//        return 1;
//}
//
//int msleep(unsigned long milisec)
//{
//    struct timespec req={0},rem={0};
//    time_t sec=(int)(milisec/1000);
//    milisec=milisec-(sec*1000);
//    req.tv_sec=sec;
//    req.tv_nsec=milisec*1000000L;
//    __nsleep(&req,&rem);
//    return 1;
//}


