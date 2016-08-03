/** 
 * @file   dtv_usr_epg.cpp
 * @author Michał Motyl
 *
 * @date   3 kwiecień 2012, 11:54
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
#include <time.h>
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
 * @brief Wątek pobierania epg
 * @param p
 * @return 
 */
void *dtv::epg_thread(void *p)
{
    uint8_t res;
    dtv *parent_p;
    
    parent_p = ((dtv*)p);
    service_manager *service_man_temp = new service_manager (); 
    service_man_temp->Init_services_data_for_user(false);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
 
    char name [17];	/* Name must be <= 16 characters + a null */

    strcpy (name,"epg_thread");
    prctl (PR_SET_NAME, (unsigned long)&name);
    
    DTV_CORE_DEBUG("wchodze do watku EPG....\n");
    
    while(1)
    {
        res = parent_p->epg.Download_Epg(parent_p->current_service->frequency,parent_p->current_service->transport_stream_id,service_man_temp->user_service_list);
        
        if(res==0){
            DTV_CORE_DEBUG("Pobralem EPG!!!\n");
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            //czekamy jezeli inny watek czyta z epg
            while(pthread_mutex_trylock(&parent_p->epg_schedule_lock)!=0) usleep(500000);
            //pthread_mutex_lock(&parent_p->epg_schedule_lock);
  
            parent_p->epg.Save_EPG_data_for_each_service(service_man_temp->user_service_list,parent_p->current_service->frequency,parent_p->current_service->transport_stream_id);
           
            pthread_mutex_unlock(&parent_p->epg_schedule_lock); 
   
            break;
        }
        
        if(res==2) {
            DTV_CORE_ERROR_DEBUG("Timeout pobierania EPG!!!\n");
            break;
        }
    }

   
    delete service_man_temp;
    parent_p->dtv_epg_downloading = false;
    return NULL;
}


/**
 * @brief Funkcja wew. inicjalizująca wątek pobierania epg
 * @return 0 - ok, 1 - bład
 */
uint8_t dtv::init_epg_thread(void)
{
    int res;
    
    res = pthread_create(&epgThreadId, NULL, &dtv::epg_thread,this);

    if(res<0)
    {
        DTV_CORE_ERROR_DEBUG("Bląd tworzenia wątku '%s'.\n", strerror(errno));
        return 1;
    }
    
    return 0;
}



/**
 * @todo Sprawdzic dizałanie tej funkcji!!!!
 * @brief Funkcja wewnętrzna,która anuluje pobieranie epg(kończy wątek pobierania epg)
 * @return 
 */
uint8_t dtv::exit_epg_thread(void)
{
    pthread_cancel(epgThreadId);
    
    pthread_join(epgThreadId,NULL);
    
    dtv_epg_downloading=false;

    if(current_service!=NULL)epg.clean_epg_download_res(current_service->frequency,current_service->transport_stream_id,service_management->user_service_list);
    
    tsi_remove_pid_from_filter(PID_EIT);
    
    epg.epg_sched_busy = false;
    return 0;
}

/**
 * @brief Funkcja zwraca wskaźnik do listy z eventami dla określonego dnia
 * @param ch - numer kanału na liscie
 * @param day - dzień[0-7]
 * @param events_cnt - do tego wskaźnika jest wpisywana ilość eventów w zwracanej tablicy
 * @return NULL jeśli błąd
 */
