/** 
 * @file   eit.cpp
 * @author michalm
 *
 * @date   5 grudzień 2011, 07:16
 */

#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>

#include "struct_define.h"
#include "eit.h"
#include "section.h"
#include "crc32.h"
#include "misc_func.h"
#include "descriptors.h"
#include "xml_read_write.h"
#include "service_list.h"


//static char add_buffer[256];       //!<Bufor pomocniczy
/**
 * @brief Funkcja pobierająca epg
 * @param freq
 * @param ts_id
 * @param service_list_p
 * @return 0 - jeśli OK
 */
uint8_t eit::get_eit_for_all_service(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    //struct section *section = NULL;
    //section_usr sbuf_epg;
    uint8_t segment_num;
    uint8_t segment_section;
    uint8_t table = 0;
    uint8_t last_section_number = 0;
    uint8_t seg_last_sec_num = 0;
    uint8_t table_id,curr_next_ind,last_table_id,section_num;
    
    #ifndef PC_VERSION
    uint16_t real_read_len;
    stream_t my_stream;
    #endif
        

//    struct timeval start_time,end_time;
//    uint16_t timeout = 30;
    uint16_t service_id = 0;
    uint32_t total_bytes_recieved = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int section_len = 0;
    int service_cnt;
    int i;
    uint8_t res = 0,tmp_cnt = 0;
    int service_cnt_with_epg = 0;
    uint8_t *data_ptr;
    uint16_t curr_transport_stream_id;


    if(service_list_p==NULL) return 1;
    
    service_cnt = service_list_p->get_list_count();
    
    if(service_cnt==0) return 1;

    //przesukujemy listę z programami i alokujemy pamięć dla epg dla kanałów które spełniaja warunek
    //if(freq==service_temp->frequency && ts_id==service_temp->transport_stream_id), dla aktualnej częstotliwości
    
    for(i = 0;i<service_cnt;i++)
      {
        service_temp = service_list_p->get_from_list(i);
        
        if(service_temp==NULL) continue;
             
        if(freq==service_temp->frequency && ts_id==service_temp->transport_stream_id)
        {
            if(service_temp->eit_schedule) 
            {
                service_temp->epg = new eit_shedule;//zaalokuj przestrezn dla epg dla konkretej stacjii
                memset(service_temp->epg,0,sizeof(eit_shedule));//trzeba wyzerować
                service_temp->epg_events_list.destroy_list();//na wszelki wypadek kasujemy eventy z listy z poprzedniego pobierania
                service_cnt_with_epg++;
            }
        }

      }

//    printf("service_cnt:%d\n",service_cnt);
//    printf("service_cnt_with_epg:%d\n",service_cnt_with_epg);
    
    if(service_cnt_with_epg==0) return 1;

    memset(&sbuf_epg,0,sizeof(section_usr));

    real_read_len = READ_LEN;

    my_stream.buffer = sbuf_epg.databuf;
    my_stream.read_len = (188);
    my_stream.ops = 0;
    my_stream.PID_type = PSI;
    my_stream.buff_idx = TSI_TIME_BUFFINDEX;

    my_stream.time_out = 100;//set the timeout to 2 seconds
    //my_stream.fill_buffer = tsi_fill_buffer;
    my_stream.type = 1;

    tsi_add_pid_to_filter(PID_EIT,PSI,TSI_TIME_BUFFINDEX);
//    gettimeofday(&start_time,NULL);

    epg_sched_busy = true;
    
      while(1)
     {    
          pthread_testcancel();
          
                res = find_section_live(PID_EIT,&sbuf_epg,&my_stream,3);    

            if(res!=0){
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                sbuf_epg.used = 0;
                
                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);
               
                continue;
            }
            
            
          if(_dvb_crc32(section_buf_data(sbuf_epg.section_bufs),sbuf_epg.section_bufs->len))
          {
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                sbuf_epg.used = 0;
                
                printf("@@@ EPG CRC ERROR!!!!\n");
                pthread_mutex_lock(&crc_err_mutex2);
                crc_err_cnt2++;
                pthread_mutex_unlock(&crc_err_mutex2);
                
                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);
                continue;
          }
            
                pthread_mutex_lock(&crc_err_mutex2);
                crc_err_cnt2 = 0;
                pthread_mutex_unlock(&crc_err_mutex2);
                
            section_len = (sbuf_epg.section_bufs->len - 4);//-4 ponieważ w buforze są dane z crc a ja potrzebuje do parsowania same dane bez crc
            
            data_ptr = section_buf_data(sbuf_epg.section_bufs);

            if(!(section_len>0)) {
                
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                sbuf_epg.used = 0;

                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);
                continue;
            }
   

            table_id = data_ptr[0];

            if(!(table_id>=0x50 && table_id<=0x5f)) {
                
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                sbuf_epg.used = 0;

                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);
                
                continue;//należy sprawdzać czy jest poprawny table_id!!!
            }            //w strumieniu skopiowanym z PC przek kartę wszystkie table_id są poprawne a na żywca nie!!!

            
             
            service_id = (data_ptr[3]<<8 | data_ptr[4]);
            curr_transport_stream_id = (data_ptr[8]<<8 | data_ptr[9]);
            section_num = data_ptr[6];
            last_section_number = data_ptr[7];
            curr_next_ind = (data_ptr[5]&0x01);
            last_table_id = data_ptr[13];
            seg_last_sec_num = data_ptr[12];

            if(!curr_next_ind) {
                
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                
                sbuf_epg.used = 0;
                
                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);
                
                continue;
            }
            
            table = (table_id%16);
            segment_num = (section_num/8);
            segment_section = (section_num % 8);
            
            for(i = 0;i<(service_cnt);i++)
              {
                service_temp = service_list_p->get_from_list(i);

                if(service_temp==NULL) continue;
                           
                    if(service_temp->service_id==service_id && service_temp->eit_schedule && service_temp->frequency==freq)
                        {
                            if(service_temp->transport_stream_id!=curr_transport_stream_id) 
                            {
                                continue;
                            }

                            epg_p = service_temp->epg;

                            if(epg_p==NULL)continue;   

                            if( epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer == NULL)
                              {                
                                total_bytes_recieved+= section_len;

                                epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer =  new uint8_t[section_len];  
                                memcpy (epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer,data_ptr,section_len);

                                EPG_DEBUG("table:%d segment_num:%d segment_section:%d service_id:%d last_table_id:0x%x last_section_number:%d\n",table,segment_num,segment_section,service_id,last_table_id,(last_section_number/8));

                                epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_len = section_len;

                                epg_p->eit_tables[table].eit_segment[segment_num].usage++;

                                if(epg_p->eit_tables[table].eit_segment[segment_num].usage == (seg_last_sec_num%8)+1){

                                    epg_p->eit_tables[table].usage++;

                                        if(epg_p->eit_tables[table].usage == ((last_section_number/8)+1)){
                                            epg_p->table_cnt++;
                                            EPG_DEBUG("table complete\n");
                                        }
                                    }

                                    if(epg_p->table_cnt==((last_table_id%16)+1))
                                    {
                                        tmp_cnt++;
                                        EPG_DEBUG("tmp_cnt:%d\n",tmp_cnt);
                                        if(tmp_cnt==service_cnt_with_epg)goto __exit;
                                    }

                                    break;
                                }
                            else
                              {
                                sbuf_epg.continuities = 0;
                                sbuf_epg.i = 0;
                                sbuf_epg.section_status = 0;
                                sbuf_epg.sz = 0;
                                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);

                                sbuf_epg.used = 0;

                                if(sbuf_epg.section_bufs!=NULL) section_buf_reset(sbuf_epg.section_bufs);

                                epg_p = NULL;
                              }
                      }
              }
            
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);
                sbuf_epg.used = 0;
                

             if(sbuf_epg.section_bufs!=NULL)section_buf_reset(sbuf_epg.section_bufs);
               
             epg_p = NULL;
         }
