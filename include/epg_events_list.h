/** 
 * @file   epg_events_list.h
 * @author michalm
 *
 * @date   5 grudzień 2011, 13:20
 */
#include <stdint.h>
#include "eit_struct_define.h"

#ifndef EPG_EVENTS_LIST_H
#define	EPG_EVENTS_LIST_H

/**
 * @brief Klasa listy z eventami EPG
 */
class dvb_epg_events_list {
public:
    dvb_epg_events_list();
    ~dvb_epg_events_list();
    
    uint8_t     add_to_list(eit_event *data);
    eit_event *   get_from_list(uint16_t pos);
    uint32_t    get_list_count(void);
//    int32_t     get_list_max_idx(void);
    void        destroy_list(void);
//    void        remove_nth_node(uint16_t); 
private:
    typedef struct eit_event_list_node{

  eit_event                  *data;
  struct eit_event_list_node      *next;
  //struct eit_event_list_node      *prev;

}__attribute__((packed)) eit_event_list_node_t;

    eit_event_list_node_t *list_head;
    eit_event_list_node_t *list_end;
    uint32_t    list_cnt;
    pthread_mutex_t lock;
};

#endif	/* EPG_EVENTS_LIST_H */

