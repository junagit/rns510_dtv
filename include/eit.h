/** 
 * @file   eit.h
 * @author michalm
 *
 * @date   5 grudzień 2011, 07:23
 */

#ifndef EIT_H
#define	EIT_H

#include "service_list.h"
#include "multiplex.h"
#include "section.h"


#define EPG_DEBUG_T       0

#if EPG_DEBUG_T
        #define EPG_DEBUG(fmt, args...)   printf("[EPG][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define EPG_DEBUG(fmt, args...)
#endif

/**
 * @brief Klasa do pobierania informacji EPG z sieci,zapisu i odczytu tych informacji z pliku dla każdej stacji
 */
class eit{
    friend class dtv;
public:
    eit(){
        pthread_mutex_init(&crc_err_mutex,NULL);
        pthread_mutex_init(&crc_err_mutex2,NULL);
        add_info = 0;
        epg_pf_busy = false;
        epg_sched_busy = false;
        crc_err_cnt = 0;
        crc_err_cnt2 = 0;
        memset(&sbuf_epg_pf,0,sizeof(section_usr));
        memset(&sbuf_epg,0,sizeof(section_usr));
    };
    ~eit(){};
    
    
    void clear_err_cnt1()
    {
        pthread_mutex_lock(&crc_err_mutex);
        crc_err_cnt = 0;
        pthread_mutex_unlock(&crc_err_mutex);
    };
    
    void clear_err_cnt2()
    {
        pthread_mutex_lock(&crc_err_mutex2);
        crc_err_cnt2 = 0;
        pthread_mutex_unlock(&crc_err_mutex2);
    };
    
    int get_crc_err_cnt1(void)
    {
        int temp;
        pthread_mutex_lock(&crc_err_mutex);
        temp = crc_err_cnt;
        pthread_mutex_unlock(&crc_err_mutex);
        return temp;
    };
    
    int get_crc_err_cnt2(void)
    {
        int temp;
        pthread_mutex_lock(&crc_err_mutex2);
        temp = crc_err_cnt2;
        pthread_mutex_unlock(&crc_err_mutex2);
        return temp;
    };
    
    //void Save_EPG_data_for_each_service(dvb_service_list *list);
    void Save_EPG_data_for_each_service(dvb_service_list *list,uint32_t freq,uint32_t tsid);
    void clean_epg_download_res(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    void clean_epg_download_present_following_res(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    uint8_t Download_Epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    uint8_t Download_Epg_Present_Following(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    bool EpgIsReadyForService(dvb_service *service_temp);
    bool EpgIsActuallForService(dvb_service *service_temp);
    int time_to_epg_day(dvb_service * service_info);
    
private:
    void free_all_eit_sections_pf(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    void free_all_eit_sections_epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    void parse_eit_section(eit_section *section,dvb_service *service_temp,bool present_following);
    
    char * get_short_event_dsc(uint8_t *buf,char *add_inf);
    char * get_extended_event_dsc(uint8_t *buf);
    
    uint8_t parse_eit_sections_for_pf(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    uint8_t parse_eit_sections_for_epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    uint8_t save_service_2epg(uint32_t freq,uint16_t service_id,uint16_t ts_id,dvb_epg_events_list *service_event_list);
    uint8_t get_eit_for_all_service(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    uint8_t load_service_epg_from_file(uint32_t freq,uint16_t service_id,uint16_t ts_id,dvb_epg_events_list *service_event_list,uint16_t *days_p);
    uint8_t get_eit_present_following_for_services(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p);
    
    uint8_t add_info;           //!<Zmienna pomocnicza
    
    section_usr sbuf_epg;       //!<Zmienna wykorzystywana przy pobieraniu epg schedule
    section_usr sbuf_epg_pf;    //!<Zmienna wykorzystywana przy pobieraniu epg present/following
    bool        epg_pf_busy;
    bool        epg_sched_busy;
    pthread_mutex_t crc_err_mutex;
    pthread_mutex_t crc_err_mutex2;
    volatile int  crc_err_cnt;
    volatile int  crc_err_cnt2;
    
};
#endif	/* EIT_H */