__exit:
                sbuf_epg.continuities = 0;
                sbuf_epg.i = 0;
                sbuf_epg.section_status = 0;
                sbuf_epg.sz = 0;
                memset(&sbuf_epg.tsvals,0,sizeof(struct transport_values));
                sbuf_epg.used = 0;
                memset(sbuf_epg.databuf,0,DATA_BUF_SIZE);        

                if(sbuf_epg.section_bufs!=NULL)
                {
                    section_buf_reset(sbuf_epg.section_bufs);
                    free(sbuf_epg.section_bufs);
                    sbuf_epg.section_bufs = NULL;
                }


   tsi_remove_pid_from_filter(PID_EIT);
                
    printf("EPG:total bytes recieved:%dkB\n",(total_bytes_recieved/1024));
    epg_sched_busy = false;
    return 0;
}


/**
 * @brief Funkcja zwalniająca zaalokowana pamięć podczas pobierania EPG
 * @param freq
 * @param ts_id
 * @param service_list_p
 */
void eit::free_all_eit_sections_pf(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t table = 0,i;
    uint8_t segment = 0;
    uint8_t section_num = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int service_cnt;
    
    bool present_following = true;
    
    if(service_list_p==NULL) return;
    
    service_cnt = service_list_p->get_list_count();

    if(service_cnt==0) return;
    

    for(i = 0;i<service_cnt;i++)
     {
          service_temp = service_list_p->get_from_list(i);

              if(service_temp==NULL) continue;

              //pobieramy epg dla okreśłonej częstotliwości i transport strema id  
              if(!service_temp->eit_schedule || freq !=service_temp->frequency || ts_id!=service_temp->transport_stream_id) continue;

              //jeżeli epg nie jest zaalokowane to nic nie robimy  
                if(present_following)
                {
                    if(service_temp->epg_pf==NULL) continue;
                }
                else
                if(service_temp->epg==NULL) continue;
          
                
                if(present_following)
                {
                    epg_p = service_temp->epg_pf;
                }
                else
                epg_p = service_temp->epg;
             
              for(table = 0;table<2;table++)
                  {
                      for(segment = 0;segment<32;segment++)
                      {
                          if(epg_p->eit_tables[table].eit_segment[segment].usage==0) continue;

                          for(section_num = 0;section_num<8;section_num++)
                          {
                              if(epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer==NULL) continue;
                              else
                              {
                                delete[] epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer;
                                epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer = NULL;  
                              }
                          }
                      }
                      
                  }
          
                      if(present_following)
                      {
                          if(service_temp->epg_pf!=NULL && service_temp->eit_present_following)
                            {
                                delete service_temp->epg_pf;
                                service_temp->epg_pf = NULL;
                                //service_temp->epg_pf_events_list.destroy_list();
                                EPG_DEBUG("free epg\n");
                            }
                      }
                      else
                      {
                        if(service_temp->epg!=NULL && service_temp->eit_schedule)
                        {
                            delete service_temp->epg;
                            service_temp->epg = NULL;
                            //service_temp->epg_events_list.destroy_list();
                            EPG_DEBUG("free epg\n");
                        }
                      }
      }
    
}

void eit::free_all_eit_sections_epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t table = 0,i;
    uint8_t segment = 0;
    uint8_t section_num = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int service_cnt;
    
    bool present_following = false;
    
    if(service_list_p==NULL) return;
    
    service_cnt = service_list_p->get_list_count();

    if(service_cnt==0) return;
    

    for(i = 0;i<service_cnt;i++)
     {
          service_temp = service_list_p->get_from_list(i);

              if(service_temp==NULL) continue;

              //pobieramy epg dla okreśłonej częstotliwości i transport strema id  
              if(!service_temp->eit_schedule || freq !=service_temp->frequency || ts_id!=service_temp->transport_stream_id) continue;

              //jeżeli epg nie jest zaalokowane to nic nie robimy  
                if(present_following)
                {
                    if(service_temp->epg_pf==NULL) continue;
                }
                else
                if(service_temp->epg==NULL) continue;
          
                
                if(present_following)
                {
                    epg_p = service_temp->epg_pf;
                }
                else
                epg_p = service_temp->epg;
             
              for(table = 0;table<2;table++)
                  {
                      for(segment = 0;segment<32;segment++)
                      {
                          if(epg_p->eit_tables[table].eit_segment[segment].usage==0) continue;

                          for(section_num = 0;section_num<8;section_num++)
                          {
                              if(epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer==NULL) continue;
                              else
                              {
                                delete[] epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer;
                                epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer = NULL;  
                              }
                          }
                      }
                      
                  }
          
                      if(present_following)
                      {
                          if(service_temp->epg_pf!=NULL && service_temp->eit_present_following)
                            {
                                delete service_temp->epg_pf;
                                service_temp->epg_pf = NULL;
                                //service_temp->epg_pf_events_list.destroy_list();
                                EPG_DEBUG("free epg\n");
                            }
                      }
                      else
                      {
                        if(service_temp->epg!=NULL && service_temp->eit_schedule)
                        {
                            delete service_temp->epg;
                            service_temp->epg = NULL;
                            //service_temp->epg_events_list.destroy_list();
                            EPG_DEBUG("free epg\n");
                        }
                      }
      }
    
}

/**
 * @brief Główna funkcja pobierająca EPG
 * @param freq
 * @param ts_id
 * @param service_list_p
 * @return 0 - OK
 */
