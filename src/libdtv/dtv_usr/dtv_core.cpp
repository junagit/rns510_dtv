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
#include <ncurses.h>

#include "dvbtcontrol.h"
#include "multiplex.h"
#include "service_management.h"
#include "eit.h"
#include "dtv_core.h"
#include "crc32.h"
#include "dvbtcontrol.h"
#include "gpio_ctrl.h"
#include "tsi.h"
#include "descriptors.h"
//#include "MemTrack.h"
#define esc 27
#define cls printf("%c[2J",esc)
#define pos(row,col) printf("%c[%d;%dH",esc,row,col)

/** 
 * @file   dtv_core.cpp
 * @author Michał Motyl
 * @date   29 czerwiec 2011, 13:44
 */

/*! 
 * \mainpage BIBLIOTEKA OBSŁUGI TELEWIZJI CYFROWEJ DVB-T DLA SSD1938
 * \section author_sec Autor
 * Michał Motyl JUNA P.P.H.U.
 * 
 * \section intro_sec Wprowadzenie
 *
 * Biblioteka ta została napisana do obsługi telewizji DVB-T na procesorze firmy SOLOMONT SYSTECH SSD1938
 * Do odtwarzania telwizji jest urzywana biblioteka gstreamer-a z dostaczonymi dekoderami MPEG2,H264,MPEG4 przez producenta 
 * procesora. Biblioteka jest używana do odtwarzania telewizji DVB-T(MPEG2 i H264), przeglądania EPG
 * 
 * \section install_sec Instalacja
 * Skompilowana bibliotekę należy zlinkować statycznie do głównej aplikacji odtwarzającej telewizję
 * 
 * \section todo_sec Do Zrobienia
 */

// Uwaga!!! 
// 1.Przy odtwarzaniu dzwieku AC3 jest zwiekszone użycie CPU niz w przypadku MP3!!!
// 2.Mogą wystepowac problemy przy rysowaniu GUI w szybkich strumieniach np. 12Mbit/s!!!


//volatile int curr_pmt = 0;
//using namespace MemTrack;
using namespace std;
dtv::dtv()
{
    dtv_init = false;
    dtv_scan_flag = false;
    dtv_epg_downloading = false;
    dtv_epg_pf_downloading = false;
    time_is_set = false;
    time_is_updating = false;
    time_offset = 0;
    is_playing = false;
    dibcom_thread_is_running = false;
    is_scanning = false;
    debug_running = false;
    curr_freq  = 0;
    current_service = NULL;
    dvb_player = NULL;
    my_multiplex = NULL;
    epgThreadId = 0;
    epg_pf_ThreadId = 0;
    debug_ThreadId = 0;
    is_recording = false;
//    nit = NULL;
    sdt = NULL;
    pat = NULL;
    pmt = NULL;
    
    current_audio_pid = NULL;
    current_video_pid = NULL;
        
    freq_locked = false;
    
    memset(&event_present,0,sizeof(eit_event));
    memset(&event_following,0,sizeof(eit_event));
    
    make_ts = 0;
    record_total_time = 0;
    record_start_time = 0;
    
    service_management = new service_manager;
    
}


dtv::~dtv()
{
    DTV_Close();
}


/**
 * @brief Funkcja incjalizuje sprzęt i biblioteke do odtwarzania TV
 * @param play_scan_flag - flaga okresla czy będziemy odtwarzali TV czy skanowali. TRUE - odtwarzanie,FALSE-skanowanie
 * @return 0 - OK,1 - telwizja juz jest zainicjalizowana, 2- bład alokacji pamięci dla zmiennej dvb_player,3 - błąd inicjalizacji gstreamera
 */
bool dtv::DTV_Init()
{
    int res;
    
    if(dtv_init==true){
        DTV_CORE_DEBUG("TV juz zainicjalizowane!!!\n");
        return false;
    }
    
    init_section_mutex();
    
    set_gpio_output(0,22,0);
    usleep(300000);
    set_gpio_value(0,22,1);
    
    res = HW_Initialized();
    
    if(res!=HW_NO_ERROR)
    {
        DTV_CORE_ERROR_DEBUG("HW_Initialized err\n");
        return false;
    }
    
    if(access("/dev/tsi", F_OK)!= 0)//2009-04-02, Maggie
    {
	if(system("insmod /usr/juna/modules/ssltsi.ko tsi_major=0")!=0) return false;

	system("major=`cat /proc/devices | grep SSL_TSI | cut -c1-3`;mknod /dev/tsi c $major 0");
    }

    res = tsi_initial_interface();
    
    if(res<0)
    {
        DTV_CORE_ERROR_DEBUG("TSI INIT ERROR!!!\n");
        return false;
    }
   

        if(dvb_player==NULL)
        {
            dvb_player = new gst_dvb;
            
            if(dvb_player==NULL) 
            {
                DTV_CORE_ERROR_DEBUG("Bląd alokacji pamieci na dvb_player.EXIT.\n");
                return false;
            }
        }
    
       gst_init (NULL, NULL);
           
       if (!g_thread_supported ()) g_thread_init (NULL);
        
        service_management->Init_services_data_for_user(false);//załaduj listę ze stacjami
        
        pthread_mutex_init (&present_following_lock,NULL);
        //pthread_mutex_init (&present_following_lock_file,NULL);
        pthread_mutex_init (&service_select_mutex,NULL);
        pthread_mutex_init (&epg_schedule_lock,NULL);
        pthread_mutex_init (&stop_lock,NULL);
       
    
    dtv_init = true;
    return true;
}


