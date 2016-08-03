/** 
 * @file   eit_struct_define.h
 * @author michalm
 *
 * @date   20 grudzień 2011, 14:19
 */

#ifndef EIT_STRUCT_DEFINE_H
#define	EIT_STRUCT_DEFINE_H
#include <sys/time.h>
//epg
typedef struct {

    uint8_t *section_buffer;
    uint16_t section_len;

}__attribute__((packed))eit_section;


typedef struct{

    eit_section section[8];
    uint8_t     usage;

}__attribute__((packed))eit_segment_t;

typedef struct {

    eit_segment_t eit_segment[32];
    uint8_t     usage;

}__attribute__((packed))eit_table;

typedef struct {

    eit_table eit_tables[16];
    uint8_t table_cnt;

} __attribute__((packed))eit_shedule,eit_curr_next;


/**
 * @brief Struktura opisująca pojedyńczy event w EPG
 */
typedef struct
{
    uint16_t event_id; /**<Identyfikator audycji*/
    time_t start_time; /**< Czas startu audycji */
    time_t end_time;   /**< Czas końca audycji */
  //  time_t duration;

    char *title;        /**<Tytuł audycji*/
    char *des;          /**<Opis audycji*/
    
}__attribute__((packed)) eit_event ;

#endif	/* EIT_STRUCT_DEFINE_H */