uint8_t eit::Download_Epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t res;
 
    if(service_list_p==NULL) return 10;
    
    if((res = get_eit_for_all_service(freq,ts_id,service_list_p))==0)
       {
           pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
           
           EPG_DEBUG("POBRALEM DOBRZE EPG....\n");
           
           //sprawdzamy czy inny wątek nie czyta epg z pliku - jezeli tak to czekamy
          
           if(!parse_eit_sections_for_epg(freq,ts_id,service_list_p))
           {    
               EPG_DEBUG("SPARSOWALEM DOBRZE EPG....\n");
               //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
               return 0;
           }
           else
           {
               EPG_DEBUG("BŁĄD PARSOWANIA EPG....\n");
               free_all_eit_sections_epg(freq,ts_id,service_list_p);
               pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
               return 2;
           }
           
       }
       else
       {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        
        free_all_eit_sections_epg(freq,ts_id,service_list_p);

        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        
        if(res==2) return 2;

        return 1; 
       }
}

/**
 * @brief Funkcja parsuje pobrane surowe dane z sieci
 * @param freq
 * @param ts_id
 * @param service_list_p
 * @return 0 - OK
 */
uint8_t eit::parse_eit_sections_for_pf(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t table = 0,i;
    uint8_t segment = 0;
    uint8_t section_num = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int service_cnt;
    
    bool present_following = true;
    
    if(service_list_p==NULL) return 1;
    
    service_cnt = service_list_p->get_list_count();


    for(i = 0;i<service_cnt;i++)
     {
          service_temp = service_list_p->get_from_list(i);
          

          if(service_temp==NULL) continue;

          if(present_following) 
          {
              if(!service_temp->eit_present_following) continue;//jesli nie posiada epg to omin ta stację
          }
          else
          {
                if(!service_temp->eit_schedule) continue;//jesli nie posiada epg to omin ta stację
          }
          
          if(service_temp->frequency!=freq || service_temp->transport_stream_id!=ts_id) continue;
          
          
          
          if(present_following) 
          {
              if(service_temp->epg_pf==NULL) continue;
          }
          else
          if(service_temp->epg==NULL) continue;
           
          
//        if(present_following)
//        {
//            if(service_temp->epg_pf_events_list.get_list_count()>0) service_temp->epg_pf_events_list.destroy_list();
//        }
//        else
//        {
//            if(service_temp->epg_events_list.get_list_count()>0) service_temp->epg_events_list.destroy_list();
//        }
                
          
              if(present_following) epg_p = service_temp->epg_pf;
              else
              epg_p = service_temp->epg;
             
              

              for(table = 0;table<2;table++)
                  { 
                      for(segment = 0;segment<32;segment++)
                      {
                          if(epg_p->eit_tables[table].eit_segment[segment].usage==((uint8_t)0)) continue;

                          for(section_num = 0;section_num<8;section_num++)
                          {
                              if(epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer==NULL) continue;
                              else
                              {
                         //         DEMUXER_DEBUG("table:%d segment:%d sectiono num:%d  len:%d\n",table,segment,section_num,epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_len);
                                    parse_eit_section(&epg_p->eit_tables[table].eit_segment[segment].section[section_num],service_temp,present_following);
                                    
                                    delete[] epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer;
                                    epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer = NULL;
                              }
                          }
                      }
 
                  }
          
                  if(present_following)
                    {
                        if(service_temp->epg_pf!=NULL && service_temp->eit_present_following)
                        {
                            delete service_temp->epg_pf;
                            service_temp->epg_pf = NULL;
                        }
                    }
                    else
                    {
                        if(service_temp->epg!=NULL && service_temp->eit_schedule)
                        {
                            delete service_temp->epg;
                            service_temp->epg = NULL;
                        }
                    }
      }
    
    return 0;
}

uint8_t eit::parse_eit_sections_for_epg(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t table = 0,i;
    uint8_t segment = 0;
    uint8_t section_num = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int service_cnt;
    
    bool present_following = false;
    
    if(service_list_p==NULL) return 1;
    
    service_cnt = service_list_p->get_list_count();


    for(i = 0;i<service_cnt;i++)
     {
          service_temp = service_list_p->get_from_list(i);
          

          if(service_temp==NULL) continue;

          if(present_following) 
          {
              if(!service_temp->eit_present_following) continue;//jesli nie posiada epg to omin ta stację
          }
          else
          {
                if(!service_temp->eit_schedule) continue;//jesli nie posiada epg to omin ta stację
          }
          
          if(service_temp->frequency!=freq || service_temp->transport_stream_id!=ts_id) continue;
          
          
          
          if(present_following) 
          {
              if(service_temp->epg_pf==NULL) continue;
          }
          else
          if(service_temp->epg==NULL) continue;
           
          
//        if(present_following)
//        {
//            if(service_temp->epg_pf_events_list.get_list_count()>0) service_temp->epg_pf_events_list.destroy_list();
//        }
//        else
//        {
//            if(service_temp->epg_events_list.get_list_count()>0) service_temp->epg_events_list.destroy_list();
//        }
                
          
              if(present_following) epg_p = service_temp->epg_pf;
              else
              epg_p = service_temp->epg;
             
              

              for(table = 0;table<2;table++)
                  { 
                      for(segment = 0;segment<32;segment++)
                      {
                          if(epg_p->eit_tables[table].eit_segment[segment].usage==((uint8_t)0)) continue;

                          for(section_num = 0;section_num<8;section_num++)
                          {
                              if(epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer==NULL) continue;
                              else
                              {
                         //         DEMUXER_DEBUG("table:%d segment:%d sectiono num:%d  len:%d\n",table,segment,section_num,epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_len);
                                    parse_eit_section(&epg_p->eit_tables[table].eit_segment[segment].section[section_num],service_temp,present_following);
                                    
                                    delete[] epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer;
                                    epg_p->eit_tables[table].eit_segment[segment].section[section_num].section_buffer = NULL;
                              }
                          }
                      }
 
                  }
          
                  if(present_following)
                    {
                        if(service_temp->epg_pf!=NULL && service_temp->eit_present_following)
                        {
                            delete service_temp->epg_pf;
                            service_temp->epg_pf = NULL;
                        }
                    }
                    else
                    {
                        if(service_temp->epg!=NULL && service_temp->eit_schedule)
                        {
                            delete service_temp->epg;
                            service_temp->epg = NULL;
                        }
                    }
      }
    
    return 0;
}

/**
 * @brief Funkcja parsująca pojedynczyą sekcję eit
 * @param section
 * @param service_temp
 */