/**
 * @brief Funkcja zamykająca TV.Zatrzymuje odtwrzanie TV,konczy nagrywanie i kończy pobieranie EPG
 * @return 0 - OK
 */
bool dtv::DTV_Close(void)
{
    if(dtv_init==false) return 0;
    
    if(my_multiplex!=NULL)
    {
        if(!my_multiplex->service_list->is_empty()) 
        {
            clear_multiplex_data();
            delete my_multiplex->service_list;
        }
        
        delete my_multiplex;
    }
    
    
    if(dvb_player) 
    {
        delete dvb_player;
        dvb_player = NULL;
        gst_deinit();
    }
    
    if(is_playing==true) DTV_Stop_Play();
    
    destroy_section_mutex();
    
    pthread_mutex_unlock(&present_following_lock);
    pthread_mutex_destroy(&present_following_lock);
    
    //pthread_mutex_unlock(&present_following_lock_file);
    //pthread_mutex_destroy(&present_following_lock_file);
    
    pthread_mutex_unlock(&epg_schedule_lock);
    pthread_mutex_destroy(&epg_schedule_lock);
    
    pthread_mutex_unlock(&service_select_mutex);
    pthread_mutex_destroy(&service_select_mutex);
    
    pthread_mutex_unlock(&stop_lock);
    pthread_mutex_destroy(&stop_lock);
    
    DTV_EPG_Update_Exit();
    DTV_EPG_Present_Following_Update_Exit();
    
    if(time_is_updating==true) exit_time_thread();
   
    
    service_management->Clean_Loaded_User_Service_Data();
    delete service_management;
    
    service_management = NULL;
    current_audio_pid = NULL;
    current_video_pid = NULL;
    current_service = NULL;
     

    HW_Close();
    tsi_remove_pid_from_filter(-1);
    tsi_release_all_resource();

    dtv_init = false;

    
    return true;
}


/**
 * @brief Skanowanie manualne
 * @param freq
 * @return Zwraca wskaźnik do listy ze znalezionymi programami.
 */
