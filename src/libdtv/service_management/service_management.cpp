/** 
 * @file   service_management.cpp
 * @author Michał Motyl
 *
 * @date   30 czerwiec 2011, 11:56
 */


#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


#include "multiplex.h"
#include "struct_define.h"
#include "misc_func.h"
#include "service_management.h"
#include "dvb_srvice_pid_list.h"
using namespace std;


//dvb_service_list user_service_list;//!< Globalna lista z danymi stacji odczytanych z pliku podczas inicjalizacji TV

service_manager::service_manager()
{
    user_service_list = new dvb_service_list;
}


service_manager::~service_manager()
{
    Clean_Loaded_User_Service_Data();
    delete user_service_list;
}

//=======================================FUNKCJE PRYWATNE======================================================
/**
 * @brief Funkcja zapisuje do pliku stacje z listy.Zapisywanie nie powoduje duplikowania stacji
 * @param file - nazwa pliku do zapisu
 * @param p - multiplex z którego mają byc pobrane dane do zapisu
 * @return 
 */
uint8_t service_manager::save_services2file(const char *file,dvb_multiplex *p)
{
    uint16_t service_cnt = 0;
    uint16_t service_cnt_tmp = 0,services_added = 0;
    uint8_t skip = 0;
    dvb_service *service_tmp;
    dvb_service_list temp_list;
    FILE *service_file = NULL;
    size_t res;
    uint16_t pid_cnt;
    dvb_service_pid *pid_tmp;
    
    if(p==NULL) return 1;
    
    //najpierw prubujemy otworzyc plik normalnie
    service_file = fopen(file,"r+");
    
    if(service_file==NULL) 
    {
        //jezeli go nie ma to tworzymy nowy i zapisujemy początkową wartość ilości stacji na zero
        service_file = fopen(file,"wb+");
        
        if(service_file==NULL)  return 1;
        else
        res = fwrite(&service_cnt,sizeof(uint16_t),1,service_file);
    }
    else
    {
        //jezeli otworzyliśmy plik to odczytujemy ilość stacji w nim zapisanych
        res = fread(&service_cnt,sizeof(uint16_t),1,service_file);
    }
    
    service_cnt_tmp = p->service_list->get_list_count();
    
//    for(int z = 0;z<service_cnt_tmp;z++)
//    {
//        service_tmp = p->service_list.get_from_list(z);
//        printf("DBG: service name:%s\n",service_tmp->getServiceName());
//    }
    
    if(service_cnt_tmp==0) 
    {
        fclose(service_file);
        sync();
        return 1;
    }
    
    //jezeli już są jakies stacje to ładujemy je do listy tymczasowej z którą będziemy porównywać stacje do dodania;
    if(service_cnt>0) 
    {
        load_services_from_file(file,&temp_list,true);         
        fseek(service_file,0,SEEK_END);
    }
    
    for(int i = 0;i<service_cnt_tmp;i++)
    {
        service_tmp = p->service_list->get_from_list(i);
        
        //SERVICE_MANAGEMENT_DEBUG("temp_list.get_list_count():%d\n",temp_list.get_list_count());
        
        if(temp_list.get_list_count()>0)
        {
            for(unsigned int j = 0;j<temp_list.get_list_count();j++)
            {
                dvb_service *service_temp2 = temp_list.get_from_list(j);
                
                //szukamy na liście czy juz jest taka stacja
                if(service_temp2->transport_stream_id == service_tmp->transport_stream_id && service_temp2->service_id==service_tmp->service_id && service_temp2->frequency==service_tmp->frequency) 
                {
                    skip = 1;
                    break;
                }
                
            }
            
            //jeśli nie znaleźliśmy zduplikowanego wipsu to zapisz stację do pliku
            if(!skip)
            {
                res = fwrite(&service_tmp->visible,sizeof(uint8_t),1,service_file);
                
                strcpy((char*)service_tmp->network_name,(char*)p->network_name);
                res = fwrite(service_tmp->getNetworkName(),128,1,service_file);
                
                res = fwrite(service_tmp->getServiceName(),128,1,service_file);
                res = fwrite(service_tmp->getServiceProviderName(),128,1,service_file);
                res = fwrite(&service_tmp->service_type,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->eit_schedule,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->eit_present_following,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->free_CA_mode,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->service_id,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->pmt_pid,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->pcr_pid,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->transport_stream_id,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->frequency,sizeof(uint32_t),1,service_file);

                pid_cnt = service_tmp->service_pid_list.get_list_count();
                res = fwrite(&pid_cnt,sizeof(uint16_t),1,service_file);

                for(int j = 0;j<pid_cnt;j++)
                {
                    pid_tmp = service_tmp->service_pid_list.get_from_list(j);
                    res = fwrite(pid_tmp,sizeof(dvb_service_pid),1,service_file);
                }

                services_added++;     
            }
            
            skip = 0;
        }
        else
        {
                res = fwrite(&service_tmp->visible,sizeof(uint8_t),1,service_file);
                
                strcpy((char*)service_tmp->network_name,(char*)p->network_name);
                res = fwrite(service_tmp->network_name,128,1,service_file);
                
                res = fwrite(service_tmp->service_name,128,1,service_file);
                res = fwrite(service_tmp->service_provider_name,128,1,service_file);
                res = fwrite(&service_tmp->service_type,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->eit_schedule,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->eit_present_following,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->free_CA_mode,sizeof(uint8_t),1,service_file);
                res = fwrite(&service_tmp->service_id,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->pmt_pid,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->pcr_pid,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->transport_stream_id,sizeof(uint16_t),1,service_file);
                res = fwrite(&service_tmp->frequency,sizeof(uint32_t),1,service_file);

                pid_cnt = service_tmp->service_pid_list.get_list_count();
                res = fwrite(&pid_cnt,sizeof(uint16_t),1,service_file);

                for(int j = 0;j<pid_cnt;j++)
                {
                    pid_tmp = service_tmp->service_pid_list.get_from_list(j);
                    res = fwrite(pid_tmp,sizeof(dvb_service_pid),1,service_file);
                }

                services_added++;
        }
    }
//===========================================================================================================
    fseek(service_file,0,SEEK_SET);//ustawiamy sie na początek pliku żeby zaktualizować liczbę stacji w pliku
    
    SERVICE_MANAGEMENT_DEBUG("Liczba dodanych stacji:%d\n",services_added);
    
    service_cnt+=services_added;
    res = fwrite(&service_cnt,sizeof(uint16_t),1,service_file);
//===========================================================================================================
    //zwalniamy wszystkie rzeczy dla tymczasowj listy odczytanej z pliku
//    service_cnt = temp_list.get_list_count();
//    for(int i = 0;i<service_cnt;i++){
//        service_tmp = temp_list.get_from_list(i);
//        service_tmp->service_pid_list.destroy_list();
//    }
    temp_list.destroy_list();
    
    fclose(service_file);
    sync();
    
    return 0;
}