void eit::parse_eit_section(eit_section *section,dvb_service *service_temp,bool present_following)
{
    uint8_t     *data_ptr = section->section_buffer;
    uint8_t     *data_p,dsc_tag,dsc_len;
    uint16_t	len = 0;
    uint16_t    dsc_loop_len,dsc_loop_len_temp;
    char	*str_p = NULL;
    char	description[4096];
    char	title[256];
    eit_event	*event = NULL;
    time_t      timep;
    struct      tm *p;
    time_t      start_time,end_time;
    dvb_ts_time  	start_time2;
    uint16_t    event_id;
    uint16_t service_id;

    
    if(service_temp==NULL) return;
    
    
    len = section->section_len;

    if(len==0)
      {
	EPG_DEBUG("section_len=0!!!!\n");
	return;
      }

    memset(description,0,4096);
    memset(title,0,256);

    if(data_ptr == NULL)
      {
	EPG_DEBUG("NULL DATA!!!\n");
	return;
      }
    
    service_id = (data_ptr[3]<<8 | data_ptr[4]);

    data_ptr+=14;//skaczemy do pierwszego pola w pętli - event_id
    len=(len-14);
    
   // memset(add_buffer,0,256);
    
    while(len)
    {
        event_id = data_ptr[0]<<8 | data_ptr[1];
        time(&timep);
        p=gmtime(&timep);
        parseMJD(data_ptr[2]<<8|data_ptr[3],&start_time2);
        start_time2.hour   = ((data_ptr[4]&0xf0)>>4)*10+(data_ptr[4]&0x0f);
        start_time2.minute = ((data_ptr[5]&0xf0)>>4)*10+(data_ptr[5]&0x0f);
        start_time2.second = ((data_ptr[6]&0xf0)>>4)*10+(data_ptr[6]&0x0f);

        p->tm_year = start_time2.year;
        p->tm_mon  = start_time2.month-1;
        p->tm_mday = start_time2.day;
        p->tm_hour = start_time2.hour;
        p->tm_min = start_time2.minute;
        p->tm_sec = start_time2.second;
        start_time = mktime(p);


        int tmp_hour = ((data_ptr[7]&0xf0)>>4)*10 + (data_ptr[7]&0x0f);
        int tmp_min  = ((data_ptr[8]&0xf0)>>4)*10 + (data_ptr[8]&0x0f);
        int tmp_sec  = ((data_ptr[9]&0xf0)>>4)*10 + (data_ptr[9]&0x0f);
        
        end_time = tmp_hour*3600 + tmp_min*60 + tmp_sec + start_time;
        dsc_loop_len_temp = dsc_loop_len = (data_ptr[10]&0x0f)<<8 | data_ptr[11];
        data_p = data_ptr+12;

        event = new eit_event;
        memset(event,0,sizeof(eit_event));

        while(dsc_loop_len)
        {
            dsc_tag = data_p[0];
            dsc_len = data_p[1];
	  
            switch(dsc_tag)
            {
                case SHORT_EVENT_DSC:
                     //   EPG_DEBUG("Short event descriptor\n");

                    str_p = get_short_event_dsc(data_p,description);

                    if(str_p!=NULL) strcat(title,str_p);
                    
//                    if(add_info==1)
//                    {
//                        add_info=0;
//                            if(strlen(add_buffer)>0){
//                            add_buffer[0] = toupper(add_buffer[0]);
//                            strcat(description,add_buffer);
//                            strcat(description,".");
//                            strcat(description,"\n");
//                        }
//                        memset(add_buffer,0,256);
//                    }         
                    break;

                case EXTENDED_EVENT_DSC:
                     //   EPG_DEBUG("Extended event descriptor\n");

		    //trzeba kopiać ze sprawdzaniem czy wskaźnik nie jest pusty
		    str_p = get_extended_event_dsc(data_p);

		    if(str_p!=NULL) strcat(description,str_p);
                    else
                    {
                     //   printf("###### str_p==NULL!!!!!!!!!!!\n");
                    }
                    break;

                case CONTENT_DSC:
                     //   EPG_DEBUG("Content descriptor 0x%x     %c%c%c\n",data_p[3],data_p[2],data_p[4],data_p[5]);
                        

                    break;

                case PARENTAL_RATING_DSC:
                    //    EPG_DEBUG("Parental Rating descriptor\n");

               //     parse_parental_desc(data_p,event);
                    break;


                default:
                    EPG_DEBUG("Unknown descriptor dsc_tag:0x%x\n",dsc_tag);
                    break;
            }

            dsc_loop_len-=(dsc_len+2);
            data_p+=(dsc_len+2);

        }

        len-=(dsc_loop_len_temp+12);
        data_ptr+=(dsc_loop_len_temp+12);
        
        if(event==NULL) continue;

	event->start_time = start_time;
        event->end_time = end_time;
        event->event_id = event_id;

        uint16_t title_len = strlen(title);

	if(title_len)
        {//alokujemy pamięć i kopiujemy tytuł
            event->title = new char[title_len+1];
            memset(event->title,0,(sizeof(char)*title_len)+1);
	    memcpy (event->title,title,title_len);
        }
        else
            event->title = NULL;
        
      
        uint16_t description_len = strlen(description);
        

        if(description_len)
        {//alokujemy pamięć na opis i kopiujemy
           event->des = new char[description_len+1];
            memset(event->des,0,(sizeof(char)*description_len)+1);
	    memcpy (event->des,description,description_len);
        }
        else
            event->des = NULL;
        
        if(present_following==true)
        {
            service_temp->epg_pf_events_list.add_to_list(event);
        }
        else
        {
          service_temp->epg_events_list.add_to_list(event);
        }
        
	event = NULL;

        memset(description,0,4096);
        memset(title,0,256);
    }

    return;
}

/**    
 * @brief Funkcja pobierajca tytuł audycji TV z danych eit
 * @param buf - wskaźnik na bufor z danymi
 * @return 
 */
char * eit::get_short_event_dsc(uint8_t *buf,char *add_inf)
{
uint8_t event_name_len = buf[5];

char add_buffer[256];
memset(add_buffer,0,256);

    if(event_name_len)
      {
        if(buf[6+event_name_len]>0)
        {
            strcpy(add_buffer,generate_xml_text ((unsigned char*)&buf[6+event_name_len+1],(unsigned int)buf[6+event_name_len]));
            
            if(strlen(add_buffer)>0){
            add_buffer[0] = toupper(add_buffer[0]);
            strcat(add_inf,add_buffer);
            strcat(add_inf,".");
            strcat(add_inf,"\n");
            }
//            add_info = 1;
            
        }
//        else
//        add_info = 0;
        
        
	return generate_xml_text ((unsigned char*)&buf[6],event_name_len);
	//return (char*)&buf[9];
      }
      else
        return NULL;
      
}