dvb_service_list *dtv::DTV_Scan(uint32_t freq, int band)
{
    uint16_t i;
    dvb_service *s = NULL;
    bool ret;
    
    int res = 0;
    if(is_playing)
    {
        DTV_CORE_ERROR_DEBUG("Nie mozna skanować podczas odtwarzania\n");
        return NULL;
    }
    
    if(is_scanning) return NULL;
    /*Przed skanowaniem sprawdzamy czy wcześniej nie skanowaliśmy.Jeżeli tak to czyścimy zmienną my_multiplex przed 
     * kolejnym skanowaniem
     */
    
    if(my_multiplex==NULL) 
    {
        my_multiplex = new dvb_multiplex;
        if(my_multiplex!=NULL) my_multiplex->service_list = new dvb_service_list();
    }
    
    if(!my_multiplex->service_list->is_empty()){
        clear_multiplex_data();
    }
        
    is_scanning = true;
    tsi_remove_pid_from_filter(-1);
    //DTV_CORE_DEBUG("HW_Lock at:%d\n",(freq/1000));

    res = HW_Lock((freq/1000),band);
    
    if(res!=1)
    {
	DTV_CORE_DEBUG("HW_Lock err\n");
        is_scanning = false;
        return NULL;
    }
    
    
    my_multiplex->frequency = freq;
    
    ret = get_data_from_network((dvb_multiplex*)my_multiplex);

    if(ret==false)
    {
//        delete pmt;
//        pmt = NULL;
        
        if(!my_multiplex->service_list->is_empty()) clear_multiplex_data();
        
        is_scanning = false;
        return NULL;
    }
    
    if(pmt==NULL) pmt = new dvb_pmt_data;

    int res2 = pmt->find_pmt_for_services((dvb_multiplex*)my_multiplex);

    if(res2==4)
    {
//        delete sdt;
//        sdt = NULL;
//
//        delete pat;
//        pat = NULL;

        delete pmt;
        pmt = NULL;
        
        if(!my_multiplex->service_list->is_empty()) clear_multiplex_data();
        
        is_scanning = false;
        return NULL;
    }
    
    int j = my_multiplex->service_list->get_list_count();

    if(j>0)
    {
        DTV_CORE_DEBUG("Znalazlem %d stacji\n",j);
        
        for(i = 0;i<j;i++)
        {
           int idx_to_remove;
            s = my_multiplex->service_list->get_from_list(i);
//            printf("@@@@@@ s->service_name:%s  s->free_CA_mode:%d  s->scan_error:%d\n",s->service_name,s->free_CA_mode,s->scan_error);
            
            if(s->scan_error==true)
            {
                DTV_CORE_ERROR_DEBUG("SERVICE ERROR 1!!! REMOVING FROM LIST... %s\n",s->service_name);
                idx_to_remove = i;
                my_multiplex->service_list->remove_nth_node(idx_to_remove);
                i = 0;
                j = my_multiplex->service_list->get_list_count();
            }
            
//            if(s->free_CA_mode==true)
//            {
//                DTV_CORE_ERROR_DEBUG("SERVICE ERROR!!! REMOVING FROM LIST... %s\n",s->service_name);
//                idx_to_remove = i;
//                my_multiplex->service_list.remove_nth_node(idx_to_remove);
//                i = 0;
//                j = my_multiplex->service_list.get_list_count();
//            }
            
//            if( (Advanced_codec_HD_Television==s->service_type) || \
//                        (Advanced_codec_SD_Television==s->service_type) || \
//                        (Digital_Television_Service==s->service_type) || (MPEG2_HD_Television_Service==s->service_type))
//                    {
//                        
//                    }
//                    else
//                    {
//                        DTV_CORE_ERROR_DEBUG("SERVICE ERROR!!! REMOVING FROM LIST... %s  s->service_type:%d\n",s->service_name,s->service_type);    
//                        idx_to_remove = i;
//                        my_multiplex->service_list.remove_nth_node(idx_to_remove);
//                        i = 0;
//                        j = my_multiplex->service_list.get_list_count();        
//                    }
            
            //##############################################################################################
            if(s->service_pid_list.get_list_count()>0)
            {
                //sprawdzamy czy pobralismy pidy audio i video
                if(get_audio_pid_from_list(s)==NULL || get_video_pid_from_list(s)==NULL)
                {
                    DTV_CORE_ERROR_DEBUG("SERVICE ERROR 2!!! REMOVING FROM LIST... %s\n",s->service_name);
                    idx_to_remove = i;
                    my_multiplex->service_list->remove_nth_node(idx_to_remove);
                    i = 0;
                    j = my_multiplex->service_list->get_list_count();
                }
                
            }
            else
            {
                DTV_CORE_ERROR_DEBUG("SERVICE ERROR 3!!! REMOVING FROM LIST... %s\n",s->service_name);
                idx_to_remove = i;
                my_multiplex->service_list->remove_nth_node(idx_to_remove);
                i = 0;
                j = my_multiplex->service_list->get_list_count();
            }
            //##############################################################################################
            
         }

#if 0
        for(i = 0;i<j;i++)
        {
            s = my_multiplex.service_list.get_from_list(i);
            DTV_CORE_DEBUG("######################################################################################\n");
            DTV_CORE_DEBUG("service_name:%s  service_provider_name:%s service_id:%d\n",s->service_name,s->service_provider_name,s->service_id);
            DTV_CORE_DEBUG("transport_stream_id:%d  service type:%d free CA:%d\n",s->transport_stream_id,s->service_type,s->free_CA_mode);
            DTV_CORE_DEBUG("eit_present_following:%d  eit_schedule:%d pmt_pid:0x%04x  pcr_pid:0x%04x\n",s->eit_present_following,s->eit_schedule,s->pmt_pid,s->pcr_pid);
            DTV_CORE_DEBUG("s->service_pid_list.get_list_count():%d\n",s->service_pid_list.get_list_count());    
            
            for(pid_cnt = 0;pid_cnt<s->service_pid_list.get_list_count();pid_cnt++)
            {
                dvb_service_pid *pid_tmp = s->service_pid_list.get_from_list(pid_cnt);     
                DTV_CORE_DEBUG("PID %d:service_pid_type:%d pid:0x%04x\n",pid_cnt,pid_tmp->service_pid_type,pid_tmp->pid);
            }
        }
#endif
//        delete nit;
//        nit = NULL;

//        delete sdt;
//        sdt = NULL;
//
//        delete pat;
//        pat = NULL;

        delete pmt;
        pmt = NULL;

#if 0
        //nie zapisujemy stacji odrazu po znalezieniu.
        //Robimy to funkcją DTV_Save_Scan
        if(service_management.save_services2file(DVB_SERVICES_FILE,&my_multiplex)!=0)
        {
            DTV_CORE_DEBUG("Blad zapisu stacji do pliku!!!\n");
            if(!my_multiplex.service_list.is_empty())
            {
                    clear_multiplex_data();
            }

            return NULL;
        }
#endif 
        
        if(my_multiplex->service_list->is_empty()) 
        {
            is_scanning = false;
            return NULL;
        }
        
        //jezeli znaleźliśmy stacje to zwracamy wskaźnik na listę
        is_scanning = false;
        
        return my_multiplex->service_list; 
    }
    else
    {
//        delete nit;
//        nit = NULL;

//        delete sdt;
//        sdt = NULL;
//
//        delete pat;
//        pat = NULL;

        delete pmt;
        pmt = NULL;
        
        if(!my_multiplex->service_list->is_empty())
        {
                clear_multiplex_data();
        }
    
        is_scanning = false;
        return NULL;
    }
    
}


/**
 * @brief Funkcja zapisuje znalezione kanaly podczas skanowania
 * @return 
 */
bool dtv::DTV_Save_Scan(bool f /* = 0 */)
{
    //sprawdzamy czy cos znalezlismy
    //jesli nie to nie zapisujemy
    if(my_multiplex==NULL) return false;
    
    if(my_multiplex->service_list->is_empty()){
        return false;
    }
    
    
    if(service_management->save_services2file(DVB_SERVICES_FILE,my_multiplex)!=0)
        {
            DTV_CORE_DEBUG("Blad zapisu stacji do pliku!!!\n");
            if(!my_multiplex->service_list->is_empty())
            {
                    clear_multiplex_data();
            }
//            delete my_multiplex;
//            my_multiplex = NULL;
            return false;
        }
    
    
    //jesli flaga == true to wczytaj zapisane stacje do listy kanalow użytkownika
    if(f==true)
    {
        service_management->Clean_Loaded_User_Service_Data();
        service_management->Init_services_data_for_user(false);//załaduj listę ze stacjami
    }
    
    return true;
    
}