/**
 * @brief Funkcja odczytuje dane stacji z pliku
 * @param file - plik z którego mają być pobrane dane o stacjach
 * @param current_list - lista do której mają być załadowane stacje
 * @param skip_visible_check - wł/wył sprawdzania zmiennej visible
 * @return Zwraca 0 jesli wszystko OK
 */
uint8_t service_manager::load_services_from_file(const char *file,dvb_service_list *current_list,bool skip_visible_check)
{
    dvb_service *service_tmp;
    uint16_t service_cnt = 0;
    size_t res;
    FILE *service_file = NULL;
    uint16_t pid_cnt;
    dvb_service_pid *pid_tmp;
    
    if(current_list==NULL) return 1;
    
    service_file = fopen(file,"r");
    
    if(service_file==NULL) return 1;
    
    res = fread(&service_cnt,sizeof(uint16_t),1,service_file);
    
    if(service_cnt==0) 
    {
        fclose(service_file);
        sync();
        return 1;
    }
    
    for(int i = 0;i<service_cnt;i++)
    {
        service_tmp = create_new_service();
        
        res = fread(&service_tmp->visible,sizeof(uint8_t),1,service_file);
        res = fread(service_tmp->network_name,128,1,service_file);
        res = fread(service_tmp->service_name,128,1,service_file);
        res = fread(service_tmp->service_provider_name,128,1,service_file);
        res = fread(&service_tmp->service_type,sizeof(uint8_t),1,service_file);
        res = fread(&service_tmp->eit_schedule,sizeof(uint8_t),1,service_file);
        res = fread(&service_tmp->eit_present_following,sizeof(uint8_t),1,service_file);
        res = fread(&service_tmp->free_CA_mode,sizeof(uint8_t),1,service_file);
        res = fread(&service_tmp->service_id,sizeof(uint16_t),1,service_file);
        res = fread(&service_tmp->pmt_pid,sizeof(uint16_t),1,service_file);
        res = fread(&service_tmp->pcr_pid,sizeof(uint16_t),1,service_file);
        res = fread(&service_tmp->transport_stream_id,sizeof(uint16_t),1,service_file);
        res = fread(&service_tmp->frequency,sizeof(uint32_t),1,service_file);
        
        
       // printf("load_services_from_file:%s free_CA_mode:%d  service_type:%d\n",service_tmp->service_name,service_tmp->free_CA_mode,service_tmp->service_type);
        
        res = fread(&pid_cnt,sizeof(uint16_t),1,service_file);
        
        for(int j = 0;j<pid_cnt;j++)
        {
            pid_tmp = new dvb_service_pid;
            int rr;
            rr = fread(pid_tmp,sizeof(dvb_service_pid),1,service_file);
            service_tmp->service_pid_list.add_to_list(pid_tmp);
        }
        
        if(skip_visible_check==false)
        {
            if(service_tmp->visible==0)
            {
                delete service_tmp;
                service_tmp = NULL;
            }
            else
            {
#ifdef ENABLE_ALL
                current_list->add_to_list(service_tmp);
#else
                if(service_tmp->free_CA_mode==true)
                {
                    SERVICE_MANAGEMENT_DEBUG("#### Stacja %s jest nieobslugiwana - nie dodajemy do listy!!!! freq:%d\n",service_tmp->service_name,service_tmp->frequency);
                    delete service_tmp;
                    service_tmp = NULL;
                }
                else
                {
                    if( (Advanced_codec_HD_Television==service_tmp->service_type) || \
                        (Advanced_codec_SD_Television==service_tmp->service_type) || \
                        (Digital_Television_Service==service_tmp->service_type))
                    {
                        current_list->add_to_list(service_tmp);
                    }
                    else
                    {
                        SERVICE_MANAGEMENT_DEBUG("#### Stacja %s jest nieobslugiwana - nie dodajemy do listy service_type:0x%x!!!!\n",service_tmp->service_name,service_tmp->service_type);
                        delete service_tmp;
                        service_tmp = NULL;
                    }
                }
#endif
            }
        }
        else
        {
            
#ifdef ENABLE_ALL
                current_list->add_to_list(service_tmp);
#else
        if(service_tmp->free_CA_mode==true)
                {
                    SERVICE_MANAGEMENT_DEBUG("#### Stacja %s jest nieobslugiwana - nie dodajemy do listy!!!! freq:%d\n",service_tmp->service_name,service_tmp->frequency);
                    delete service_tmp;
                    service_tmp = NULL;
                }
                else
                {
                    if( (Advanced_codec_HD_Television==service_tmp->service_type) || \
                        (Advanced_codec_SD_Television==service_tmp->service_type) || \
                        (Digital_Television_Service==service_tmp->service_type))
                    {
                        current_list->add_to_list(service_tmp);
                    }
                    else
                    {
                        SERVICE_MANAGEMENT_DEBUG("#### Stacja %s jest nieobslugiwana - nie dodajemy do listy service_type:0x%x!!!!\n",service_tmp->service_name,service_tmp->service_type);
                        delete service_tmp;
                        service_tmp = NULL;
                    }
                }
#endif
        }
    }
    
    fclose(service_file);
    sync();
    
    return 0;
}