/**
 * @brief Funkcja pobierająca długi opis dla audycji TV z danych eit
 * @param buf - wskaźnik na bufor z danymi 
 * @return 
 */
char * eit::get_extended_event_dsc(uint8_t *buf)
{
    uint8_t dsc_tag = buf[0];
    uint8_t dsc_len = buf[1];
//    uint8_t dsc_num = (buf[2]& 0xf0)>>4;
//    uint8_t last_dsc_num = (buf[2]& 0x0f);
//    uint8_t lang_code[3];
    uint8_t length_of_items = buf[6];
    uint8_t item_dsc_len;
    uint8_t *temp_buf = buf;
    uint8_t txt1[1024],txt2[1024];
    uint8_t txt_len;
    uint8_t length_of_items_temp = length_of_items;

    if(length_of_items) length_of_items_temp--;//chyba tak trzeba


    if(dsc_tag!=0x4e) return NULL;

    if(!dsc_len) return NULL;

    dsc_len-=7;
    temp_buf+=7;

	//poniższą pętle można wyżucić i skoczyć od razu do długiego opisu
        while(length_of_items)
        {
            item_dsc_len = temp_buf[0];

            memcpy(txt1,&temp_buf[4],(item_dsc_len-3));
	   
            temp_buf+=(item_dsc_len+1);
            length_of_items -= (item_dsc_len+1);

            item_dsc_len = temp_buf[0];
            memcpy(txt2,&temp_buf[4],item_dsc_len);

            temp_buf+=(item_dsc_len+1);
            length_of_items -= (item_dsc_len+1);
        }

        dsc_len -=length_of_items_temp;

	if(dsc_len==0) return NULL;

	//=======================
	//Długi opis
        txt_len = temp_buf[0];
	dsc_len-=txt_len;

	if(txt_len)
	  {
	    //DEMUXER_DEBUG("txt len:%d\n",txt_len);
	    temp_buf++;

	    return generate_xml_text ((unsigned char*)&temp_buf[0],(unsigned int)txt_len);
	  }
	//======================
	else
	  return NULL;
}


/**
 * @brief Funkcja czyści zaalokowana pamięć dla stworzonej listy eventów
 * @param freq
 * @param ts_id
 * @param service_list_p
 */
void eit::clean_epg_download_res(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
   
    if(this->sbuf_epg.section_bufs!=NULL)
    {
        section_buf_reset(this->sbuf_epg.section_bufs);
        free(this->sbuf_epg.section_bufs);
        this->sbuf_epg.section_bufs = NULL;
    }
    
    if(service_list_p==NULL) return;
        
    free_all_eit_sections_epg(freq,ts_id,service_list_p);
      
}

/**
 * @brief Funkcja czyści zaalokowana pamięć dla stworzonej listy eventów
 * @param freq
 * @param ts_id
 * @param service_list_p
 */
void eit::clean_epg_download_present_following_res(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    if(sbuf_epg_pf.section_bufs!=NULL)
    {
        section_buf_reset(sbuf_epg_pf.section_bufs);
        free(sbuf_epg_pf.section_bufs);
        sbuf_epg_pf.section_bufs = NULL;
    }
    
    if(service_list_p==NULL) return;

    free_all_eit_sections_pf(freq,ts_id,service_list_p);     

}





/**
 * @brief Funkcja tworzy pojedyńczy plik z EPG dla jednej usługi(stacji TV)
 * @param freq
 * @param service_id
 * @param ts_id
 * @return Zwraca 0 jesli ok
 */
uint8_t eit::save_service_2epg(uint32_t freq,uint16_t service_id,uint16_t ts_id,dvb_epg_events_list *service_event_list)
{
    uint8_t epg_file_name[255];
    FILE *fp = NULL;
    uint16_t events_number,i;
    eit_event *event = NULL;
    static uint16_t start_header = 0x55AA;
    uint16_t len,events_number_s = 0;
    uint16_t x1 = 0,y1 = 0;
    eit_event *tmp = NULL;
    //uint16_t progs_cnt_in_day[8];
    epg_info_entry progs_cnt_in_day[8];
    struct tm * timeinfo = NULL;
    
    if(service_event_list==NULL) return 1;
    
    events_number = service_event_list->get_list_count();
    
    if(events_number==0) return 1;
    
    memset(epg_file_name,0,255);
    
    for(i = 0;i<8;i++) memset(&progs_cnt_in_day[i],0,sizeof(epg_info_entry));
    
    sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",freq,service_id,ts_id);
    
    EPG_DEBUG("EPG FILE:%s\n",epg_file_name);
    
    tmp = service_event_list->get_from_list(0);
    
    if(tmp==NULL) return 1;
    
    fp = fopen((char*)epg_file_name,"wb+");
    
    if(fp==NULL) 
    {
        EPG_DEBUG("Can't create epg file...\n");
        return 1;
    }
//******************************************************************************
    //liczymy ilości eventów w każdym dniu
    x1 = 0;
    y1 = 0;
    
    
    timeinfo = localtime ( &tmp->start_time );

    //unsigned int prev = event_tab[0]->start_time.day;
    uint16_t prev = timeinfo->tm_mday;
    uint16_t count = 1;
    uint16_t ARRAYSIZE = events_number;
    
    progs_cnt_in_day[y1].epg_day_size_in_bytes+=(8+(2*sizeof(time_t)));
    if(tmp->title!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->title); 
    if(tmp->des!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->des);

    for (x1 = 1; x1 < ARRAYSIZE; x1++)
    {
        tmp = service_event_list->get_from_list(x1);
        
        if(tmp==NULL) continue;
 
        timeinfo = localtime ( &tmp->start_time );
    
        if (timeinfo->tm_mday == prev)
        {
            prev = timeinfo->tm_mday;
            
            progs_cnt_in_day[y1].epg_day_size_in_bytes+=(8+(2*sizeof(time_t)));
            if(tmp->title!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->title); 
            if(tmp->des!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->des);
            
            ++count;
            
        } else {

            progs_cnt_in_day[y1].epg_events_in_day = count;
            ++y1;

            if(timeinfo!=NULL) prev = timeinfo->tm_mday;
            else
            {
                EPG_DEBUG("timeinfo==NULL!!!\n");
            }
            count = 1; ///zero
            
            progs_cnt_in_day[y1].epg_day_size_in_bytes+=(8+(2*sizeof(time_t)));
            if(tmp->title!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->title); 
            if(tmp->des!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->des);
            
        }
    }
    
    progs_cnt_in_day[y1].epg_events_in_day = count;
    
    progs_cnt_in_day[y1].epg_day_size_in_bytes+=(8+(2*sizeof(time_t)));
    if(tmp->title!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->title); 
    if(tmp->des!=NULL)progs_cnt_in_day[y1].epg_day_size_in_bytes+=strlen(tmp->des);
    

 //   printf("#######################################################\n");
    //zapis do pliku tablicy w której są zliczone ilości eventów w każdym dniu
    for(i = 0;i<8;i++) 
    {
        fwrite(&progs_cnt_in_day[i].epg_events_in_day,1,4,fp); 
        fwrite(&progs_cnt_in_day[i].epg_day_size_in_bytes,1,4,fp); 
      //  printf("epg_events_in_day:%d epg_day_size_in_bytes:%d\n",progs_cnt_in_day[i].epg_events_in_day,progs_cnt_in_day[i].epg_day_size_in_bytes);
    }
//******************************************************************************

    for(i = 0;i<events_number;i++)
    {
        event = service_event_list->get_from_list(i);

        if(event==NULL) continue;
        
        events_number_s++;
        //zapis nagłówka eventu
        fwrite(&start_header,1,sizeof(uint16_t),fp);
        
        fwrite(&event->event_id,1,sizeof(uint16_t),fp);
        fwrite(&event->start_time,1,sizeof(time_t),fp);
        fwrite(&event->end_time,1,sizeof(time_t),fp);
        
        //zapis tytułu
        if(event->title!=NULL){
            len = strlen(event->title);
            fwrite(&len,1,2,fp);//zapis długości tytułu
            fwrite(event->title,1,len,fp);
        }
        else
        {
            len = 0;
            fwrite(&len,1,2,fp);//zapis długości tytułu
        }
        
        if(event->des!=NULL){
         
            len = strlen(event->des);
            fwrite(&len,1,2,fp);//zapis długości opisu
            fwrite(event->des,1,len,fp);
        }
        else
        {
            len = 0;
            fwrite(&len,1,2,fp);//zapis długości tytułu
        }
    }
   
    EPG_DEBUG("saved events number:%d\n",events_number_s);
    fclose(fp);
    //sync();
    return 0;
}