///**
// * @brief
// * @param start_freq
// * @param stop_freq
// * @return 
// */
//uint8_t dtv::DTV_Auto_Scan(uint32_t start_freq,uint32_t stop_freq)
//{
//    if(is_playing)
//    {
//        DTV_CORE_ERROR_DEBUG("Nie mozna skanować podczas odtwarzania\n");
//        return 1;
//    }
//    
//#ifdef PC_VERSION
//    
//#else
//
//#endif
//   
//    return 0;
//}



/**
 * @brief Funkcja wybiera i odtwarza program z listy wskazany przez parametr
 * @param service_pos_on_list
 * @return 0 - jesli OK, 1-telewizja niezainicjalizowana 2 - lista z kanałami pusta,3-blad pobrania wskaznika do kanalu z listy
 *              4-pid audio lub video pobrany z listy==NULL,5-blad dostrajania do czestotliwosci,6-inny proces wykonuje ta funkcję
 */
uint8_t dtv::DTV_Service_Select(uint16_t service_pos_on_list)
{
//struct timeval start,end;
//double diff;
//double s,e,sec,usec;
//bool update_time = false;
uint32_t f_temp = 0;
dvb_service *service_temp = NULL;

    if(pthread_mutex_trylock(&service_select_mutex)!=0) 
    {
        DTV_CORE_DEBUG("mutex locked!!!\n");
        return 6;
    }
   
    
    if(is_playing==true)
    {
       //DTV_CORE_ERROR_DEBUG("Telewizja jest odtwazana.========>>>> STOP \n"); 
       f_temp = current_service->frequency;
       DTV_Stop_Play();   
    }

    if(dtv_init==false) 
    {
        DTV_CORE_ERROR_DEBUG("Telewizja nie zainicjalizaowana.\n");
        is_playing = false;
        current_audio_pid = NULL;
        current_video_pid = NULL;
        current_service = NULL;
        freq_locked = false;
        pthread_mutex_unlock(&service_select_mutex);  
        return 1;
    }

    if(is_scanning) 
    {
        pthread_mutex_unlock(&service_select_mutex);  
        freq_locked = false;
        return 1;
    }

   // gettimeofday(&start,NULL);

    //jezeli pobieramy epg schedule to przed zmianą kanalu trzeba wyjsc z watku pobierania epg schedule
    DTV_EPG_Update_Exit();

    //jezeli pobieramy epg present/following to przed zmianą kanalu trzeba wyjsc z watku pobierania epg present/following
    DTV_EPG_Present_Following_Update_Exit();

    //jezeli pobieramy czas to wychodzimy z pobierania czasu
    if(time_is_updating==true) exit_time_thread();


//    if(service_management.user_service_list.get_list_count()==0)
//    {
//        DTV_CORE_ERROR_DEBUG("Brak stacji na liście...\n");
//        current_audio_pid = NULL;
//        current_video_pid = NULL;
//        current_service = NULL;
//        selecting = false;
//        is_playing = false;
//        pthread_mutex_unlock(&service_select_mutex);  
//        return 2;
//    }


    service_temp = service_management->DTV_Index_To_Service(service_pos_on_list);
    //DTV_CORE_ERROR_DEBUG("service_temp:%p service_management.get_services_count():%d service_pos_on_list:%d\n",service_temp,service_management.get_services_count(),service_pos_on_list);
    
    if(service_temp==NULL || service_management->get_services_count()==0) 
    {
        DTV_CORE_ERROR_DEBUG("Pusty wskaźnik pobrany z listy...\n");
        current_service = NULL;
        current_audio_pid = NULL;
        current_video_pid = NULL;
        is_playing = false;
        freq_locked = false;
        pthread_mutex_unlock(&service_select_mutex);  
        return RET_DTV_LIST_EMPTY;
    }
    
    current_service = service_temp;

    printf("####### WYBIERAM KANAL: %s ######\n",current_service->getServiceName ());
//    if(current_service->free_CA_mode==1)
//    {
//        DTV_CORE_DEBUG("#### KANAL KODOWANY ####\n");
//        selecting = false;
//        freq_locked = false;
//        pthread_mutex_unlock(&service_select_mutex);  
//        return 10;
//    }
    /*TODO Dopisać odtwarzanie wybranego programu za pomocą funkcji gstreamera*/
    //DTV_Select_Audio_Track(1);
    
    current_audio_pid = get_audio_pid_from_list(current_service);
    current_video_pid = get_video_pid_from_list(current_service);
    
//    for(int g = 0;g<current_service->service_pid_list.get_list_count();g++)
//    {
//        dvb_service_pid *pid = current_service->service_pid_list.get_from_list(g);
//        
//        printf("Pid %d %x: type:%d\n",pid->pid,pid->pid,pid->service_pid_type);
//    }
    
   
    if(current_audio_pid==NULL || current_video_pid==NULL)
    {
        current_audio_pid = NULL;
        current_video_pid = NULL;
        is_playing = false;
        freq_locked = false;
        DTV_CORE_ERROR_DEBUG("Pusty wskaźnik pobrany z listy PID\n");
        pthread_mutex_unlock(&service_select_mutex);  
        
        return 4; 
    }

    int res = HW_Lock((current_service->frequency/1000),8000);

    curr_freq = current_service->frequency;

    if(res!=1)
    {
        DTV_CORE_ERROR_DEBUG("HW_Lock err\n");
        //curr_freq = current_service->frequency;
        is_playing = false;
        freq_locked = false;
        pthread_mutex_unlock(&service_select_mutex);  
        return 5;
    }

    freq_locked = true;
            
    if(tsi_restart()!=TSI_ERR_NONE)
    {
        printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
        tsi_restart();
    }
            
    tsi_add_pid_to_filter(current_service->pmt_pid,PSI,TSI_MEDIA_DATA_BUFFINDEX);
    tsi_add_pid_to_filter(PID_PAT,PSI,TSI_MEDIA_DATA_BUFFINDEX);
    tsi_add_pid_to_filter(current_video_pid->pid,VIDEO,TSI_MEDIA_DATA_BUFFINDEX);
    tsi_add_pid_to_filter(current_audio_pid->pid,AUDIO,TSI_MEDIA_DATA_BUFFINDEX);

    //printf("pmt:0x%x vid:0x%x audio:0x%x\n",current_service->pmt_pid,current_video_pid->pid,current_audio_pid->pid);
    /*TODO Dopisać odtwarzanie wybranego programu za pomocą funkcji gstreamera*/
    if(dvb_player->GST_Play(current_video_pid, current_audio_pid, current_service->service_id,current_service)==false)
    {
        DTV_CORE_ERROR_DEBUG("Blad odtwarzania...\n");
        dvb_player->GST_Close();
        tsi_remove_pid_from_filter(-1);
        tsi_clear_buffer(1);
        is_playing = false;
        pthread_mutex_unlock(&service_select_mutex);  
        return 6;
    } 
    
    if(dvb_player->need_upadate_tv_list()==true) 
    {
        DTV_CORE_ERROR_DEBUG("TREZBA ZAKTUALIZOWAĆ PID-y audio/wideo\n");
        service_management->UpdateServices();
        //po zmianie musimy załadować stacje ponownie
        service_management->Clean_Loaded_User_Service_Data();
        service_management->Init_services_data_for_user(false);//załaduj listę ze stacjami
    }
    
    tsi_remove_pid_from_filter(current_service->pmt_pid);
    tsi_remove_pid_from_filter(PID_PAT);
      
    is_playing = true;
    pthread_mutex_unlock(&service_select_mutex);  
    
    return 0;
}


