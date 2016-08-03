/** 
 * @file   list.cpp
 * @author michalm
 * 
 * @date   5 maj 2011, 13:17
 */
#include <stdint.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>


#include "list.h"



mm_list::mm_list() 
{
    list_head = NULL;
   // list_tail = NULL;
    list_cnt = 0;
}



mm_list::~mm_list() 
{
    
    
}


/**
 * 
 * @param data
 * @return 
 */
uint8_t mm_list::add_to_list(browser_item *data)
{
 list_node_t *temp1 = NULL, *temp2 = NULL;

  temp1 = new list_node_t;
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
browser_item * mm_list::get_from_list(uint16_t pos)
{
    list_node_t *temp = NULL;
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
uint32_t mm_list::get_list_count(void)
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
int32_t mm_list::get_list_max_idx(void)
{
    if(list_cnt==0) return -1;
    
    return (list_cnt-1);
    
}

/**
 * @brief Funkcja usuwa z listy wszystkie elementy jak i dane z pod wskaźnika data;
 */
void mm_list::destroy_list(void)
{
   list_node_t *temp = NULL,*temp1;
   temp = list_head;


  while(temp!=NULL)
    {
      temp1 = temp->next;

      if(temp->data!=NULL)
      {
          if(temp->data->item_name!=NULL)
          {
              delete[] temp->data->item_name;
              temp->data->item_name = NULL;
          }
          
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
void mm_list::remove_nth_node(uint16_t pos)
{
    list_node_t *temp = NULL;
    list_node_t *prev_node = NULL;
    
    int i = 0;
    
    
    if(list_head==NULL) return;
    
    if(pos>list_cnt) return;
     
    temp = list_head;
    
    //jesli mamy jeden element na liscie i chcemy go usunąć
    if(pos==0)
    {
        list_head = temp->next;
        
        if(temp->data!=NULL)
            {
                if(temp->data->item_name!=NULL)
                {
                    delete[] temp->data->item_name;
                    temp->data->item_name = NULL;
                }
                
                delete temp->data;
                temp->data = NULL;
            }
            
            if(temp!=NULL) {
                delete temp;
                temp = NULL;
            }
            
            list_cnt--;
            return;
    }

    
    while(temp!=NULL)
      {
        if(i==pos){
            

            prev_node->next = temp->next;
            
            if(temp->data!=NULL)
            {
                delete temp->data;
                temp->data = NULL;
            }
            
            if(temp!=NULL) {
                delete temp;
                temp = NULL;
            }
            
            list_cnt--;
            return;
            
          }
        prev_node = temp;
        temp = temp->next;
        
        i++;
      } 
}

#if 0
uint8_t list::move_down(uint16_t index)
{
    	uint16_t n=0;
	list_node_t *temp,*exchange;

	
	if (list_head==NULL)
		{
			return 1;
		}
		else temp=list_head;

//index=1, change the head point;
	if (index == 1)
		{
			list_head=list_head->next;
			temp->next= list_head->next;
			list_head->next=temp;
			
                        //if (temp->next==NULL) tail=temp;
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
							return 0;
						}
						else 
                                                {
                                                        return 1;
                                                }
				}
			temp=temp->next;    		
		}
        
        return 1;

}

uint8_t list::move_up(uint16_t index)
{
    	int n=0;
	list_node_t *temp,*exchange;


	if (list_head==NULL) return 1;
	else 
        temp=list_head;

	if (index==1)
        {			
          return 1;
        }
	
	if (index == 2)
        {
            list_head=list_head->next;
            temp->next= list_head->next;
            list_head->next=temp;
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
                        return 0;
                }
		temp=temp->next;
	}

	return 1;
}
#endif