/**
 * @brief Funkcja ładująca z pliku EPG dla określonej stacji TV. Fukcja ładuje dane do listy przekazanej w parametrze service_event_list
 * @param freq - czestotliwość
 * @param service_id - id kanału
 * @param ts_id - identyfikator TRANSPORT STREAM
 * @param service_event_list - wskaxnik na listę do której maja być wrzycane odczytywane dane
 * @return 0 - OK
 */
uint8_t eit::load_service_epg_from_file(uint32_t freq,uint16_t service_id,uint16_t ts_id,dvb_epg_events_list *service_event_list,uint16_t *days_p)
{
    uint8_t epg_file_name[255];
    FILE *fp = NULL;
    uint16_t events_number = 0;
    eit_event *event = NULL;
    uint16_t start_header = 0x55AA;
    uint16_t read_start_header;
    uint32_t file_len;
    uint16_t txt_len = 0,i;
    
    memset(epg_file_name,0,255);
    sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",freq,service_id,ts_id);
    
    EPG_DEBUG("EPG FILE:%s\n",epg_file_name);
    
    fp = fopen((char*)epg_file_name,"r");
    
    if(fp==NULL) 
    {
        EPG_DEBUG("Can't open epg file...\n");
        return 1;
    }
    
    //pobierz długość pliku
    fseek(fp,0,SEEK_END);
    file_len = ftell(fp);
    fseek(fp,0,SEEK_SET);
    
        //zapis do pliku tablicy w której są zliczone ilości eventów w każdym dniu
    for(i = 0;i<8;i++) fread(&days_p[i],1,sizeof(uint16_t),fp); 
    
    file_len-=(8*sizeof(uint16_t));
    
    while(file_len)
    {
        fread(&read_start_header,1,2,fp);
        file_len-=2;
        
        if(read_start_header==start_header)
        {
          event = new eit_event;
          memset(event,0,sizeof(eit_event));
          
          events_number++;
          
          if(event!=NULL)
          {
            fread(&event->event_id,1,2,fp);
            file_len-=2;
            
            fread(&event->start_time,1,sizeof(time_t),fp);
            file_len-=sizeof(time_t);
            fread(&event->end_time,1,sizeof(time_t),fp);
            file_len-=sizeof(time_t);
            
            fread(&txt_len,1,2,fp);
            file_len-=2;
            
            if(txt_len>0)
            {
              event->title = new char[txt_len+1];
              memset(event->title,0,txt_len+1);
              fread(event->title,1,txt_len,fp);
              file_len-=txt_len;
              txt_len = 0;
            }
            
            fread(&txt_len,1,2,fp);
            file_len-=2;
            
            if(txt_len>0)
            {
              event->des = new char[txt_len+1];
              memset(event->des,0,txt_len+1);
              fread(event->des,1,txt_len,fp);
              file_len-=txt_len;
              txt_len = 0;
            }
           
            service_event_list->add_to_list(event);
          } 
        }
        else
        {
            EPG_DEBUG("WRONG HEADER....\n");
            fclose(fp);
            remove((char*)epg_file_name);
            sync();
            return 0;
        }
    }
    
    EPG_DEBUG("loaded events number:%d\n",events_number);
    fclose(fp);
    return 0;
}



/**
 * @brief Funkcja zapisuje EPG dla każdej stacji z listy do osobnego pliku
 * @param list - wskaźnik na liste z programami
 */
void eit::Save_EPG_data_for_each_service(dvb_service_list *list,uint32_t freq,uint32_t tsid)
{
    volatile uint16_t service_cnt = 0,i = 0;
    dvb_service *tmp = NULL;
    //dvb_service_list *list_temp = list;
    
    if(list==NULL) return;
    
    service_cnt = list->get_list_count();
    
    if(service_cnt == 0) return;
    
    for(i = 0;i<service_cnt;i++)
    {
      //  printf("!!!\n");
        tmp = list->get_from_list(i);
      //  printf("i:%d %p\n",i,tmp);
        fflush(stdout);
        

        if(tmp!=NULL)
        {
            if(tmp->eit_schedule && tmp->frequency==freq && tmp->transport_stream_id==tsid)
            {       
            //    printf(".\n");
                (void)save_service_2epg(tmp->frequency,tmp->service_id,tmp->transport_stream_id,&tmp->epg_events_list);
            }
        }
       //  printf("...\n");

    }
    
    sync();
}



/**
 * 
 * @param freq
 * @param ts_id
 * @param service_list_p
 * @return 
 */