/**
 * @brief Funkcja pobierająca epg.Funkcja ta działa gdy odtwarzamy TV.
 * @return 0 - OK,1 - nie wybrano kanału,2 - epg jest pobierane,3 - bład inicjalizacji wątku EPG
 */
uint8_t dtv::DTV_EPG_Start_Update()
{
    if(current_service==NULL)
    {
        DTV_CORE_ERROR_DEBUG("Nie wybrano kanalu...\n");
        return 1;
    }
    
    if(dtv_epg_downloading==true) 
    {   
        //DTV_CORE_ERROR_DEBUG("Pobieranie EPG w trwa...\n");
        return 2;
    }
    
    
    if(!init_epg_thread()) dtv_epg_downloading=true;
    else
    {
        DTV_CORE_ERROR_DEBUG("Blad inicjalizacji wątku pobierania EPG\n");
        return 3;
    }
   
    return 0;
}


/**
 * @brief Funkcja pobierająca epg.Funkcja ta dziala gdy odtwarzamy TV.
 * @return 0 - OK,1 - nie wybrano kanału,2 - epg jest pobierane,3 - bład inicjalizacji wątku EPG
 */
uint8_t dtv::DTV_EPG_Present_Following_Start_Update()
{
    if(current_service==NULL)
    {
        DTV_CORE_ERROR_DEBUG("Nie wybrano kanalu...\n");
        return 1;
    }
    
    if(dtv_epg_pf_downloading==true) 
    {   
        //DTV_CORE_ERROR_DEBUG("Pobieranie EPG w trwa...\n");
        return 2;
    }
    
    
    if(!init_epg_pf_thread()) dtv_epg_pf_downloading=true;
    else
    {
        DTV_CORE_ERROR_DEBUG("Blad inicjalizacji wątku pobierania EPG present/following\n");
        return 3;
    }
   
    return 0;
}


/**
 * @brief Funkcja kończy działanie wątku pobierania EPG
 * @return Zwraca 0 - OK,w przeciwnym razie błąd
 */
uint8_t dtv::DTV_EPG_Update_Exit()
{
    uint8_t res;
    
    if(dtv_epg_downloading==false)
    {
        //DTV_CORE_DEBUG("Pobieranie epg schedule nie uruchomione.\n");
        return 1;
    }
    
    res = exit_epg_thread();
    if(res) 
    {
        DTV_CORE_ERROR_DEBUG("Funkcja exit_epg_thread zakonczyla dzialanie z bledem\n");
    }
    
    dtv_epg_downloading = false;
    return 0;
}

/**
 * @brief Funkcja kończy działanie wątbku pobierania EPG dla present/following
 * @return Zwraca 0 - OK,w przeciwnym razie błąd
 */
