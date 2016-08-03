/** 
 * @file   dvb_srvice_pid_list.cpp
 * @author Michał Motyl
 *
 * @date   21 grudzień 2011, 07:27
 */
#include <stdio.h>
#include <string.h>
#include "dvb_srvice_pid_list.h"

dvb_srvice_pid_list::dvb_srvice_pid_list() : list_head(NULL),list_cnt(0)
{
//    list_head = NULL;
//    list_cnt = 0;
}



dvb_srvice_pid_list::~dvb_srvice_pid_list() {
    destroy_list();
}

/**
 * 
 * @param data
 * @return 
 */
uint8_t dvb_srvice_pid_list::add_to_list(dvb_service_pid *data)
{
 dvb_service_pid_list_node_t *temp1 = NULL, *temp2 = NULL;

  temp1 = new dvb_service_pid_list_node_t;
  temp1->data=data;

  // Copying the Head location into another node.
  temp2=list_head;

  if(list_head == NULL)
  {
     // If List is empty we create First Node.
     list_head=temp1;
    // list_tail = list_head;
     list_head->next=NULL;
  }
  else
  {
     // Traverse down to end of the list.
      
     while(temp2->next != NULL)
     temp2=temp2->next;

     // Append at the end of the list.
     temp1->next=NULL;
     temp2->next=temp1;
     
   //  list_tail = temp1;
  }

  list_cnt++;
  
 return 0;
}



/**
 * @brief       Funkcja zwraca wskaznik na element listy znajdujący się pod daną pozycją
 * @param       pos
 * @return      
 */
dvb_service_pid * dvb_srvice_pid_list::get_from_list(uint16_t pos)
{
    dvb_service_pid_list_node_t *temp = NULL;
    int i = 0;
    temp = list_head;

    if(pos>(list_cnt-1)) return NULL;// sprawdzamy czy nie jestesmy poza listą

    while(temp!=NULL)
      {
        if(i==pos){

            return temp->data;
          }
        temp = temp->next;
        i++;
      }

    return NULL;
}


/**
 * 
 * @return Funkcja zwaraca liczbe elementów na liście;
 */
uint32_t dvb_srvice_pid_list::get_list_count(void)
{
#if 0
    list_node_t *temp = NULL;
    uint16_t i = 0;
    temp = list_head;

    while(temp!=NULL)
      {
	i++;
	temp = temp->next;
      }

    return i;
#else
    return list_cnt;
    
#endif
}


///**
// * 
// * @return Funkcja zwraca maxymalny index do którego mozemy się odwolać;
// *         Jeżeli lista jest pusta to zwarcamy -1     
// */
//int32_t dvb_srvice_pid_list::get_list_max_idx(void)
//{
//    if(list_cnt==0) return -1;
//    
//    return (list_cnt-1);
//    
//}

/**
 * @brief Funkcja usuwa z listy wszystkie elementy jak i dane z pod wskaźnika data;
 */
void dvb_srvice_pid_list::destroy_list(void)
{
   dvb_service_pid_list_node_t *temp = NULL,*temp1;
   temp = list_head;


  while(temp!=NULL)
    {
      temp1 = temp->next;

      if(temp->data!=NULL)
      {
          delete temp->data;
          temp->data = NULL;
      }
      
      delete temp;
      
      temp = temp1;
    }
   
 list_cnt = 0;
 list_head = NULL;
}


/**
 * @brief       Usuwa element z listy z pod określonej pozycji
 * @param pos
 */
//void dvb_srvice_pid_list::remove_nth_node(uint16_t pos)
//{
//    dvb_service_pid_list_node_t *temp = NULL;
//    dvb_service_pid_list_node_t *prev_node = NULL;
//    
//    int i = 0;
//    
//    
//    if(list_head==NULL) return;
//    
//    if(pos>list_cnt) return;
//     
//    temp = list_head;
//    
//    //jesli mamy jeden element na liscie i chcemy go usunąć
//    if(pos==0)
//    {
//        list_head = temp->next;
//        
//        if(temp->data!=NULL)
//            {
//                delete temp->data;
//                temp->data = NULL;
//            }
//            
//            if(temp!=NULL) {
//                delete temp;
//                temp = NULL;
//            }
//            
//            list_cnt--;
//            return;
//    }
//
//    
//    while(temp!=NULL)
//      {
//        if(i==pos){
//            
//
//            prev_node->next = temp->next;
//            
//            if(temp->data!=NULL)
//            {
//                delete temp->data;
//                temp->data = NULL;
//            }
//            
//            if(temp!=NULL) {
//                delete temp;
//                temp = NULL;
//            }
//            
//            list_cnt--;
//            return;
//            
//          }
//        prev_node = temp;
//        temp = temp->next;
//        
//        i++;
//      } 
//}


//uint8_t dvb_srvice_pid_list::get_audio_pid_cnt(void)
//{
//    dvb_service_pid *tmp;
//    uint8_t audio_pid_cnt = 0;
//    
//    for(uint16_t i = 0;i<list_cnt;i++)
//    {
//        tmp = get_from_list(i);
//        
//        if(tmp!=NULL){
//            
//            if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_AUDIO_AAC)
//            {
//                audio_pid_cnt++;
//            }
//            
//        }
//    }
//    
//    return audio_pid_cnt;
//}