uint8_t eit::get_eit_present_following_for_services(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    //section_usr sbuf_epg;
    uint8_t segment_num;
    uint8_t segment_section;
    uint8_t table = 0;
    uint8_t last_section_number = 0;
    uint8_t seg_last_sec_num = 0;
    uint8_t table_id,curr_next_ind,last_table_id,section_num;
    uint16_t real_read_len;
    stream_t my_stream;

       
    //struct timeval start_time,end_time;
    //uint16_t timeout = 30;
    uint16_t service_id = 0;
    uint32_t total_bytes_recieved = 0;
    eit_shedule *epg_p = NULL;
    dvb_service *service_temp = NULL;
    int section_len = 0;
    int service_cnt;
    int i;
    uint8_t res = 0,tmp_cnt = 0;
    int service_cnt_with_epg = 0;
    uint8_t *data_ptr = NULL;
    uint16_t curr_transport_stream_id;

    if(service_list_p==NULL) return 1;
    
    service_cnt = service_list_p->get_list_count();
    
    if(service_cnt==0) return 1;

    //przesukujemy listę z programami i alokujemy pamięć dla epg dla kanałów które spełniaja warunek
    //if(freq==service_temp->frequency && ts_id==service_temp->transport_stream_id), dla aktualnej częstotliwości
  
    for(i = 0;i<service_cnt;i++)
      {
        service_temp = service_list_p->get_from_list(i);
        
        if(service_temp==NULL) continue;
             
        if(freq==service_temp->frequency && ts_id==service_temp->transport_stream_id)
        {
            if(service_temp->eit_present_following) 
            {
                if(service_temp->epg_pf_events_list.get_list_count()>0) service_temp->epg_pf_events_list.destroy_list();
                
                service_temp->epg_pf = new eit_shedule;//zaalokuj przestrezn dla epg dla konkretej stacjii
                memset(service_temp->epg_pf,0,sizeof(eit_shedule));//trzeba wyzerować
                service_temp->epg_pf_events_list.destroy_list();//na wszelki wypadek kasujemy eventy z listy z poprzedniego pobierania
                service_cnt_with_epg++;
            }
        }
      }
    
    if(service_cnt_with_epg==0) return 1;

    memset(&sbuf_epg_pf,0,sizeof(section_usr));

    real_read_len = READ_LEN;

    my_stream.buffer = sbuf_epg_pf.databuf;
    my_stream.read_len = (188);
    my_stream.ops = 0;
    my_stream.PID_type = PSI;
    my_stream.buff_idx = TSI_EPG_BUFFINDEX;
    my_stream.time_out = 100;//set the timeout to 2 seconds
    my_stream.type = 1;

    tsi_add_pid_to_filter(PID_EIT,PSI,TSI_EPG_BUFFINDEX);

    epg_pf_busy = true;
    
    pthread_mutex_lock(&crc_err_mutex);
    crc_err_cnt = 0;
    pthread_mutex_unlock(&crc_err_mutex);
     while(1)
     {   
          pthread_testcancel();

             res = find_section_live(PID_EIT,&sbuf_epg_pf,&my_stream,3);    

           //  printf("## find_section_live:%d\n",res);
            
            if(res!=0){
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                sbuf_epg_pf.used = 0;
                
                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);
               
                continue;
            }
            
            
          if(_dvb_crc32(section_buf_data(sbuf_epg_pf.section_bufs),sbuf_epg_pf.section_bufs->len))
          {
                EPG_DEBUG("CRC ERROR sbuf_epg_pf.section_bufs->len:%d\n",sbuf_epg_pf.section_bufs->len);
                
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                sbuf_epg_pf.used = 0;

                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);
                
                pthread_mutex_lock(&crc_err_mutex);
                crc_err_cnt++;
                pthread_mutex_unlock(&crc_err_mutex);
  
                continue;
          }
                
             pthread_mutex_lock(&crc_err_mutex);
            crc_err_cnt = 0;
            pthread_mutex_unlock(&crc_err_mutex);
            
            section_len = (sbuf_epg_pf.section_bufs->len - 4);//-4 ponieważ w buforze są dane z crc a ja potrzebuje do parsowania same dane bez crc
            
            data_ptr = section_buf_data(sbuf_epg_pf.section_bufs);

            if(!(section_len>0)) {
                
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                sbuf_epg_pf.used = 0;

                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);
                continue;//bufor = 0; nie ma danych
            }
   

            table_id = data_ptr[0];
         //   

            if(table_id!=0x4e) {
                
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                sbuf_epg_pf.used = 0;

                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);
                
                continue;//należy sprawdzać czy jest poprawny table_id!!!
            }            //w strumieniu skopiowanym z PC przek kartę wszystkie table_id są poprawne a na żywca nie!!!

            
             
            service_id = (data_ptr[3]<<8 | data_ptr[4]);
            curr_transport_stream_id = (data_ptr[8]<<8 | data_ptr[9]);
            section_num = data_ptr[6];
            last_section_number = data_ptr[7];
            curr_next_ind = (data_ptr[5]&0x01);
            last_table_id = 0;
            //last_table_id = data_ptr[13];
            seg_last_sec_num = data_ptr[12];
            
            //printf("service_id:0x%x section_num:%d last_section_number:%d\n",service_id,section_num,last_section_number);
            
            if(!curr_next_ind) {

                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                
                sbuf_epg_pf.used = 0;
                
                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);
                
                continue;
            }
            
            //table = (table_id%16);
            table = 0;
            segment_num = (section_num/8);
            segment_section = (section_num % 8);
            
            for(i = 0;i<service_cnt;i++)
              {
                service_temp = service_list_p->get_from_list(i);

                if(service_temp==NULL) continue;    
                           
                    if(service_temp->service_id==service_id && service_temp->eit_present_following)
                        {
                            if(service_temp->transport_stream_id!=curr_transport_stream_id) continue;

                            epg_p = service_temp->epg_pf;

                            if(epg_p==NULL) continue;   
                              
                            if( epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer == NULL)
                              {                
                                total_bytes_recieved+= section_len;
  
                                epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer =  new uint8_t[section_len];  
                                memcpy (epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_buffer,data_ptr,section_len);

                        
                                epg_p->eit_tables[table].eit_segment[segment_num].section[segment_section].section_len = section_len;

                                epg_p->eit_tables[table].eit_segment[segment_num].usage++;

                                if(epg_p->eit_tables[table].eit_segment[segment_num].usage == (seg_last_sec_num%8)+1){

                                    epg_p->eit_tables[table].usage++;

                                        if(epg_p->eit_tables[table].usage == ((last_section_number%8))){
                                            epg_p->table_cnt++;
                                        }
                                    }

                                    if(epg_p->table_cnt==((last_table_id%16)+1))
                                    {
                             
                                        tmp_cnt++;
                                        if(tmp_cnt==service_cnt_with_epg)goto __exit;
                                    }

                                    break;
                                }
                            else
                              {
                                sbuf_epg_pf.continuities = 0;
                                sbuf_epg_pf.i = 0;
                                sbuf_epg_pf.section_status = 0;
                                sbuf_epg_pf.sz = 0;
                                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);

                                sbuf_epg_pf.used = 0;

                                if(sbuf_epg_pf.section_bufs!=NULL) section_buf_reset(sbuf_epg_pf.section_bufs);

                                epg_p = NULL;
                              }
                      }
              }
            
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);
                sbuf_epg_pf.used = 0;
                

             if(sbuf_epg_pf.section_bufs!=NULL)section_buf_reset(sbuf_epg_pf.section_bufs);
               
             epg_p = NULL;
         }
