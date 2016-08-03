/** 
 * @file   epg_events_list.cpp
 * @author Michał Motyl
 *
 * @date   5 grudzień 2011, 13:20
 */
#include <string.h>
#include <stdio.h>

#include "struct_define.h"
#include "epg_events_list.h"

dvb_epg_events_list::dvb_epg_events_list() 
{
    list_head = NULL;
    list_end = NULL;
    list_cnt = 0;
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");

    }
}



dvb_epg_events_list::~dvb_epg_events_list() 
{
    destroy_list();
    pthread_mutex_destroy(&lock);
}


/**
 * @brief Funkcja dodaje do listy element
 * @param data
 * @return 
 */
uint8_t dvb_epg_events_list::add_to_list(eit_event *data)
{
 if(data==NULL) return 1;
 
  //pthread_mutex_lock(&lock);
  
  eit_event_list_node_t *temp1 = NULL, *temp2 = NULL;
 
  temp1 = new eit_event_list_node_t;
  
  temp1->data=data;


  if(list_head == NULL)
  {
     list_head=temp1;
     list_head->next=NULL;
     list_end = list_head;
  }
  else
  {
//     #warning Przetestować działanie listy po dodaniu wskaźnika końca listy!!!
     //temp2=list_head;
     temp2 = list_end;
      
     while(temp2->next != NULL)
     temp2=temp2->next;

     // Append at the end of the list.
     temp1->next=NULL;
     temp2->next=temp1;
     list_end = temp1;
  }

  list_cnt++;
  
  //pthread_mutex_unlock(&lock);
  
 return 0;
}



/**
 * @brief       Funkcja zwraca wskaznik na element listy znajdujący się pod daną pozycją
 * @param       pos
 * @return      
 */
eit_event * dvb_epg_events_list::get_from_list(uint16_t pos)
{
   // pthread_mutex_lock(&lock);
    
    eit_event_list_node_t *temp = NULL;
    int i = 0;
    
    temp = list_head;

    if(pos>(list_cnt-1)) 
    {
     //   pthread_mutex_unlock(&lock);
        return NULL;// sprawdzamy czy nie jestesmy poza listą
    }

    while(temp!=NULL)
      {
        if(i==pos) 
        {
     //       pthread_mutex_unlock(&lock);
            return temp->data;
        }
        
        temp = temp->next;
        i++;
      }

   // pthread_mutex_unlock(&lock);
    return NULL;
}


/**
 * 
 * @return Funkcja zwaraca liczbe elementów na liście;
 */
uint32_t dvb_epg_events_list::get_list_count(void)
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


/**
 * 
 * @return Funkcja zwraca maxymalny index do którego mozemy się odwolać;
 *         Jeżeli lista jest pusta to zwarcamy -1     
 */
//int32_t dvb_epg_events_list::get_list_max_idx(void)
//{
//    if(list_cnt==0) return -1;
//    
//    return (list_cnt-1);
//}

/**
 * @brief Funkcja usuwa z listy wszystkie elementy jak i dane z pod wskaźnika data;
 */
void dvb_epg_events_list::destroy_list(void)
{
   // pthread_mutex_lock(&lock);
    eit_event_list_node_t *temp = NULL,*temp1 = NULL;
  
   temp = list_head;

  while(temp!=NULL)
    {
      temp1 = temp->next;

      if(temp->data!=NULL)
      {
          if(temp->data->des!=NULL) {delete[] temp->data->des;};
          
          if(temp->data->title!=NULL) {delete[] temp->data->title;}
         
          delete temp->data;
      }
      
      delete temp;
      temp = temp1;
    }
   
 list_cnt = 0;
 list_head = NULL;
 
 //pthread_mutex_unlock(&lock);
}


/**
 * @brief       Usuwa element z listy z pod określonej pozycji
 * @param pos
 */
//void dvb_epg_events_list::remove_nth_node(uint16_t pos)
//{
//    eit_event_list_node_t *temp = NULL;
//    eit_event_list_node_t *prev_node = NULL;
//    
//    int i = 0;
//    
//    if(list_head==NULL) return;
//    
//    if(pos>=list_cnt) return;// max index = list_cnt - 1
//     
//    temp = list_head;
//    
//    //jesli mamy jeden element na liscie i chcemy go usunąć
//    if(pos==0)
//    {
//        list_head = temp->next;
//        
//        if(temp->data!=NULL)
//        {
//            if(temp->data->des!=NULL) {delete[] temp->data->des;};
//
//            if(temp->data->title!=NULL) {delete[] temp->data->title;}
//
//            delete temp->data;
//        }
//            
//        if(temp!=NULL) delete temp;
//     
//        list_cnt--;
//        return;
//    }
//
//    
//    while(temp!=NULL)
//      {
//        if(i==pos)
//        {
//            prev_node->next = temp->next;
//            
//           if(temp->data!=NULL)
//            {
//                if(temp->data->des!=NULL) {delete[] temp->data->des;};
//
//                if(temp->data->title!=NULL) {delete[] temp->data->title;}
//
//                delete temp->data;
//            }
//            
//            if(temp!=NULL) delete temp;
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
//
//
//

