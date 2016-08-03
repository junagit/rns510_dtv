/** 
 * @file   service_list.h
 * @author michalm
 *
 * @date   5 grudzień 2011, 13:11
 */

#ifndef SERVICE_LIST_H
#define	SERVICE_LIST_H

#include "struct_define.h"
#include <qlist.h>
#include<pthread.h>
using namespace std;



/**
 * @brief Klasa listy ze stacjami DVB
 */
class dvb_service_list {
public:
    dvb_service_list();
    ~dvb_service_list();
    
    uint8_t     add_to_list(dvb_service *data);
    dvb_service *get_from_list(uint16_t pos);
    uint32_t    get_list_count(void);
    void        destroy_list(void);
    void        remove_nth_node(uint16_t);
    uint8_t     move_down(uint16_t index);
    uint8_t     move_up(uint16_t index);
    bool        is_empty(void){if(list_cnt>0) return false; return true;};
private:
typedef struct service_list_node{

  dvb_service                  *data;
  struct service_list_node      *next;

}service_list_node_t;

    volatile service_list_node_t *list_head;
    volatile service_list_node_t *list_end;
    volatile uint32_t    list_cnt;
    pthread_mutex_t lock;
};

#endif	/* SERVICE_LIST_H */