__exit:
                sbuf_epg_pf.continuities = 0;
                sbuf_epg_pf.i = 0;
                sbuf_epg_pf.section_status = 0;
                sbuf_epg_pf.sz = 0;
                memset(&sbuf_epg_pf.tsvals,0,sizeof(struct transport_values));
                sbuf_epg_pf.used = 0;
                memset(sbuf_epg_pf.databuf,0,DATA_BUF_SIZE);        

                if(sbuf_epg_pf.section_bufs!=NULL)
                {
                    section_buf_reset(sbuf_epg_pf.section_bufs);
                    free(sbuf_epg_pf.section_bufs);
                    sbuf_epg_pf.section_bufs = NULL;
                }

    tsi_remove_pid_from_filter(PID_EIT);
    
    EPG_DEBUG("total bytes recieved:%dkB\n",(total_bytes_recieved/1024));
    epg_pf_busy = false;
    return 0;
    

}


/**
 * @brief Główna funkcja pobierająca EPG
 * @param freq
 * @param ts_id
 * @param service_list_p
 * @return 0 - OK
 */
uint8_t eit::Download_Epg_Present_Following(uint32_t freq,uint16_t ts_id,dvb_service_list *service_list_p)
{
    uint8_t res;

    if(service_list_p==NULL) return 10;
     
    res = get_eit_present_following_for_services(freq,ts_id,service_list_p);
            
    if(res==0)
       {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

            EPG_DEBUG("POBRALEM DOBRZE EPG PRESENT/FOLLOWING....\n");

            if(!parse_eit_sections_for_pf(freq,ts_id,service_list_p))
            {
                EPG_DEBUG("SPARSOWALEM DOBRZE EPG....\n");
                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
                return 0;
            }
            else
            {
                EPG_DEBUG("BŁAD PARSOWANIA EPG....\n");
                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
                return 2;
            }
       }
       else
       {    
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

            free_all_eit_sections_pf(freq,ts_id,service_list_p);    
            
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            
            if(res==2) return 2;

            return 1; 
       }
}


/**
 * @brief Funkcja sprawdza czy zostało pobrane epg dla danej stacji(nie sprawdza aktualnosci epg!!!)
 * @param service_temp
 * @return true jesli epg jest dostepne dla stacji,false -nie dostepne lub wskaźnik jest pusty
 */
bool eit::EpgIsReadyForService(dvb_service *service_temp)
{
    uint8_t epg_file_name[255];
    FILE *fp = NULL;

    if(service_temp==NULL) return false;
    
    memset(epg_file_name,0,255);
    sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",service_temp->frequency,service_temp->service_id,service_temp->transport_stream_id);
    
    EPG_DEBUG("EPG FILE:%s\n",epg_file_name);
    
    fp = fopen((char*)epg_file_name,"r");
    
    if(fp==NULL) 
    {
        EPG_DEBUG("Can't open epg file...\n");
        return false;
    }
    
    fclose(fp);
    
    return true;
}


/**
 * 
 * @param service_temp
 * @return 
 */
bool eit::EpgIsActuallForService(dvb_service *service_temp)
{
    uint8_t epg_file_name[255];
    time_t time_now;
    uint32_t diff;
    struct stat buf;
    struct tm tm_file,tm_time_now;
    if(service_temp==NULL) return false;
    
  
    memset(epg_file_name,0,255);
    sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",service_temp->frequency,service_temp->service_id,service_temp->transport_stream_id);
   
    
    time_now = time(NULL);
    //printf("@@@@@@@@@ time_now:%s\n",ctime(&time_now));

    if(access((char*)epg_file_name,F_OK)!=-1)
    {
        int res=stat((char*)epg_file_name,&buf);
        
        if (res<0)
        {
           return false;
        }

        localtime_r(&buf.st_mtime, &tm_file);
        localtime_r(&time_now,&tm_time_now);

        if(tm_time_now.tm_wday == 0) tm_time_now.tm_wday = 7;
	if(tm_file.tm_wday == 0) tm_file.tm_wday = 7;

        diff = tm_time_now.tm_yday - tm_file.tm_yday;

       // printf("@@@@@@@@@ diff:%d %d   %d\n",diff,tm_time_now.tm_yday,tm_file.tm_yday);
        if(diff>0) {
           // printf("PLIK EPG NIE AKTUALNY...\n");
            return false;//przekroczyliśmy 7 dni - możemy nawet uaktualnić plik epg
        }

        return true;
    }
    else
    {
        EPG_DEBUG("BRAK PLIKU EPG...\n");
        return false;
    }
    
    
    return true;
}


int eit::time_to_epg_day(dvb_service * service_info)
{
    time_t time_now;
    int res,diff;
    struct stat buf;
    char epg_file[100];
    struct tm tm_file,tm_time_now;

    
    if(service_info==NULL) return false;
    
    
    time_now = time(NULL);

    memset(epg_file,0,255);
    sprintf((char*)epg_file,"%d_%d_%d.epg",service_info->frequency,service_info->service_id,service_info->transport_stream_id);
    

    if(access(epg_file,F_OK)!=-1)
    {
        res=stat(epg_file,&buf);
        if (res<0)
        {
           printf("Can not get the EPG file date!\n");
           return 0;
        }

        localtime_r(&buf.st_mtime, &tm_file);
        localtime_r(&time_now,&tm_time_now);
        
        if(tm_time_now.tm_wday == 0) tm_time_now.tm_wday = 7;
	if(tm_file.tm_wday == 0) tm_file.tm_wday = 7;

        diff = tm_time_now.tm_yday - tm_file.tm_yday;

        if(diff>6) return -1;//przekroczyliśmy 7 dni - możemy nawet uaktualnić plik epg

        return diff;
    }
    else
    {
        return -2;//brak pliku epg
    }
}