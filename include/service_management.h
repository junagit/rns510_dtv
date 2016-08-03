/** 
 * @file   service_management.h
 * @author michalm
 *
 * @date   30 czerwiec 2011, 11:57
 */

#include "struct_define.h"
#include "service_list.h"
#include "epg_events_list.h"
#include "multiplex.h"

#define MPEG2_HD_Television_Service                     (0x11)
#define Advanced_codec_HD_Television                    (0x19)
#define Advanced_codec_SD_Television                    (0x16)
#define Digital_Television_Service                      (0x01)
#define Digital_Radio_Sound_Service                     (0x02)
#define Advanced_Codec_Digital_Radio_Sound_Service      (0x0a)

#ifndef SERVICE_MANAGEMENT_H
#define	SERVICE_MANAGEMENT_H

#define DVB_SERVICES_FILE       "/media/localdata/dvb_services.dat"

#define SERVICE_MANAGEMENT_DEBUG_T       1
#if SERVICE_MANAGEMENT_DEBUG_T
        #define SERVICE_MANAGEMENT_DEBUG(fmt, args...)   printf("[SERVICE_MANAGEMENT][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define SERVICE_MANAGEMENT_DEBUG(fmt, args...)
#endif


/**
 * @brief Klasa która zażądza stacjami DVB
 */
class service_manager{
    friend class dtv;
    friend class play_ctrl;
public:
    service_manager();
    ~service_manager();
    
    uint8_t             UpdateServices(void)
    {
      return services_update(DVB_SERVICES_FILE,user_service_list);  
    };
    uint8_t             Init_services_data_for_user(bool skip_visible_check /* = 0 */);
    void                Clean_Loaded_User_Service_Data(void);
    uint8_t             Service_Delete(uint16_t service_index);
    uint8_t             ServiceRemoveFromList(uint16_t service_index);
    dvb_service         *DTV_Index_To_Service(uint16_t index);
    uint8_t             DTV_Service_enable_disable(uint16_t service_index,bool enable);
    uint8_t             DTV_Service_Movedown(uint16_t service_index);
    uint8_t             DTV_Service_Moveup(uint16_t service_index);
    dvb_service         *DTV_Search_Service(uint16_t service_id, unsigned int f);
    uint16_t            get_services_count();
    
    uint8_t             services_update(const char *file,dvb_service_list *p);
    uint8_t             save_services2file(const char *file,dvb_multiplex *p);
private:
    uint8_t             load_services_from_file(const char *file,dvb_service_list *current_list,bool skip_visible_check);
    dvb_service_list    *user_service_list;
    
};

#endif	/* SERVICE_MANAGEMENT_H */

