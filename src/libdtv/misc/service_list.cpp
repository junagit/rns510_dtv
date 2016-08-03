/** 
 * @file   service_list.cpp
 * @author Michał Motyl
 *
 * @date   5 grudzień 2011, 13:11
 */
#include <stdint.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "service_list.h"



dvb_service_list::dvb_service_list() : list_head(NULL),list_end(NULL),list_cnt(0)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");

    }
//    list_head = NULL;
//    list_end = NULL;
//    list_cnt = 0;
 // L.clear ();
}


dvb_service_list::~dvb_service_list() 
{
    destroy_list();
    
    pthread_mutex_destroy(&lock);
}



/**
 * 
 * @param data
 * @return 
 */
uint8_t dvb_service_list::add_to_list(dvb_service *data)
{
   // pthread_mutex_lock(&lock);
    
 service_list_node_t *temp1 = NULL, *temp2 = NULL;
 
 if(data==NULL) 
 {
    // pthread_mutex_unlock(&lock);
     return 1;
 }
 
  temp1 = new service_list_node_t;
  temp1->data=data;

  // Copying the Head location into another node.
  //temp2=list_head;

  if(list_head == NULL)
  {
     list_head=temp1;
     list_head->next=NULL;
     list_end = (service_list_node_t *)list_head;
  }
  else
  {
//     #warning Przetestować działanie listy po dodaniu wskaźnika końca listy!!!
      temp2 = (service_list_node_t *)list_end;
      
     while(temp2->next != NULL)
     temp2=(service_list_node_t *)temp2->next;

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
dvb_service * dvb_service_list::get_from_list(uint16_t pos)
{
   // pthread_mutex_lock(&lock);
    
    service_list_node_t *temp = NULL;
    volatile int i = 0;
    
    if (pos >= 0 && pos < list_cnt)
    {
//    if(pos>(list_cnt-1)) 
//    {
//        pthread_mutex_unlock(&lock);
//        return NULL;// sprawdzamy czy nie jestesmy poza listą
//    }
    
        temp = (service_list_node_t *)list_head;
        
        if(pos==0) 
        {
       //     pthread_mutex_unlock(&lock);
            return temp->data;
        }
        else
        {
            while (i < pos)
           {
              temp = temp->next;
              i++;
           }   
            
      //      pthread_mutex_unlock(&lock);
            return temp->data;
        }
        
    
    
//    while(temp!=NULL)
//      {
//        if(i==pos) 
//        {
//            pthread_mutex_unlock(&lock);
//            return temp->data;
//        }
//        
//        temp = temp->next;
//        i++;
//      }

    }
  //  pthread_mutex_unlock(&lock);
    return NULL;
}


/**
 * 
 * @return Funkcja zwaraca liczbe elementów na liście;
 */
uint32_t dvb_service_list::get_list_count(void)
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
    //return L.size ();

    return list_cnt;
    
#endif
}



/**
 * 
 */
void dvb_service_list::destroy_list(void)
{
  //  pthread_mutex_lock(&lock);
   service_list_node_t *temp = NULL,*temp1;
   temp = (service_list_node_t *)list_head;

   if(list_cnt==0) 
   {
   //    pthread_mutex_unlock(&lock);
       return;
   }

  while(temp!=NULL)
    {
      temp1 = temp->next;

      if(temp->data!=NULL)
      {    
        if(temp->data->epg!=NULL) delete temp->data->epg;
        if(temp->data->epg_pf!=NULL) delete temp->data->epg_pf;

        temp->data->epg_events_list.destroy_list();   
        temp->data->epg_temp_events_list.destroy_list(); 
        temp->data->epg_pf_events_list.destroy_list();  
        temp->data->service_pid_list.destroy_list();

        delete temp->data;
      }
      
      delete temp;
      
      temp = temp1;
    }
   
 list_cnt = 0;
 list_head = NULL;
 list_end = NULL;
//  pthread_mutex_unlock(&lock);
 // L.clear ();
}


/**
 * @brief       Usuwa element z listy z pod określonej pozycji
 * @param pos
 */
void dvb_service_list::remove_nth_node(uint16_t pos)
{
  //pthread_mutex_lock(&lock);
//  if(L.isEmpty ()==true) return;
//  
//  if(pos>L.size ()) return;
//  
//  L.removeAt (pos);
    service_list_node_t *temp = NULL;
    service_list_node_t *prev_node = NULL;
    
    int i = 0;
    
    
    if(list_head==NULL) 
    {
     
   //     pthread_mutex_unlock(&lock);
        return;
    }
    
    if(pos>=list_cnt) 
    {
    //    pthread_mutex_unlock(&lock);
        return;
    }
        
     
    temp = (service_list_node_t *)list_head;
    
    //jesli mamy jeden element na liscie i chcemy go usunąć
    if(pos==0)
    {
        list_head = temp->next;
        
        if(temp!=NULL)
        {
            if(temp->data!=NULL)
            {
                if(temp->data->epg!=NULL) delete temp->data->epg;
                if(temp->data->epg_pf!=NULL) delete temp->data->epg_pf;

                temp->data->epg_events_list.destroy_list();   
                temp->data->epg_temp_events_list.destroy_list(); 
                temp->data->epg_pf_events_list.destroy_list();  
                temp->data->service_pid_list.destroy_list();

                delete temp->data;
                temp->data = NULL;
            }

            delete temp;

            list_cnt--;
        }
     //   pthread_mutex_unlock(&lock);
        return;
    }

    
    while(temp!=NULL)
      {
        if(i==pos)
        {
            prev_node->next = temp->next;
            
               if(temp!=NULL)
                {
                    if(temp->data!=NULL)
                    {
                        if(temp->data->epg!=NULL) delete temp->data->epg;
                        if(temp->data->epg_pf!=NULL) delete temp->data->epg_pf;

                        temp->data->epg_events_list.destroy_list();   
                        temp->data->epg_temp_events_list.destroy_list(); 
                        temp->data->epg_pf_events_list.destroy_list();  
                        temp->data->service_pid_list.destroy_list();

                        delete temp->data;
                    }

                    delete temp;

                    list_cnt--;
                }
       //     pthread_mutex_unlock(&lock);
            return;
          }
        prev_node = temp;
        temp = temp->next;
        
        i++;
      } 
  
 // pthread_mutex_unlock(&lock);
}

uint8_t dvb_service_list::move_down(uint16_t index)
{
   // pthread_mutex_lock(&lock);
    	uint16_t n=0;
	service_list_node_t *temp,*exchange;

	
	if (list_head==NULL)
		{
                      //   pthread_mutex_unlock(&lock);
			return 1;
		}
		else temp=(service_list_node_t *)list_head;

//index=1, change the head point;
	if (index == 1)
		{
			list_head=list_head->next;
			temp->next= list_head->next;
			list_head->next=temp;
			
                        //if (temp->next==NULL) tail=temp;
                    //    pthread_mutex_unlock(&lock);
			return 0;
		}

	while (temp!= NULL)
		{
			n++;
			if (n==index-1) 
				{
					if (temp->next->next!=NULL)
						{
						//index>1, exchange the points: 1->2->3->4  ==>  1->3->2->4     temp=1
							exchange=temp->next->next; //exchang=3
							temp->next->next=exchange->next;// 1->2->4
							exchange->next=temp->next; // 1->2->4   3->2->4
							temp->next=exchange;	// 1->3->2->4
							//if (exchange->next->next==NULL) tail=exchange->next->next;
						//	pthread_mutex_unlock(&lock);
                                                        return 0;
						}
						else 
                                                {
                                                    //    pthread_mutex_unlock(&lock);
                                                        return 1;
                                                }
				}
			temp=temp->next;    		
		}
      //  pthread_mutex_unlock(&lock);
        return 1;
  
//  if(L.isEmpty ()==true) return 1;
//  if(index+1<L.size ()) return 1;
//  
//  L.move (index,index+1);


}

uint8_t dvb_service_list::move_up(uint16_t index)
{
  //  pthread_mutex_lock(&lock);
    	int n=0;
	service_list_node_t *temp,*exchange;


	if (list_head==NULL) 
        {
     //       pthread_mutex_unlock(&lock);
            return 1;
        }
	else 
        temp=(service_list_node_t *)list_head;

	if (index==1)
        {		
     //       pthread_mutex_unlock(&lock);
          return 1;
        }
	
	if (index == 2)
        {
            list_head=list_head->next;
            temp->next= list_head->next;
            list_head->next=temp;
        //    pthread_mutex_unlock(&lock);
            return 0;
        }

	while (temp!= NULL)
	{
		n++;
		if (n==index-2)
                {
                //index>2, exchange the points:	1->2->3->4  ==>  1->3->2->4     temp=1
                        exchange=temp->next->next; //exchang=3
                        temp->next->next=exchange->next;// 1->2->4
                        exchange->next=temp->next; // 1->2->4   3->2->4
                        temp->next=exchange;	// 1->3->2->4
               //         pthread_mutex_unlock(&lock);
                        return 0;
                }
		temp=temp->next;
	}

      //  pthread_mutex_unlock(&lock);
	return 1;
  
  
//  if(L.isEmpty ()==true) return 1;
//  if((int)(index-1)<0) return 1;
//  
//  L.move (index,index-1);
  
}