dvb_epg_events_list *dtv::DTV_EPG_Get_Channel_Events_List(dvb_service *s,uint8_t day,uint16_t & events_cnt)
{
    void *ptr = NULL;
    uint8_t epg_file_name[255];
    FILE *fp = NULL;
    uint32_t file_len;
    epg_info_entry progs_cnt_in_day[8];
    uint16_t i;
    eit_event *event = NULL;
    uint8_t *offset = NULL;
    uint32_t day_size = 0;
    uint16_t read_start_header,txt_len,cnt = 0;
    
    if(pthread_mutex_trylock(&epg_schedule_lock)!=0) return NULL;
    //pthread_mutex_lock(&epg_schedule_lock);
    
    memset(epg_file_name,0,255);
    
    
    if(s==NULL) {
        events_cnt = 0;
        pthread_mutex_unlock(&epg_schedule_lock);
        return NULL;
    }
    
    if(day>=8)
    {
        events_cnt = 0;
        
        pthread_mutex_unlock(&epg_schedule_lock);
        return NULL;   
    }

    sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",s->frequency,s->service_id,s->transport_stream_id);
    
    //DTV_CORE_DEBUG("EPG FILE:%s\n",epg_file_name);
    
    fp = fopen((char*)epg_file_name,"rb");

    if(fp==NULL) 
    {
        DTV_CORE_ERROR_DEBUG("Can't open epg file...\n");
        events_cnt = 0;
        
        pthread_mutex_unlock(&epg_schedule_lock);
        return NULL;
    }

    //pobierz długość pliku
    fseek(fp,0,SEEK_END);
    file_len = ftell(fp);
    fseek(fp,0,SEEK_SET);
        
   if(file_len==0) 
   {
        events_cnt = 0;
        pthread_mutex_unlock(&epg_schedule_lock);
        return NULL;
   }
   
    ptr = (unsigned char*)mmap(0,file_len,PROT_READ,MAP_SHARED,fileno(fp),0);
 
    if(ptr==NULL)
    {
        events_cnt = 0;
        fclose(fp);
        pthread_mutex_unlock(&epg_schedule_lock);
        return NULL;
    }


    if(s->epg_temp_events_list.get_list_count()!=0) s->epg_temp_events_list.destroy_list();

    memcpy(progs_cnt_in_day,ptr,(64));
    day_size = progs_cnt_in_day[day].epg_day_size_in_bytes;

    if(day>0)
    {
        offset = ((uint8_t*)ptr + (8*sizeof(epg_info_entry)));
        
        for(i = 0;i<(day);i++)
        {
            offset+=progs_cnt_in_day[i].epg_day_size_in_bytes;
        }
    }
    else
    {
        offset = ((uint8_t*)ptr + (8*sizeof(epg_info_entry)));
    }
   
    
        while(day_size)
        {
            memcpy(&read_start_header,(void*)offset,2);
            
            day_size-=2;
            offset+=2;
 
            if(read_start_header==0x55aa) 
            {
                event = new eit_event;
                memset(event,0,sizeof(eit_event));//konieczne!!!
                
                if(event!=NULL)
                {
                    //odczyt ID
                    memcpy(&event->event_id,(void*)offset,2);
                    day_size-=2;//event id
                    offset+=2;
                    
                    //odczyt start time
                    memcpy(&event->start_time,(void*)offset,sizeof(time_t));
                    day_size-=sizeof(time_t);//start time + end_time
                    offset+=sizeof(time_t);
                    
                    //odczyt end time
                    memcpy(&event->end_time,(void*)offset,sizeof(time_t));
                    day_size-=sizeof(time_t);//start time + end_time
                    offset+=sizeof(time_t);

                    //odczyt długosci tytulu
                    memcpy(&txt_len,(void*)offset,2);
                    if(txt_len>0)
                    {
                        day_size-=2;
                        offset+=2;
                        
                        //odczyt tytulu
                        event->title = new char[txt_len+1];
                        memset(event->title,0,txt_len+1);
                        memcpy(event->title,offset,txt_len);
                        day_size-=txt_len;
                        offset+=txt_len;
                        txt_len = 0;
                    }
                    else
                    {
                        event->title = NULL;
                        txt_len = 0;
                        day_size-=2;
                        offset+=2;
                    }
                   
              
                    //odczyt dlugosci opisu
                    memcpy(&txt_len,(void*)offset,2);
                    
                    if(txt_len>0)
                    {
                        day_size-=2;
                        offset+=2;
                        
                        //odczyt opisu
                        event->des = new char[txt_len+1];
                        memset(event->des ,0,txt_len+1);
                        memcpy(event->des ,(void*)offset,txt_len);

                        offset+=txt_len;
                        day_size-=txt_len;
                        txt_len = 0;
                    }
                    else
                    {
                        event->des = NULL;
                        day_size-=2;
                        offset+=2;
                        txt_len = 0;
                    }
   
                    
                   //s->events_tab_for_one_day[cnt] = event;
                    s->epg_temp_events_list.add_to_list(event);
                    
                   cnt++;
                   if(cnt==progs_cnt_in_day[day].epg_events_in_day)
                   {
                       cnt=0;
                   }
                   
                }
            }
            else
            {
                DTV_CORE_ERROR_DEBUG("WRONG HEADER....read_start_header:0x%x\n",read_start_header);
                events_cnt = 0;
    
                if(ptr!=NULL)munmap(ptr,file_len);

                fclose(fp);
                remove((char*)epg_file_name);
                sync();
                pthread_mutex_unlock(&epg_schedule_lock);
                return NULL;
            }
            
        }
    
   
    
    //DTV_CORE_DEBUG("DTV_EPG_Get_Channel_Events_List OK...\n");
    
    events_cnt = progs_cnt_in_day[day].epg_events_in_day;
    
    if(ptr!=NULL)munmap(ptr,file_len);
    
    fclose(fp);
    
    pthread_mutex_unlock(&epg_schedule_lock);
    
    return &s->epg_temp_events_list;
    
}