uint8_t dtv::DTV_EPG_Present_Following_Update_Exit()
{
    uint8_t res;
    
    if(dtv_epg_pf_downloading==true)
    {
        dtv_epg_pf_downloading = false;
        
        res = exit_epg_present_following_thread();

        if(res) DTV_CORE_ERROR_DEBUG("Funkcja exit_epg_thread zakonczyla dzialanie z bledem\n");  
    }
//    else
//    DTV_CORE_DEBUG("Pobieranie epg present/following nie uruchomione.\n");
    
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

    return 0;
}

/**
 * @brief  Funkcja wybiera do odtarzania ścieżke audio(w strumieniu może być kilka strumieni audio dla jedego kanału).Funkcja działa tylko przy odtwarzanej stacji       
 * @param  audio_num
 * @return 0 - OK 1 - NIE WYBRANA ŻADNA STACJA, 2 - nie znaleziono pidu na pozycji podanej w parametrze
 * @todo   Sprawdzić działanie tej funkcji
 */
uint8_t dtv::DTV_Select_Audio_Track(uint16_t audio_num)
{
    //TODO Sprawdzić działanie tej funkcji
    dvb_service_pid *tmp;
    uint8_t audio_pid_cnt = 0,list_cnt;
    
    if(current_service==NULL)
    {
        DTV_CORE_ERROR_DEBUG("Blad odtwarzania.Nie wybrano stacji do odwarzania\n");
        return 1;
    }
    
    list_cnt = current_service->service_pid_list.get_list_count();
    
    for(uint16_t i = 0;i<list_cnt;i++)
    {
        tmp = current_service->service_pid_list.get_from_list(i);
        
        if(tmp!=NULL)
        {
            //printf("tmp->service_pid_type:%d %d\n",tmp->service_pid_type,tmp->pid);
            if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_AUDIO_AAC || tmp->service_pid_type==STREAM_TYPE_AUDIO_AC3 || tmp->service_pid_type==STREAM_TYPE_AUDIO_EAC3)
            {
                if(audio_num==audio_pid_cnt)
                {
                    current_audio_pid = tmp;
                    printf("znalazlem:%d\n",audio_pid_cnt);
                    return 0;
                }
                
                audio_pid_cnt++;
            }
            
        }
    }
    
    return 1;
}


/**
 * @brief Funkcja zwraca wskaźnik do pierwszego na liście PID'u strumienia video.
 * @param sp - wskaźnik do struktóry opisującej pojedyńczą stację
 * @return Zwraca wskaźnik do pidu video dla stacji podanej w parametrze
 */
dvb_service_pid *dtv::get_video_pid_from_list(dvb_service *sp)
{
    uint16_t pids_cnt,i;
    dvb_service_pid *tmp;
    
    if(sp==NULL) return NULL;
    
    pids_cnt = sp->service_pid_list.get_list_count();
    
    //printf("get_video_pid_from_list: pids_cnt = %d\n",pids_cnt);
    
    if(!pids_cnt) return NULL;
    
    for(i = 0;i<pids_cnt;i++)
    {
        tmp = sp->service_pid_list.get_from_list(i);
        
        if(tmp)
        {
            if(tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_VIDEO_H264)
            {
                return tmp;
            }
        }
    }
    
    return NULL;
}


/**
 * @brief Funkcja zwraca wskaźnik do pierwszego na liście PID'u strumienia audio.
 * @param sp - wskaźnik do struktóry opisującej pojedyńczą stację
 * @return Zwraca dvb_service_pid - wskaźnik do struktury do pierwszego pidu audio dla stacji podanej w parametrze
 */
dvb_service_pid *dtv::get_audio_pid_from_list(dvb_service *sp)
{
    uint16_t pids_cnt,i;
    dvb_service_pid *tmp;
    dvb_service *sp2 = sp;
    
    if(sp2==NULL) return NULL;
    
    pids_cnt = sp2->service_pid_list.get_list_count();
    
    if(!pids_cnt) return NULL;
    
    for(i = 0;i<pids_cnt;i++)
    {
        tmp = sp2->service_pid_list.get_from_list(i);
        
        if(tmp)
        {
            if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_AUDIO_AAC || tmp->service_pid_type==STREAM_TYPE_AUDIO_AC3)
            {
                return tmp;
            }
        }
    }
    
    return NULL;
}


/**
 * @brief Funkcja zwraca pid napisów dla strumienia
 * @param sp
 * @return Jesli bład - NULL, w przeciwnym razie wskaźnik do pidu z napisami
 */
dvb_service_pid *dtv::get_subtitle_pid_from_list(dvb_service *sp)
{
    uint16_t pids_cnt,i;
    dvb_service_pid *tmp;
    
    if(sp==NULL) return NULL;
    
    pids_cnt = sp->service_pid_list.get_list_count();
    
    if(!pids_cnt) return NULL;
    
    for(i = 0;i<pids_cnt;i++)
    {
        tmp = sp->service_pid_list.get_from_list(i);
        
        if(tmp)
        {
            if(tmp->service_pid_type==STREAM_TYPE_DVB_SUBTITLES )return tmp;
        }
    }
    
    return NULL;
}


/**
 * @brief Funkcja czyści zmienną my_multiplex, wykorzystywaną podczas skanowania
 */
void dtv::clear_multiplex_data(void)
{
    if(my_multiplex!=NULL)
    {
        my_multiplex->frequency = 0;
        my_multiplex->network_id = 0;
        memset(my_multiplex->network_name,0,128);
        my_multiplex->original_network_id = 0;
        my_multiplex->transport_stream_id = 0;
        my_multiplex->service_list->destroy_list();
    }
}