/**
 * @brief Funkcja wykorzystywana do aktualizacji listy kanałów po usunięciu kanału,zmianie jego pozycji na liście lub "odchaczenia"
 * @param file
 * @param p
 * @return Zwraca 0 jesli wszystko OK
 */
uint8_t service_manager::services_update(const char *file,dvb_service_list *p)
{
    uint16_t service_cnt = 0;

    dvb_service *service_tmp;
    FILE *service_file = NULL;
    size_t res;
    uint16_t pid_cnt;
    dvb_service_pid *pid_tmp;
    
    //usówamy plik
    remove(file);
    sync();
    //tworzymy nowy
    service_file = fopen(file,"wb+");
        
    if(service_file==NULL)  return 1;
   
    service_cnt = p->get_list_count();

    
    res = fwrite(&service_cnt,sizeof(uint16_t),1,service_file);
    
    
    for(int i = 0;i<service_cnt;i++)
    {
        service_tmp = p->get_from_list(i);
        
        res = fwrite(&service_tmp->visible,sizeof(uint8_t),1,service_file);
                
        res = fwrite(service_tmp->network_name,128,1,service_file);

        res = fwrite(service_tmp->service_name,128,1,service_file);
        res = fwrite(service_tmp->service_provider_name,128,1,service_file);
        res = fwrite(&service_tmp->service_type,sizeof(uint8_t),1,service_file);
        res = fwrite(&service_tmp->eit_schedule,sizeof(uint8_t),1,service_file);
        res = fwrite(&service_tmp->eit_present_following,sizeof(uint8_t),1,service_file);
        res = fwrite(&service_tmp->free_CA_mode,sizeof(uint8_t),1,service_file);
        res = fwrite(&service_tmp->service_id,sizeof(uint16_t),1,service_file);
        res = fwrite(&service_tmp->pmt_pid,sizeof(uint16_t),1,service_file);
        res = fwrite(&service_tmp->pcr_pid,sizeof(uint16_t),1,service_file);
        res = fwrite(&service_tmp->transport_stream_id,sizeof(uint16_t),1,service_file);
        res = fwrite(&service_tmp->frequency,sizeof(uint32_t),1,service_file);
        
        pid_cnt = service_tmp->service_pid_list.get_list_count();
        res = fwrite(&pid_cnt,sizeof(uint16_t),1,service_file);

        for(int j = 0;j<pid_cnt;j++)
        {
            pid_tmp = (dvb_service_pid*)service_tmp->service_pid_list.get_from_list(j);
            res = fwrite(pid_tmp,sizeof(dvb_service_pid),1,service_file);
        }

    }
    
    
    fclose(service_file);
    sync();
    

    return 0;
}