//####################################################################################################################

/**
 * @brief Funkcja wew. inicjalizująca wątek pobierania EPG present/following
 * @return 0 - ok, 1 - bład
 */
uint8_t dtv::init_epg_pf_thread(void)
{
    int res;
    
    res = pthread_create(&epg_pf_ThreadId, NULL,&dtv::epg_pf_thread,this);
        
    if(res<0)
    {
        DTV_CORE_ERROR_DEBUG("Bląd tworzenia wątku dla EPG present following '%s'.\n", strerror(errno));
        return 1;
    }

    return 0;
}




//void dtv::epg_cleanup_handler(void *arg)
//{
//   DTV_CORE_DEBUG("Wychodze z pobierania EPG present/following...\n");
//}

/**
 * @brief Wątek pobierania EPG  dla present/following
 * @param p
 * @return 
 */
void *dtv::epg_pf_thread(void *p)
{
    uint8_t res = 0;
    uint8_t cnt = EPG_PF_DOWNLOAD_INTERVAL;
    dtv *parent_p;
    parent_p = ((dtv*)p);
    
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    
    char name [17];	/* Name must be <= 16 characters + a null */

    strcpy (name,"epg_pf_thread");
    prctl (PR_SET_NAME, (unsigned long)&name);
    
//    DTV_CORE_DEBUG("wchodze do watku EPG present/following...\n");
    
    while(1)
    {
        pthread_testcancel();
        
            usleep(500000);

            if(cnt--) continue;

            cnt = EPG_PF_DOWNLOAD_INTERVAL;
        
        if(pthread_mutex_trylock(&parent_p->present_following_lock)!=0) continue;
        
        res = parent_p->epg.Download_Epg_Present_Following(parent_p->current_service->frequency,parent_p->current_service->transport_stream_id,parent_p->service_management->user_service_list);

//        if(res==0){
//            printf("Pobralem EPG present/following!!!\n");
//            fflush(stdout);
//        }
        
        if(res==2) {
            DTV_CORE_ERROR_DEBUG("Timeout pobierania EPG present/following!!!\n");
        }
        
        pthread_mutex_unlock(&parent_p->present_following_lock);
    }
    
    
    DTV_CORE_DEBUG("wychodze z watku EPG present/folowing...\n");
    
    return((void *)0);
}

/**
 * @brief Funkcja wewnętrzna,która anuluje pobieranie epg(kończy wątek pobierania epg)
 * @return 
 */
uint8_t dtv::exit_epg_present_following_thread(void)
{
    pthread_cancel(epg_pf_ThreadId);
   
    pthread_join(epg_pf_ThreadId,NULL);

    epg.clean_epg_download_present_following_res(current_service->frequency,current_service->transport_stream_id,service_management->user_service_list);

    epg.epg_pf_busy = false;
    
    tsi_remove_pid_from_filter(PID_EIT);
    
    dtv_epg_pf_downloading = false;
    pthread_mutex_unlock(&present_following_lock);
    
    return 0;
}


/**
 * @brief Funkcja zwraca aktualną audycje na danym kanale
 * @param s
 * @return NULL jesli blad
 */
