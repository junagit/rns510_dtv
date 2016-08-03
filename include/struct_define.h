/** 
 * @file   struct_define.h
 * @author michalm
 *
 * @date   27 czerwiec 2011, 12:29
 */

#include <stdint.h>
#include <string.h>
#include <string>

#include "service_list.h"
#include "eit_struct_define.h"
#include "epg_events_list.h"
#include "dvb_srvice_pid_list.h"

using namespace std;

#ifndef STRUCT_DEFINE_H
#define	STRUCT_DEFINE_H

class eit;
class dvb_service_list;
class dtv;
class service_manager;
class descriptor_parser;
/**
 * @brief Klasa opisująca pojedyńczy kanał stacji DVB
 */
class dvb_service
{
    friend class eit;
    friend class dvb_service_list;
    friend class dtv;
    friend class service_manager;
    friend class descriptor_parser;
    friend class dvb_pmt_data;
public:
    dvb_service()// : epg(NULL),epg_pf(NULL),visible(0),service_type(0),eit_schedule(0),eit_present_following(0),free_CA_mode(0),service_id(0),pmt_pid(0),pcr_pid(0),transport_stream_id(0),frequency(0),scan_error(0)
    {
        epg = NULL;
        epg_pf = NULL;
//        
        visible = 0;
        memset(network_name,0,128);
        memset(service_name,0,128);
        memset(service_provider_name,0,128);
        service_type = 0;
        eit_schedule = 0;
        eit_present_following = 0;
        free_CA_mode = 0;
        service_id = 0;
        pmt_pid = 0;
        pcr_pid = 0;
        transport_stream_id = 0;
        frequency = 0;
        scan_error = false;
    };
    
    ~dvb_service()
    {
        service_pid_list.destroy_list();
        epg_events_list.destroy_list();
        epg_temp_events_list.destroy_list();
        epg_pf_events_list.destroy_list();
    };
    
    char *getNetworkName()      {return (char*)network_name;};
    char *getServiceName()      {return (char*)service_name;};
    char *getServiceProviderName(){return (char*)service_provider_name;};
    uint8_t getServiceType()    {return service_type;};
    bool haveEPG()              {return (bool)eit_schedule;};
    bool haveEPG_CurrentNext()  {return (bool)eit_present_following;};
    uint16_t getServiceID()     {return service_id;};
    uint16_t getPMTpid()        {return pmt_pid;};
    uint16_t getPCRpid()        {return pcr_pid;};
    uint16_t getTsId()          {return transport_stream_id;};
    uint32_t getServiceFreq()   {return frequency;};
    dvb_srvice_pid_list *getServicePidList(){ return &service_pid_list;};             
    bool isVisible()            {return visible;};
     
//###################################################################################
private:
    uint8_t                     visible;                                //!<czy stacja ma być widoczna dla użytkownika
    uint8_t                     network_name[128];                      //!<nazwa sieci
    uint8_t                     service_name[128];                      //!<nazwa stacji
    uint8_t                     service_provider_name[128];             //!<nazwa dostarczyciela usługi
    uint8_t                     service_type;                           //!<typ:radio,telewizja
    uint8_t                     eit_schedule;                           //!<czy posiada "duże" epg         
    uint8_t                     eit_present_following;                  //!<czy posiada "małe" epg        
    uint8_t                     free_CA_mode;
    uint16_t                    service_id;                             //!<id kanału        
    uint16_t                    pmt_pid;                                //!<pid pmt        
    uint16_t                    pcr_pid;                                //!<pid pcr
    uint16_t                    transport_stream_id;                    //!<id muliplexu
    uint32_t                    frequency;                              //!<częstotliwość(wypełniane podczas skanowania)  
    bool                        scan_error;
    eit_shedule                 *epg;                                   //!<epg na 8 dni,używane tylko podczas pobierania epg;        
    eit_curr_next               *epg_pf;                                //!<epg teraz/następny,używane tylko podczas pobierania epg
    dvb_srvice_pid_list         service_pid_list;                       //!<lista pidów dla tego kanału
    dvb_epg_events_list         epg_events_list;                        //!<lista uzywana przy pobieraniu epg schedule
    dvb_epg_events_list         epg_pf_events_list;                     //!<lista z eventami dla present/following
    dvb_epg_events_list         epg_temp_events_list;                   //!<lista z eventami,wypełniana przy wywołaniu funkcji DTV_EPG_Get_Channel_Events_List
    
};


/**
 * @brief Struktura opisująca czas dla DVB
 */
typedef struct 
{
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
}__attribute__((packed)) dvb_ts_time;


typedef struct 
{
    uint32_t epg_events_in_day;
    uint32_t epg_day_size_in_bytes;
    
}__attribute__((packed)) epg_info_entry;

#endif	/* STRUCT_DEFINE_H */