/**
 * @brief  Funkcja główna,ładuje(inicjuje) do listy informacje o stacjach,używana podczas wywołania funkcji DTV_Start
 * @return 0 - ok else error
 */
uint8_t service_manager::Init_services_data_for_user(bool skip_visible_check /* = 0 */)
{
    //if(user_service_list==NULL) return 1;
    
    return load_services_from_file(DVB_SERVICES_FILE,user_service_list,skip_visible_check);
}

/**
 * @brief Funkcja czyszcząca listę ze stacjami użytkownika podczas zamykania telewizji
 */
void service_manager::Clean_Loaded_User_Service_Data(void)
{
    //if(user_service_list==NULL) return;
    
//    SERVICE_MANAGEMENT_DEBUG("Usuwam liste z pamieci...\n");
    user_service_list->destroy_list();
    ////delete user_service_list;
    //user_service_list = NULL;
}
//=======================================FUNKCJE UŻYTKOWNIKA======================================================
/**
 * @brief Funkcja usuwa stację z listy,aktualizuje stacje w pliku i usuwa powiazany ze stacja plik epg
 * @param service_index
 * @return Zwraca 0 jesli wszystko OK
 */
uint8_t service_manager::Service_Delete(uint16_t service_index)
{
    //if(user_service_list==NULL) return 1;
    
    uint16_t services_cnt = user_service_list->get_list_count();
    uint8_t epg_file_name[255];
    dvb_service *s = DTV_Index_To_Service(service_index);
    
    if(s!=NULL)
    {
        memset(epg_file_name,0,255);
        sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",s->frequency,s->service_id,s->transport_stream_id);
        
        if(remove((char*)epg_file_name)!=0) SERVICE_MANAGEMENT_DEBUG("Blad usuwania pliku epg...\n");

        if(services_cnt==0 || service_index>=services_cnt)
        {
            SERVICE_MANAGEMENT_DEBUG("Lista pusta...\n");
            return 1;
        }
    
        user_service_list->remove_nth_node(service_index);
        return services_update(DVB_SERVICES_FILE,user_service_list);
    }
    
    return 1;
}

/**
 * @brief Funkcja usuwa stacje z listy zaladowanej do ram-u Uwaga!!! Nie uaktualnia pliku ze stacjami
 * @param service_index
 * @return 
 */