/**
 * @brief Funkcja Uzytkownika do zatrzymywania odtwarzania kanału
 * @return 0 - STOP OK, 1 - bład 
 */
bool dtv::DTV_Stop_Play()
{
    if(is_playing==false) return false;

    if(dvb_player==NULL) return false;
    
    if(pthread_mutex_trylock(&stop_lock)!=0) return false;
    
    curr_freq  = 0;
    
    //jezeli pobieramy epg schedule to przed zmianą kanalu trzeba wyjsc z watku pobierania epg schedule
    DTV_EPG_Update_Exit();
 
    //jezeli pobieramy epg present/following to przed zmianą kanalu trzeba wyjsc z watku pobierania epg present/following
    DTV_EPG_Present_Following_Update_Exit();
    
    if(time_is_updating==true) exit_time_thread();

    bool res = dvb_player->Stop_Play();

    is_playing = false;
    
    tsi_remove_pid_from_filter(-1);

    pthread_mutex_unlock(&stop_lock);

    return res;
}

bool dtv::DTV_Force_Stop_Play()
{
    if(dvb_player==NULL) return false;
    
    if(is_playing==false) return false;
    
    is_playing = false;
    //if(pthread_mutex_trylock(&stop_lock)!=0) return false;
    
    curr_freq  = 0;
            
    //jezeli pobieramy epg schedule to przed zmianą kanalu trzeba wyjsc z watku pobierania epg schedule
    DTV_EPG_Update_Exit();

    //jezeli pobieramy epg present/following to przed zmianą kanalu trzeba wyjsc z watku pobierania epg present/following
    DTV_EPG_Present_Following_Update_Exit();
    
    if(time_is_updating==true) exit_time_thread();
    
    bool res = dvb_player->Stop_Play();
    
    tsi_remove_pid_from_filter(-1);
    //pthread_mutex_unlock(&stop_lock);
   
    return res;
}


/**
 * @brief Funkcja sprawdza czy dany kanal posiada napisy 
 * @param s
 * @param sub_cnt
 * @return Zwraca true jezeli posiada napisy.Do sub_cnt sa zapisane liczba napisow
 */
bool dtv::service_have_subtitles(dvb_service *s,uint8_t & sub_cnt)
{
    uint16_t i,pid_cnt;
    dvb_service_pid *pp;
    uint8_t cnt = 0;
    
    if(s==NULL)
    {
        DTV_CORE_ERROR_DEBUG("Pusty wskaznik w parametrze!!!\n");
        return false;
    }
    
    pid_cnt = s->service_pid_list.get_list_count();
    
    if(pid_cnt==0)
    {
        DTV_CORE_ERROR_DEBUG("Brak pid-ow na liscie...\n");
        return false;
    }
            
    for(i = 0;i<pid_cnt;i++)
    {
        pp = s->service_pid_list.get_from_list(i);
        
        if(pp==NULL) continue;
        
        if(pp->service_pid_type==STREAM_TYPE_DVB_SUBTITLES)cnt++;
    }
    
    if(cnt>0)
    {
        sub_cnt = cnt;
        return true;
    }
    
    sub_cnt = 0;
    return false;
}

void dtv::DTV_DebugStart(void)
{
    int res;

    if(debug_running==true) return;
    
    res = pthread_create(&debug_ThreadId, NULL, &dtv::dtv_core_monitor_thread,this);
        
    if(res<0)
    {
        DTV_CORE_ERROR_DEBUG("Bląd tworzenia wątku '%s'.\n", strerror(errno));
        return;
    }
    
    debug_running = true;
    return;
}

void dtv::DTV_DebugStop(void)
{
    if(debug_running==false) return;
    
    pthread_cancel(debug_ThreadId);
    pthread_join(debug_ThreadId,NULL);
    debug_running = false;
}

void dtv::dtv_core_monitor(void)
{
    
//    cls;
//    printf("dtv_epg_downloading:        %d\n",dtv_epg_downloading);
//    printf("dtv_epg_pf_downloading:     %d\n",dtv_epg_pf_downloading);
//    printf("freq_locked:                %d\n",freq_locked);
//    printf("time_is_set:                %d\n",time_is_set);
//    printf("time_is_updating:           %d\n",time_is_updating);
//    printf("dibcom_thread_is_running:   %d\n",dibcom_thread_is_running);
//    printf("is_playing:                 %d\n",is_playing);
//    printf("is_recording:               %d\n",is_recording);
//    printf("curr_freq:                  %d\n",curr_freq);
//    pos(0,0);
    
}

void *dtv::dtv_core_monitor_thread(void *p)
{
    dtv *parent_p;
    parent_p = ((dtv*)p);

    while(1)
    {
        pthread_testcancel();
        parent_p->dtv_core_monitor();
        usleep(500000);
    }
    
    return NULL;
}

