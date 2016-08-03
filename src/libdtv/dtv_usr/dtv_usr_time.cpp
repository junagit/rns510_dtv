/** 
 * @file   dtv_usr_time.cpp
 * @author Michał Motyl
 *
 * @date   3 kwiecień 2012, 11:56
 */

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <pthread.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <stdint.h>

#include <sys/prctl.h>
#include "multiplex.h"
#include "service_management.h"
#include "eit.h"
#include "dtv_core.h"
#include "crc32.h"


#ifdef TSI_LIVE
#include "dvbtcontrol.h"
#include "gpio_ctrl.h"
#include "tsi.h"
#endif


/**
 * 
 * @param p
 * @return 
 */
void *dtv::time_update_thread(void *p)
{
    struct timeval set_a;
    struct timezone set_b;
    struct tm * ptm;
    dtv *parent_p;
    time_t current_time;
    parent_p = ((dtv*)p);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    
    char name [17];	/* Name must be <= 16 characters + a null */

    strcpy (name,"time_update_thread");
    prctl (PR_SET_NAME, (unsigned long)&name);
    
    while(1)
    {
        pthread_testcancel();
        
        if(!parent_p->network_time.get_tot(current_time,parent_p->time_offset))
        {
//            DTV_CORE_DEBUG("Pobralem czas!!! Obecny czas:%s\n",ctime(&current_time));
            
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
            break;
        }
//        else
//        {
//            DTV_CORE_ERROR_DEBUG("Blad pobierania czasu!!!\n");
//        }
    }
    
    set_a.tv_sec=current_time;
    set_a.tv_usec=0;
    set_b.tz_minuteswest=0;
    set_b.tz_dsttime=0;
    
    if (settimeofday(&set_a,&set_b)!=0) 
        {
                DTV_CORE_ERROR_DEBUG("Nie moge ustawic czasu!!!\n");
                parent_p->time_is_set = false;
        }
        else 
        {       
            struct rtc_time rt;
            ptm = gmtime ( &current_time );

            rt.tm_hour = ptm->tm_hour;
            rt.tm_isdst = ptm->tm_isdst;
            rt.tm_mday = ptm->tm_mday;
            rt.tm_min = ptm->tm_min;
            rt.tm_mon = ptm->tm_mon;
            rt.tm_sec = ptm->tm_sec;
            rt.tm_wday = ptm->tm_wday;
            rt.tm_yday = ptm->tm_yday;
            rt.tm_year = ptm->tm_year;

            /* set your values here */
            int fd = open("/dev/rtc0", O_RDWR);
            ioctl(fd, RTC_SET_TIME, &rt);
            close(fd);
            parent_p->time_is_set = true;
            //system("hwclock --noadjfile --systohc --localtime");
            //DTV_CORE_DEBUG("Czas ustawiony!!!\n");
        }
    
    parent_p->network_time.clean_time_data();
    parent_p->time_is_updating = false;

    return NULL;
}

/**
 * 
 * @return 
 */
uint8_t dtv::init_time_thread(void)
{
    int res;
    
    if(time_is_updating == true) return 1;
    
    res = pthread_create(&time_ThreadId, NULL, &dtv::time_update_thread,this);
        
    if(res<0)
    {
        DTV_CORE_ERROR_DEBUG("Bląd tworzenia wątku '%s'.\n", strerror(errno));
        return 1;
    }
    time_is_updating = true;
    return 0;
}

/**
 * 
 * @return 
 */
uint8_t dtv::exit_time_thread(void)
{
    pthread_cancel(time_ThreadId);
    pthread_join(time_ThreadId,NULL);

    network_time.clean_time_data();
    time_is_updating = false;
    return 0;
}



void dtv::DTV_TimeUpdate(void)
{
    if(DTV_Is_Playing()==false) return;
    
    if(time_is_updating==false)init_time_thread();
}
    
void dtv::DTV_TimeUpdateStop(void)
{
    if(time_is_updating==true) exit_time_thread();
}

bool dtv::DTV_TimeIsUpdating(void)
{
    return time_is_updating;
}