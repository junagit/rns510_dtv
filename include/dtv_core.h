/* 
 * File:   dtv_core.h
 * Author: michalm
 *
 * Created on 22 styczeń 2012, 10:21
 */

#ifndef DTV_CORE_H
#define	DTV_CORE_H

#include "multiplex.h"

#include "dvb_pat_data.h"
#include "dvb_sdt_data.h"
#include "misc_func.h"
#include "dvb_pmt_data.h"
#include "dvb_network_info.h"
#include "service_management.h"
#include "eit.h"
#include "epg_events_list.h"
#include "gst_core.h"
#include "dtv_time.h"

#define EPG_PF_DOWNLOAD_INTERVAL 4

#define RET_NO_SIGNAL           5
#define RET_BUSY                6
#define RET_PLAY_ERROR          6
#define RET_DTV_ERROR           1
#define RET_DTV_LIST_EMPTY      3


//typedef struct 
//{
//	int16_t			sync_byte;
//	int8_t 			transport_error_indicator;
//	int8_t 			layload_unit_start_indicator;
//	int8_t 			transport_priority;
//	int16_t			PID;
//	int8_t 			transport_scrambling_control;
//	int8_t 			adaptation_field_control;
//	int8_t 			continuity_counter;
//	int8_t 			adaptation_length;
//   	int16_t			buf_pos,buf_len;
//	unsigned char           *buffer;
//}packet_t;
//  

#define DTV_CORE_DEBUG_T       1
#define DTV_CORE_ERROR_DEBUG_T 1

#if DTV_CORE_DEBUG_T
        #define DTV_CORE_DEBUG(fmt, args...)   printf("[DTV_CORE][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define DTV_CORE_DEBUG(fmt, args...)
#endif

#if DTV_CORE_ERROR_DEBUG_T
        #define DTV_CORE_ERROR_DEBUG(fmt, args...)   printf("[DTV_CORE ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define DTV_CORE_ERROR_DEBUG(fmt, args...)
#endif

/**
 * @brief Głowna klasa do odtwarzania telewizji wykorzystywana przez uzytkownika
 */
class dtv{
    
    friend class eit;
    friend class QTvServiceManager;
    friend class play_ctrl;
public:
    dtv();
    ~dtv();

    bool                DTV_Init();
    bool                DTV_Close(void);
    dvb_service_list*   DTV_Scan(uint32_t freq, int band);
    bool                DTV_Save_Scan(bool f /* = 0 */);
    //uint8_t             DTV_Auto_Scan(uint32_t start_freq,uint32_t stop_freq);
    uint8_t             DTV_Service_Select(uint16_t service_pos_on_list);
    //uint8_t             DTV_Service_Select(uint16_t service_id,uint32_t freq);
    uint8_t             DTV_Service_Select(dvb_service *service_p);
    uint8_t             DTV_Select_Audio_Track(uint16_t audio_num);
    bool                DTV_Stop_Play();
    bool                DTV_Tv_Is_Playing(){ return is_playing; }; 
    bool                DTV_Force_Stop_Play();
    
    bool                DTV_need_resrtart()
    {
        return dvb_player->need_restart();
    };
    
    dvb_epg_events_list *DTV_EPG_Get_Channel_Events_List(dvb_service *s,uint8_t day,uint16_t & events_cnt);
    uint8_t             DTV_EPG_Start_Update();
    uint8_t             DTV_EPG_Present_Following_Start_Update();
    uint8_t             DTV_EPG_Update_Exit();
    uint8_t             DTV_EPG_Present_Following_Update_Exit();
    bool                DTV_EPG_Is_Ready_For_Service(dvb_service *s);
    bool                DTV_EPG_Is_Actuall_For_Service(dvb_service *s);
    eit_event           *DTV_Get_Current_Event(dvb_service *s);
    eit_event           *DTV_Get_Current_Event_From_File(dvb_service *s);
    eit_event           *DTV_Get_Next_Event(dvb_service *s);
    eit_event           *DTV_Get_Next_Event_From_File(dvb_service *s);
    
    bool                DTV_Start_Record(char *file);
    bool                DTV_Stop_Record();
    bool                DTV_Is_Playing(){ return is_playing; };
    bool                DTV_TimeIsSet(void) { return time_is_set; };
    void                DTV_TimeUpdate(void);
    void                DTV_TimeUpdateStop(void);
    bool                DTV_TimeIsUpdating(void);
    
    /**
     * @brief Funkcja wyciszająca audio.Wartości:true-mute false:normalna glosnosc;Działa tylko podczas odtwarzania
     * @param f
     */
    void                DTV_Mute(bool f){ if(is_playing) dvb_player->mute(f); };
    
    /**
     * @brief Funkcja ustawiajaca glosność audio. Zakres 0 - 10;Działa tylko podczas odtwarzania
     * @param val
     */
    void                DTV_Set_Volume(double val){ if(is_playing) dvb_player->volume_set(val);};
    
    /**
     * @brief Funkcja zwraca liczbę stacji zaladowanych z pliku
     * @return 
     */
    uint16_t            DTV_Get_Service_Cnt(void){ return service_management->get_services_count();};
    
    /**
     * @brief Funkcja zwraca aktualnie odtwarzaną stację
     * @return NULL jesli żadna stacja nie jest obecnie odtwarzana
     */
    dvb_service*        DTV_GetCurrent_Service(){ return current_service; };     
    /**
     * @brief Funkcja kasuje pojedyńczą stacje pod danym indeksem
     * @param service_index
     * @return 0 - OK,inne=błąd
     */
    uint8_t DTV_Service_Delete(uint16_t service_index){return service_management->Service_Delete(service_index);};
    
    
    