uint8_t service_manager::ServiceRemoveFromList(uint16_t service_index)
{
    //if(user_service_list==NULL) return 1;
    
    uint16_t services_cnt = user_service_list->get_list_count();
    uint8_t epg_file_name[255];
    dvb_service *s = DTV_Index_To_Service(service_index);
    
    if(s!=NULL)
    {
        memset(epg_file_name,0,255);
        sprintf((char*)epg_file_name,"/media/localdata/%d_%d_%d.epg",s->frequency,s->service_id,s->transport_stream_id);
        
        if(remove((char*)epg_file_name)!=0) SERVICE_MANAGEMENT_DEBUG("Blad usuwania pliku epg...\n");

        if(services_cnt==0 || service_index>=services_cnt)
        {
            SERVICE_MANAGEMENT_DEBUG("Lista pusta...\n");
            return 1;
        }
    
        user_service_list->remove_nth_node(service_index);
        return 0;
    }
    
    return 1;
}


/**
 * @brief Funkcja pobiera dane stacji z okreslonego indeksu
 * @param index
 * @return Zwraca wskaźnik do kanału TV z listy
 */
dvb_service * service_manager::DTV_Index_To_Service(uint16_t index)
{
  //  if(user_service_list==NULL) return NULL;
    
    if(user_service_list->get_list_count()==0 || index>user_service_list->get_list_count()) return NULL;
    
    return (dvb_service *)user_service_list->get_from_list(index);
}



/**
 * @brief Funkcja ustawia lub kasuje zmienna visible dla określonej stacji aby nie była widoczna na liscie z programami podczas po inicjalizacji biblioteki
 * @param service_index
 * @param enable - czy wyłaczyć kanał
 * @return 0 - jeśli OK
 */
uint8_t service_manager::DTV_Service_enable_disable(uint16_t service_index,bool enable)
{
   // if(user_service_list==NULL) return 1;
    
    dvb_service *service_tmp = NULL;
    uint16_t services_cnt = user_service_list->get_list_count();
    
    if(services_cnt==0 || service_index>=services_cnt)
    {
        SERVICE_MANAGEMENT_DEBUG("Lista pusta lub index poza zakresem\n");
        return 1;
    }
    
    service_tmp = DTV_Index_To_Service(service_index);
    
    if(service_tmp==NULL) return 1;
    
    if(enable==true)service_tmp->visible = 1;
    else
        service_tmp->visible = 0;
    
 
    return services_update(DVB_SERVICES_FILE,user_service_list);
}



/**
 * @brief Funkcja przesówa wpis na liście o 1 raz w dół
 * @param service_index
 * @return 0 - jeśli OK
 */
uint8_t service_manager::DTV_Service_Movedown(uint16_t service_index)
{
//    if(user_service_list==NULL) return 1;
    
    uint16_t services_cnt = user_service_list->get_list_count();
    
    if(services_cnt==0)
    {
        SERVICE_MANAGEMENT_DEBUG("Lista pusta...\n");
        return 1;
    }
    
    if(user_service_list->move_down(service_index)==0) return services_update(DVB_SERVICES_FILE,user_service_list);
    
    return 1;
}


/**
 * @brief Funkcja przesówa wpis na liście o 1 raz w górę
 * @param service_index
 * @return 0 - jeśli OK
 */
uint8_t service_manager::DTV_Service_Moveup(uint16_t service_index)
{
//    if(user_service_list==NULL) return 1;
    
    uint16_t services_cnt = user_service_list->get_list_count();
    
    if(services_cnt==0)
    {
        SERVICE_MANAGEMENT_DEBUG("Lista pusta...\n");
        return 1;
    }
    
    if(user_service_list->move_up(service_index)==0) return services_update(DVB_SERVICES_FILE,user_service_list);
    
    return 1;
}


/**
 * @brief Funkcja szuka stację na liscie
 * @param service_id
 * @param f
 * @return Zwraca wskaźnik do stacji
 */
dvb_service* service_manager::DTV_Search_Service(uint16_t service_id, unsigned int f)
{
//    if(user_service_list==NULL) return NULL;
    
    dvb_service *temp = NULL;
    uint16_t services_cnt = user_service_list->get_list_count();
    
    if(services_cnt==0)
    {
        SERVICE_MANAGEMENT_DEBUG("Lista pusta...\n");
        return NULL;
    }
    
    for(int i = 0;i<services_cnt;i++)
    {
        temp = DTV_Index_To_Service(i);
        
        if(temp!=NULL)
        {
            if(temp->service_id==service_id && temp->frequency==f) return temp;
        }
    }
	
	return temp;
}


uint16_t service_manager::get_services_count()
{
    if(user_service_list==NULL) return 0;
    
    return user_service_list->get_list_count();
}