eit_event *dtv::DTV_Get_Current_Event(dvb_service *s)
{
    eit_event *e;
    
    if(s==NULL) return NULL;
    
    if(s->eit_present_following)
    {
        //ZABEZPIECZENIE:event mozemy pobrać tylko dla stacji która jest na tej częstotliwości!!! - tylko dla live
        if(s->frequency!=curr_freq) return NULL;
        
        //odczyt eventu mozliwy gdy dziala watek pobierania epg present/following
        if(dtv_epg_pf_downloading==false) return NULL;
    
        if(pthread_mutex_trylock(&present_following_lock)!=0) {
           // printf("mutex locked!!!\n");
            return NULL;
        }
        
        if(s->epg_pf_events_list.get_list_count()>0)
        {
            e = s->epg_pf_events_list.get_from_list(0);
            
            if(e==NULL) {
                
                if(event_present.title!=NULL)
                {
                    delete[] event_present.title;
                    event_present.title = NULL;
                }
                
                if(event_present.des!=NULL)
                {
                    delete[] event_present.des;
                    event_present.des = NULL;
                }
                
                pthread_mutex_unlock(&present_following_lock);
                return NULL;
            }
            
            event_present.event_id = e->event_id;
            event_present.start_time = e->start_time+time_offset;
            event_present.end_time = e->end_time+time_offset;
            
            if(e->title!=NULL)
            {
                if(event_present.title!=NULL)
                {
                    delete[] event_present.title;
                    event_present.title = NULL;
                }
                
                event_present.title = new char[strlen(e->title)+1];
                memset(event_present.title,0,(strlen(e->title)+1));
                
                if(event_present.title!=NULL)
                {
                    memcpy(event_present.title,e->title,strlen(e->title));
                }       
//                else
//                {
//                    pthread_mutex_unlock(&present_following_lock);
//                    return NULL;
//                }
                    
            }
            
            if(e->des!=NULL)
            {
                if(event_present.des!=NULL)
                {
                    delete[] event_present.des;
                    event_present.des = NULL;
                }
                
                event_present.des = new char[strlen(e->des)+1];
                memset(event_present.des,0,(strlen(e->des)+1));
                
                if(event_present.des!=NULL)
                {
                    memcpy(event_present.des,e->des,strlen(e->des));
                }       
//                else
//                {
//                    pthread_mutex_unlock(&present_following_lock);
//                    return NULL;
//                }
            }
            
            pthread_mutex_unlock(&present_following_lock);
            return &event_present;
            
        }
        else
        { 
            
            if(event_present.title!=NULL)
            {
                delete[] event_present.title;
                event_present.title = NULL;
            }

            if(event_present.des!=NULL)
            {
                delete[] event_present.des;
                event_present.des = NULL;
            }
            
            pthread_mutex_unlock(&present_following_lock); 
            return NULL;
        }
    }
    else
    {
        if(event_present.title!=NULL)
        {
            delete[] event_present.title;
            event_present.title = NULL;
        }

        if(event_present.des!=NULL)
        {
            delete[] event_present.des;
            event_present.des = NULL;
        }
        
        pthread_mutex_unlock(&present_following_lock); 
        return NULL;
    }
}

/**
 * @brief Funkcja pobiera z listy nastepna audycję
 * @param s
 * @return NULL jesli blad
 */
eit_event *dtv::DTV_Get_Next_Event(dvb_service *s)
{
    eit_event *e;
    
    if(s==NULL) return NULL;
    
    if(s->eit_present_following)
    {
        //ZABEZPIECZENIE:event mozemy pobrać tylko dla stacji która jest na tej częstotliwości!!! - tylko dla live
        if(s->frequency!=curr_freq) return NULL;
        
        //odczyt eventu mozliwy gdy dziala watek pobierania epg present/following
        if(dtv_epg_pf_downloading==false) return NULL;
    
        if(pthread_mutex_trylock(&present_following_lock)!=0) {
            //DTV_CORE_DEBUG("mutex locked!!!\n");
            return NULL;
        }
        
        if(s->epg_pf_events_list.get_list_count()>0)
        {
            e = s->epg_pf_events_list.get_from_list(1);
            
            if(e==NULL) 
            {
                if(event_present.title!=NULL)
                {
                    delete[] event_present.title;
                    event_present.title = NULL;
                }

                if(event_present.des!=NULL)
                {
                    delete[] event_present.des;
                    event_present.des = NULL;
                }
                
                pthread_mutex_unlock(&present_following_lock);
                return NULL;
            }
            
            event_following.event_id = e->event_id;
            event_following.start_time = e->start_time + time_offset;
            event_following.end_time = e->end_time + time_offset;
            
            if(e->title!=NULL)
            {
                if(event_following.title!=NULL)
                {
                    delete[] event_following.title;
                    event_following.title = NULL;
                }
                
                event_following.title = new char[strlen(e->title)+1];
                memset(event_following.title,0,(strlen(e->title)+1));
                
                if(event_following.title!=NULL)
                {
                    memcpy(event_following.title,e->title,strlen(e->title));
                }       
                
            }
            
            if(e->des!=NULL)
            {
                if(event_following.des!=NULL)
                {
                    delete[] event_following.des;
                    event_following.des = NULL;
                }
                
                event_following.des = new char[strlen(e->des)+1];
                memset(event_following.des,0,(strlen(e->des)+1));
                
                if(event_following.des!=NULL)
                {
                    memcpy(event_following.des,e->des,strlen(e->des));
                }       
                 
            }
            
            pthread_mutex_unlock(&present_following_lock);
            return &event_following;
        }
        else
        {
            if(event_present.title!=NULL)
            {
                delete[] event_present.title;
                event_present.title = NULL;
            }

            if(event_present.des!=NULL)
            {
                delete[] event_present.des;
                event_present.des = NULL;
            }
            
            //DTV_CORE_DEBUG("Lista epg_pf_events_list pusta!!!\n");  
            pthread_mutex_unlock(&present_following_lock); 
            return NULL;
        }
    }
    else
    {
        if(event_present.title!=NULL)
        {
            delete[] event_present.title;
            event_present.title = NULL;
        }

        if(event_present.des!=NULL)
        {
            delete[] event_present.des;
            event_present.des = NULL;
        }
        
        pthread_mutex_unlock(&present_following_lock); 
        return NULL;
    }
    
}