bool dtv::get_data_from_network(dvb_multiplex *p)
{
    stream_t my_stream_pat;
    section_usr sbuf_pat;
    
    stream_t my_stream_sdt;
    section_usr sbuf_sdt;
    
    descriptor_parser dp;
    uint8_t parse_result = 0;
    bool find_pat = false;
    bool find_sdt = false;
    int res = -1;
    bool ret = true;
    int pat_find_retry_cnt = 3;
    
    struct timeval start_time,end_time;// for time out. TSI will parse data for about psi_timeout
    int time_wait = 0;
    
    memset(&sbuf_pat,0,sizeof(section_usr));
    memset(&sbuf_sdt,0,sizeof(section_usr));
  
    my_stream_pat.buffer = sbuf_pat.databuf;
    my_stream_pat.read_len = 188;
    my_stream_pat.ops = 0;
    my_stream_pat.PID_type = PSI;
    my_stream_pat.buff_idx = TSI_CTRL_BUFFINDEX;
    my_stream_pat.time_out = 100;//set the timeout to 2 seconds
    my_stream_pat.type = 1;
    
    my_stream_sdt.buffer = sbuf_sdt.databuf;
    my_stream_sdt.read_len = 188;
    my_stream_sdt.ops = 0;
    my_stream_sdt.PID_type = PSI;
    my_stream_sdt.buff_idx = TSI_CTRL_BUFFINDEX;
    my_stream_sdt.time_out = 100;//set the timeout to 2 seconds
    my_stream_sdt.type = 1;


    tsi_add_pid_to_filter(PID_SDT,PSI,my_stream_sdt.buff_idx);
    tsi_add_pid_to_filter(PID_PAT,PSI,my_stream_pat.buff_idx);
    
    gettimeofday(&start_time,NULL);// start to count time.
    while(1)
    {
        gettimeofday(&end_time,NULL);// test time out.
	//timeout for 3 seconds, 2 for PAT & SDT, 1s for operation delay
        time_wait = (int)((end_time.tv_sec - start_time.tv_sec )*1000+(end_time.tv_usec-start_time.tv_usec)/1000);
        if(time_wait>=6000)
        {
            if(sbuf_pat.section_bufs!=NULL) {
                free(sbuf_pat.section_bufs);
                sbuf_pat.section_bufs = NULL;
            }
    
            if(sbuf_sdt.section_bufs!=NULL) {
                free(sbuf_sdt.section_bufs);
                sbuf_sdt.section_bufs = NULL;
            }

            tsi_remove_pid_from_filter(PID_SDT);
            tsi_remove_pid_from_filter(PID_PAT);
            
            return false;
        }
                
        if(find_pat==false)
        {
          res = find_section_live2(PID_PAT,&sbuf_pat,&my_stream_pat);
            if(res ==0) 
            {
                
              if(_dvb_crc32(section_buf_data(sbuf_pat.section_bufs),sbuf_pat.section_bufs->len))
              {
                printf("pat crc error!!!!\n");
                fflush(stdout);
                if(pat_find_retry_cnt--==0)
                {
                    ret = false;
                    break;  
                }
                sbuf_pat.continuities = 0;
                sbuf_pat.i = 0;
                sbuf_pat.section_status = 0;
                sbuf_pat.sz = 0;
                memset(&sbuf_pat.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_pat.databuf,0,DATA_BUF_SIZE);
                sbuf_pat.used = 0;

                if(sbuf_pat.section_bufs!=NULL) section_buf_reset(sbuf_pat.section_bufs);
                
              }
            else
            {
              find_pat = true;
              tsi_remove_pid_from_filter(PID_PAT);
            }
          }
        }
        
        if(find_sdt==false)
        {
         res = find_section_live2(PID_SDT,&sbuf_sdt,&my_stream_sdt);

          if(res==0)
          {
              find_sdt = true;

              tsi_remove_pid_from_filter(PID_SDT);
          }
        }
        
        if(find_pat==true && find_sdt==true)
        {
         //   printf("%s: %d sbuf.section_bufs->len:%d\n",__FUNCTION__,__LINE__,sbuf_sdt.section_bufs->len);
//            if(_dvb_crc32(section_buf_data(sbuf_sdt.section_bufs),sbuf_sdt.section_bufs->len))
//            {
//              printf("pat crc error!!!!\n");
//              ret = false;
//              break;  
//            }

            parse_result = dp.parse_section(section_buf_data(sbuf_sdt.section_bufs),sbuf_sdt.section_bufs->len, PID_SDT, DATA_TYPE_DVB,p);
            
            if(parse_result!=0)
            {
                ret = false;
                break;
            }
            
         //   printf("%s: %d sbuf.section_bufs->len:%d\n",__FUNCTION__,__LINE__,sbuf_pat.section_bufs->len);
//            if(_dvb_crc32(section_buf_data(sbuf_pat.section_bufs),sbuf_pat.section_bufs->len))
//            {
//                ret = false;
//                break;
//            }

            parse_result = dp.parse_section(section_buf_data(sbuf_pat.section_bufs),sbuf_pat.section_bufs->len, PID_PAT, DATA_TYPE_DVB,p);
            
            if(parse_result!=0)
            {
                ret = false;
                break;
            }
            
            break;
        }
    }
    
    if(sbuf_pat.section_bufs!=NULL) {
        free(sbuf_pat.section_bufs);
        sbuf_pat.section_bufs = NULL;
    }
    
    if(sbuf_sdt.section_bufs!=NULL) {
        free(sbuf_sdt.section_bufs);
        sbuf_sdt.section_bufs = NULL;
    }
    
    tsi_remove_pid_from_filter(PID_SDT);
    tsi_remove_pid_from_filter(PID_PAT);
    
    return ret;
}