    uint8_t DTV_ServiceRemove(uint16_t service_index){return service_management->ServiceRemoveFromList(service_index);};
    
    
    
    /**
     * @brief Funkcja zwraca wskaźnik do stacji na liscie ze stacjami załadowanymi z pliku
     * @param index
     * @return Zwraca wskaźnik do stacji 
     */
    dvb_service * DTV_Index_To_Service(uint16_t index){ return service_management->DTV_Index_To_Service(index);};

    
    uint8_t DTV_Service_enable_disable(uint16_t service_index,bool enable){ return service_management->DTV_Service_enable_disable(service_index,enable);};
    
    uint8_t DTV_Service_Movedown(uint16_t service_index){ return service_management->DTV_Service_Movedown(service_index);};
    
    uint8_t DTV_Service_Moveup(uint16_t service_index){ return service_management->DTV_Service_Moveup(service_index);};
    
    dvb_service* DTV_Search_Service(uint16_t service_id, unsigned int f){ return service_management->DTV_Search_Service(service_id,f);};
    
    bool                service_have_subtitles(dvb_service *s,uint8_t & sub_cnt);
    void                dtv_core_monitor(void);
    
    void                DTV_ReloadChannleList(void)
    {
        service_management->Clean_Loaded_User_Service_Data();
        service_management->Init_services_data_for_user(false);
    };
    
    void DTV_DebugStart(void);
    void DTV_DebugStop(void);
    bool DTV_GetRecordState(void){ return is_recording; };
    
    time_t DTV_GetTimeOffset(void){
      return time_offset;  
    };
private:
    volatile bool       dtv_init;//!< Zmienna okreslająca czy TV jest zainicjalizowany
    volatile bool       dtv_scan_flag;//!< Zmienna okreslająca czy skanujemy
    volatile bool       dtv_epg_downloading;//!< Zmienna określająca czy epg jest pobierane w tej chwili   
    volatile bool       dtv_epg_pf_downloading;//!< Zmienna określająca czy epg jest pobierane w tej chwili
    volatile bool       freq_locked;
    volatile bool       time_is_set;
    volatile bool       time_is_updating;
    volatile bool       dibcom_thread_is_running;
    volatile bool       is_playing;
    volatile bool       is_recording;
    volatile bool       is_scanning;//flaga okreslająca czy weszlismy w skanowanie
    volatile bool       debug_running;
    uint32_t            curr_freq;
    time_t              time_offset;
    
    
    dvb_service_pid     *get_video_pid_from_list(dvb_service *sp);
    dvb_service_pid     *get_audio_pid_from_list(dvb_service *sp);
    dvb_service_pid     *get_subtitle_pid_from_list(dvb_service *sp);
    void                clear_multiplex_data(void);
    
    static void         *epg_thread(void *p);
    static void         *epg_pf_thread(void *p);
    static void         *time_update_thread(void *p);
//    static void         *dibcom_thread(void *p);
    static void         *record_thread(void *p);
    static void         *dtv_core_monitor_thread(void *p);
    uint8_t             init_epg_thread(void);
    uint8_t             init_epg_pf_thread(void);
    uint8_t             init_time_thread(void);
    
    uint8_t             exit_epg_thread(void);
    uint8_t             exit_epg_present_following_thread(void);
    uint8_t             exit_time_thread(void);
    
//    bool                init_dibcom_thread(void);
//    void                exit_dibcom_thread(void);
    
    bool                make_new_pmt(dvb_service *service,unsigned char *dst);
    bool                make_new_pat(dvb_service *service,unsigned char *dst);
    bool                ts_start_record(char * record_file);
    int8_t              ts_stop_record(char * record_file);
    int                 free_space(int file);
    
    unsigned char       *get_pat_pmt_copy(){ return pat_pmt;}
    
    bool                get_data_from_network(dvb_multiplex *p);
    
    dvb_multiplex       *my_multiplex;           //!< Zmienna(struktura) która jest używana podczas skanowania
    dvb_pat_data        *pat;
    dvb_sdt_data        *sdt;
    dvb_pmt_data        *pmt;
//    dvb_network_info    *nit;
    eit                 epg;
    dvb_service         *current_service;       //!< Wskaźnik na akualnie odtwarzany kanał
    service_manager     *service_management;
    gst_dvb             *dvb_player;
    dtv_time            network_time;
    
    dvb_service_pid     *current_video_pid;      //!< Zmienna w której jest przechowywany pid video obecnie odtwarzanego kanału
    dvb_service_pid     *current_audio_pid;      //!< Zmienna w której jest przechowywany pid audio obecnie odtwarzanego kanału
    
    pthread_t           epgThreadId;
    pthread_t           epg_pf_ThreadId;
    pthread_t           time_ThreadId;
//    pthread_t           dibcom_ThreadId;
    pthread_t           record_ThreadId;
    pthread_t           debug_ThreadId;
    
    eit_event           event_present;
    eit_event           event_following;
    
    pthread_mutex_t     present_following_lock;
    pthread_mutex_t     service_select_mutex;
    pthread_mutex_t     epg_schedule_lock;
    pthread_mutex_t     present_following_lock_file;
    pthread_mutex_t     stop_lock;
    
    int8_t              make_ts;
    uint64_t            record_total_time;
    uint64_t            record_start_time;
    FILE                *ts_f;
    FILE                *st_f;
    FILE                *me_f;
    int8_t              ts_stop_flag;
    uint8_t             pat_pmt[376];
//    char                record_file[256];
};


#endif	/* DTV_CORE_H */