eit_event * dtv::DTV_Get_Current_Event_From_File(dvb_service *s)
{
dvb_epg_events_list *events_tmp;
eit_event       *e = NULL;
uint16_t        e_cnt = 0,i = 0;
time_t          current_time = 0;
time_t          start_time = 0;
time_t          end_time = 0;

    if(s==NULL) return NULL;
    
    if(time_is_set==false) 
    {
        DTV_CORE_DEBUG("TIME IS NOT SET!!!\n");
        return NULL;
    }
    
    if(s->eit_schedule==true)
    {
        if(current_service!=NULL)
        {
            if(current_service->frequency==s->frequency && current_service->transport_stream_id==s->transport_stream_id && dtv_epg_downloading==true)
            {
                printf("DTV_Get_Current_Event_From_File-current service busy: EPG dwonload\n");
                return NULL;
            }
        }
        
   //    if(pthread_mutex_trylock(&epg_schedule_lock)!=0) return NULL;
        
        current_time = time(NULL);
        
        events_tmp = DTV_EPG_Get_Channel_Events_List(s,0,e_cnt);

        if(events_tmp==NULL || e_cnt==0)
        {
    //      pthread_mutex_unlock(&epg_schedule_lock);
            return NULL;
        }

         for(i = 0;i<e_cnt;i++)
         {
             e = events_tmp->get_from_list(i);
             
             if(e!=NULL)
             {
                 start_time = e->start_time;
                 end_time = e->end_time;
                 start_time+=time_offset;
                 end_time+=(time_offset);
                         
                if(((current_time>=start_time) && (current_time<end_time)))
                {
                      e->start_time+=time_offset;
                      e->end_time+=time_offset;
                      
            //          pthread_mutex_unlock(&epg_schedule_lock); 
                      return e;
                }
             }
         }
        
   //     pthread_mutex_unlock(&epg_schedule_lock); 
    }
   
    return NULL;
}

eit_event * dtv::DTV_Get_Next_Event_From_File(dvb_service *s)
{
dvb_epg_events_list *events_tmp;
eit_event       *e = NULL;
uint16_t        e_cnt = 0,i = 0;
time_t          current_time = 0;
time_t          start_time = 0;
time_t          end_time = 0;

    if(s==NULL) return NULL;
    
    if(time_is_set==false) 
    {
        DTV_CORE_DEBUG("TIME IS NOT SET!!!\n");
        return NULL;
    }
    
    if(s->eit_schedule==true)
    {
        if(current_service!=NULL)
        {
            if(current_service->frequency==s->frequency && current_service->transport_stream_id==s->transport_stream_id && dtv_epg_downloading==true)
            {
                printf("DTV_Get_Current_Event_From_File-current service busy: EPG dwonload\n");
                return NULL;
            }
        }
        
   //    if(pthread_mutex_trylock(&epg_schedule_lock)!=0) return NULL;
        
        current_time = time(NULL);
        
        events_tmp = DTV_EPG_Get_Channel_Events_List(s,0,e_cnt);

        if(events_tmp==NULL || e_cnt==0)
        {
    //      pthread_mutex_unlock(&epg_schedule_lock);
            return NULL;
        }

         for(i = 0;i<e_cnt;i++)
         {
             e = events_tmp->get_from_list(i);
             
             if(e!=NULL)
             {
                 start_time = e->start_time;
                 end_time = e->end_time;
                 start_time+=time_offset;
                 end_time+=(time_offset);
                         
                if(((current_time>=start_time) && (current_time<end_time)))
                {
                    e = events_tmp->get_from_list(i+1);
                    e->start_time+=time_offset;
                    e->end_time+=time_offset;
                    return e;
                }
             }
         }
        
   //     pthread_mutex_unlock(&epg_schedule_lock); 
    }
   
    return NULL;
    
}

bool dtv::DTV_EPG_Is_Ready_For_Service(dvb_service *s)
{
    return epg.EpgIsReadyForService(s);
}

bool dtv::DTV_EPG_Is_Actuall_For_Service(dvb_service *s)
{
    return epg.EpgIsActuallForService(s);
}

