/** 
 * @file   file_browser.cpp
 * @author michalm
 *
 * @date   17 maj 2012, 11:32
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>
#include <stddef.h>
#include <string>
#include <queue>
#include <list>
#include <termios.h>

#include <string>
#include <iostream>
#include <syslog.h>
#include <iostream>

#include <string.h>
#include "file_browser.h"
//#include "itdb.h"


//`pkg-config --cflags glib-2.0`
//`pkg-config --libs glib-2.0`
//-lgpod

file_browser::file_browser(QObject* parent) : QObject(parent) 
{
    file_counter = 0;
    dir_counter = 0;
    item_counter = 0;
    ipod_mount_point = 0;
    supported_formats_cnt = 0;

    src_check_thrd_init = false;
    ipod_flag = false;
    sd1_last_flag = false;
    sd2_last_flag = false;
    usb1_last_flag = false;
    usb2_last_flag = false;
    
    #ifdef IPOD_SUPPORT
    itdb = NULL;
    #endif


    supported_formats_tab = NULL;
    artist_list = NULL;
    unknown_artist_list_tracks = NULL;
    artist_list = NULL;
    album_list = NULL;
    composer_list = NULL;
    
    add_supported_extension(".mp3");
    add_supported_extension(".wav");
    add_supported_extension(".avi");
    add_supported_extension(".mp4");
    add_supported_extension(".3gp");
    add_supported_extension(".3g2");
    add_supported_extension(".mov");
    add_supported_extension(".mpeg");
    add_supported_extension(".m2ts");

    
    //set_base_path ("/media/usb1/");
    
    srcCheck = new QTimer(this);
    srcCheck->setInterval(200);
    connect(srcCheck, SIGNAL(timeout()),this,SLOT(media_timerEvent()),Qt::QueuedConnection);
    srcCheck->stop();

    //refresh_file_list (NULL);
 //   printf("%s %d #####\n",__FUNCTION__,__LINE__);
    select_source(SOURCE_USB1);
}


file_browser::~file_browser() 
{
    
    
    if(supported_formats_tab!=NULL) delete[] supported_formats_tab;

    file_list.destroy_list();
    
    if(ipod_flag==true)
    {
        
#ifdef IPOD_SUPPORT
        if(itdb!=NULL) 
        {
            itdb_free (itdb);
            itdb = NULL;
        }
#endif        
        
    }
    
    if(artist_list!=NULL)
    {
        g_slist_free(artist_list);
        artist_list = NULL;
    }
    
    if(unknown_artist_list_tracks!=NULL)
    {
        g_slist_free(unknown_artist_list_tracks);
        unknown_artist_list_tracks = NULL;
    }

}


/**
 * @brief Funkcja zwaraca nazwe pliku z listy plików audio
 * @param index
 * @return NULL jesli blad
 */
uint8_t * file_browser::get_file_name_by_index (uint32_t index)
{
uint16_t item_cnt = 0;
      
if(item_counter==0) return NULL;

    if(index<0) return NULL;


    if(index>file_counter) return NULL;

    item_cnt = file_list.get_list_count();

    if(item_cnt>0) return file_list.get_from_list((dir_counter+index))->item_name;

  return NULL;
}

/**
 * @brief Funkcja zwaraca nazwe elememntu z ogólnej listy plików i katalogów
 * @param index
 * @return NULL jesli blad
 */
//uint8_t * file_browser::get_item (uint32_t index)
//{
//uint16_t item_cnt = 0;
//        
//    if(item_counter==0) return NULL;
//
//    if(index>item_counter) return NULL;
//
//    item_cnt = file_list.get_list_count();
//    
//    if(item_cnt>0) 
//    { 
//        return file_list.get_from_list(index)->item_name;   
//    }
//
//  return NULL;
//}


browser_item * file_browser::get_item (uint32_t index)
{
uint16_t item_cnt = 0;
        
    if(item_counter==0) return NULL;

    if(index>item_counter) return NULL;

    item_cnt = file_list.get_list_count();
    
    if(item_cnt>0) 
    { 
        return file_list.get_from_list(index);   
    }

  return NULL;
}

/**
 * @brief Funkcja zwaraca nazwe elememntu z listy katalogów
 * @param index
 * @return NULL jesli blad
 */
uint8_t * file_browser::get_dir (uint32_t index)
{
uint16_t item_cnt = 0;

    if(index>dir_counter) return NULL;

    item_cnt = file_list.get_list_count();

    if(dir_counter>0) return file_list.get_from_list(index+1)->item_name;

  return NULL;
}



/**
 * @brief Funkcja dodaje rozszeżenie do obslugiwanych formatów audio
 * @param extension
 * @return false jezeli bląd, true - OK
 */
bool file_browser::add_supported_extension(const char *extension)
{
  string *tmp = NULL;

  if(extension==NULL) return false;

  if(extension[0]!= '.') return false;

  if(supported_formats_cnt==0)
    {
      supported_formats_tab = new string[1];
      supported_formats_tab[0] = extension;
      supported_formats_cnt++;
      return true;
    }
  else
    {
      if(supported_formats_tab!=NULL)
        {

          tmp = new string[supported_formats_cnt];

          for(unsigned int i = 0;i<supported_formats_cnt;i++)
            {
              tmp[i] = supported_formats_tab[i];
            }

          delete[] supported_formats_tab;
          supported_formats_tab = NULL;
        }
      supported_formats_cnt++;
      supported_formats_tab = new string[supported_formats_cnt];

      for(uint8_t i = 0;i<(supported_formats_cnt-1);i++)
        {
          supported_formats_tab[i] = tmp[i];
        }

      supported_formats_tab[(supported_formats_cnt-1)] = extension;
      delete[] tmp;
      return true;

    }

}

/*
 
     std::string path;
    path.clear();
    
    if(P!=NULL)
    {
        path.append(P);
        current_path.clear();
        current_path.append(P);
    }
    else
    path.append(base_path);
 
 */
/**
 * @brief Funkcja wewnętrzna odzytująca listę plików i katalogów
 * @return Zwraca < 0 - błąd
 */
int file_browser::refresh_file_list (char *P)
{
    string s1 = "",s2 = "",filename,temp;
    struct stat file_info;
    struct dirent **nameList,*temp_dirent;
    int numberOfFiles;
    int z = 0;
    browser_item *item = NULL;
    std::string path;
    path.clear();
    
    if(P!=NULL)
    {
        path.append(P);
        current_path.clear();
        current_path.append(P);
    }
    else
    path.append(base_path);
    
    item_counter = 0;
    file_counter = 0;
    dir_counter = 0;

    file_list.destroy_list();
    if(access(path.c_str(),F_OK)!=0){

        empty_dir_flag = true;
        return -1;
    }
   
    temp = path;
    
    numberOfFiles = scandir(path.c_str (), &nameList, 0, 0);
    empty_dir_flag = false;

  if(numberOfFiles==0 || numberOfFiles==-1)
    {
	empty_dir_flag = true;
	numberOfFiles = 0;
    }
  else
    {
	 //pierwszy wpis na liscie - powrót do katalogu wyżej tylko gdy nie jesteśmy w katalogu głównym
      //printf("%s  %s\n",current_path.c_str(),start_path.c_str());
      
      if(this->current_path.compare (this->start_path))
      {
	item = new browser_item;
                
            if(item!=NULL)
            {
                memset(item,0,sizeof(browser_item));
                item->item_type = 0;
                item->pos_on_list = item_counter;
                item->item_name = new uint8_t[5];
                memset(item->item_name,0,5);
                strcpy((char*)item->item_name,"..");
    
                file_list.add_to_list(item);
                item_counter++;
                //file_counter++;
                dir_counter++;
    
            }
      }

      
	
	//file_list = new string[numberOfFiles];

	        int x,y;

		//sortujemy - najpierw katalogi ,poźniej pliki
                for(x = 0; x < numberOfFiles; x++)
                {
                    for(y = 0; y < (numberOfFiles-1); y++)
                    {
                          if(nameList[y]->d_type > nameList[y+1]->d_type)
                          {
                                temp_dirent = nameList[y+1];
                                nameList[y+1] = nameList[y];
                                nameList[y] = temp_dirent;
                          }
                    }
                }
    }
//==============================================================
 
    //sortujemy alfabetycznie katalogi i pliki
    for(z = 0;z<numberOfFiles;z++)
      {
	   if(nameList[z]->d_type == DT_REG) break;//początek plikow w tablicy(tablica jest posortowana -najpierw katalogi pozniek pliki)
      }

        int x,y;
    int Value_x,Value_y;
    bool IsLess=false;
        //najpierw sortujmey pliki
	    for(x = z; x < numberOfFiles; x++)
                {
                for(y = x+1; y <numberOfFiles; y++)
                    {
			
                          int Size_x=strlen(nameList[x]->d_name);
                          int Size_y=strlen(nameList[y]->d_name);
                          int Size=0;
                          if(Size_x<Size_y)
                          Size=Size_x;
                          else
                          Size=Size_y;                 
                          for(int k=0;k<Size;k++)
                          {
                                Value_x=(int)nameList[x]->d_name[k];
                                Value_y=(int)nameList[y]->d_name[k];
                  
                                //uppercase letters, lowercase letters 
                                if(Value_x>=97)
                                Value_x=Value_x-32;
                                if(Value_y>=97)
                                Value_y=Value_y-32;
                  
                                     //numbers in name 
                                if(Value_x>47 && Value_x<58 && Value_y>47 && Value_y<58)
                                {
                                int Number_x=0;
                                int Number_y=0;
                                int q=0;
                                int q2=0;
     
                                
                                while(((Value_x>47 && Value_x<58) && q+k<Size_x))
                                {
                                  if(q==0)
                                  {
                                        Number_x=(Value_x-48);
                                  }
                                  else 
                                  {
                                      Number_x=(Number_x*10)+(Value_x-48);
                                  }
                                  q++;
                                  if(q+k<Size_x)
                                  Value_x=(int)(int)nameList[x]->d_name[k+q];
                                }
                                Value_x=Number_x;
                                
                               while(((Value_y>47 && Value_y<58) && q2+k<Size_y))
                                {
                                  if(q2==0)
                                  {
                                        Number_y=(Value_y-48);
                                  }
                                  else 
                                  {
                                      Number_y=(Number_y*10)+(Value_y-48);
                                  }
                                  q2++;
                                  if(q2+k<Size_y)
                                  Value_y=(int)(int)nameList[y]->d_name[k+q2];
                                }
                                Value_y=Number_y;
                                
                                if(q<q2)
                                {
                                    k=k+q-1;
                                }
                                else
                                {
                                    k=k+q2-1;
                                }
                    
                           }
                      
                  if(Value_y<Value_x)
                  {
                      IsLess=true;
                      break;
                  }
                  if(Value_y>Value_x)
                  {
                      break;
                  }
                  
              }
              
              if(IsLess)
              {
                     temp_dirent = nameList[y];
                    nameList[y] = nameList[x];
                    nameList[x] = temp_dirent;

                  IsLess=false;
              } 
         }
       }
    
    
    //sortujemy katalogi
    for(x = 0; x < z; x++)
                {         
                    for(y = x+1; y <z; y++)
                    {

                          int Size_x=strlen(nameList[x]->d_name);
                          int Size_y=strlen(nameList[y]->d_name);
                          int Size=0;
                          if(Size_x<Size_y)
                          Size=Size_x;
                          else
                          Size=Size_y;                 
                          for(int k=0;k<Size;k++)
                          {
                                Value_x=(int)nameList[x]->d_name[k];
                                Value_y=(int)nameList[y]->d_name[k];
                  
                                //uppercase letters, lowercase letters 
                                if(Value_x>=97)
                                Value_x=Value_x-32;
                                if(Value_y>=97)
                                Value_y=Value_y-32;
                                
                                //numbers in name 
                                if(Value_x>47 && Value_x<58 && Value_y>47 && Value_y<58)
                                {
                                int Number_x=0;
                                int Number_y=0;
                                int q=0;
                                int q2=0;
            
                                
                                while(((Value_x>47 && Value_x<58) && q+k<Size_x))
                                {
                                  if(q==0)
                                  {
                                        Number_x=(Value_x-48);
                                  }
                                  else 
                                  {
                                      Number_x=(Number_x*10)+(Value_x-48);
                                  }
                                  q++;
                                  if(q+k<Size_x)
                                  Value_x=(int)(int)nameList[x]->d_name[k+q];
                                }
                                Value_x=Number_x;
                                
                               while(((Value_y>47 && Value_y<58) && q2+k<Size_y))
                                {
                                  if(q2==0)
                                  {
                                        Number_y=(Value_y-48);
                                  }
                                  else 
                                  {
                                      Number_y=(Number_y*10)+(Value_y-48);
                                  }
                                  q2++;
                                  if(q2+k<Size_y)
                                  Value_y=(int)(int)nameList[y]->d_name[k+q2];
                                }
                                Value_y=Number_y;
                                
                                if(q<q2)
                                {
                                    k=k+q-1;
                                }
                                else
                                {
                                    k=k+q2-1;
                                }
                             #ifdef DebugowanieSortFiles_Arkadiusz
                             cout<<"\tKATALOGI X "<<Value_x<<" "<<nameList[x]->d_name<<" Y "<<Value_y<<" "<<nameList[y]->d_name<<endl;
                             #endif
                           }
                      
                  if(Value_y<Value_x)
                  {
                      IsLess=true;
                      break;
                  }
                  if(Value_y>Value_x)
                  {
                      break;
                  }
                  
              }
              
              if(IsLess)
              {
                     temp_dirent = nameList[y];
                    nameList[y] = nameList[x];
                    nameList[x] = temp_dirent;

                  IsLess=false;
              } 
                    }
          }
    
//    int x,y;
//    //najpierw sortujmey pliki
//	    for(x = z; x < numberOfFiles; x++)
//                {
//                    for(y = z; y < (numberOfFiles-1); y++)
//                    {
//			
//			if(strcmp (nameList[y]->d_name,nameList[y+1]->d_name)>0)
//                          {
//                                temp_dirent = nameList[y+1];
//                                nameList[y+1] = nameList[y];
//                                nameList[y] = temp_dirent;
//                          }
//                    }
//                }
//
//		//sortujemy katalogi
//	    for(x = 0; x < z; x++)
//                {
//                    for(y = 0; y < (z-1); y++)
//                    {
//
//			if(strcmp (nameList[y]->d_name,nameList[y+1]->d_name)>0)
//                          {
//                                temp_dirent = nameList[y+1];
//                                nameList[y+1] = nameList[y];
//                                nameList[y] = temp_dirent;
//                          }
//                    }
//                }
//==============================================================
  for (int i = 0; i < numberOfFiles; i++)
    {

      filename = nameList[i]->d_name;
      	s1 = nameList[i]->d_name;
	s2 = ".";

	  if(!s1.compare (s2))
          {
              continue;
          }

	s2 = "..";

	    if(!s1.compare (s2))
            {
                continue;
            }
        
        if(nameList[i]->d_name[0]==0x2e) continue;//nie dodawaj plikow ukrytych,zaczynających sie od .

      if(nameList[i]->d_type== DT_REG/*0x08*/)
	{
	    //sprawdź rozszeżenie pliku
            if(check_supported_extension(nameList[i]->d_name)==false)
            {
                //plik nie ma rozszezenia
            //    DBG_PUTSTR("Not supported file...\n");
                continue;
            }
	    
	    temp.append (filename);
	    
	    int res = stat(temp.c_str (),&file_info);

	    if(!res)
	      {
	        //file_list[j].append (nameList[i]->d_name);
                item = new browser_item;
                
                if(item!=NULL)
                {
                    memset(item,0,sizeof(browser_item));
                    item->item_type = 1;
                    item->size = (file_info.st_size/1024);
                    
                    item->item_name = new uint8_t[filename.length()+1];
                    memset(item->item_name,0,(filename.length()+1));
                    strcpy((char*)item->item_name,filename.c_str());
                    item->pos_on_list = item_counter;
                    file_list.add_to_list(item);
                    file_counter++;
                    item_counter++;
                   
                }
                item = NULL;
	      }
	   temp = path;

	}
      else
        if(nameList[i]->d_type==DT_DIR)
	{
            item = new browser_item;
                
            if(item!=NULL)
            {
                memset(item,0,sizeof(browser_item));
                item->item_type = 0;

                item->item_name = new uint8_t[filename.length()+1];
                memset(item->item_name,0,(filename.length()+1));
                strcpy((char*)item->item_name,filename.c_str());
                item->pos_on_list = item_counter;
                
                file_list.add_to_list(item);
                item_counter++;
                dir_counter++;
      
            }
            
                item = NULL;
	}

    }


     int d = 0;

     for(d = 0;d<numberOfFiles;d++)
         free(nameList[d]);


     free(nameList);
     nameList = NULL;

     return 0;
}


/**
 * @brief Funkcja wewnerzna sprawdzająca obslugiwane rozszażenie
 * @param file_name
 * @return true - obsluguje,false-nie
 */
bool file_browser::check_supported_extension(char *file_name)
{
  char * pch = NULL;

  pch=strrchr(file_name,'.');

  if(pch!=NULL)
  {
    for(unsigned int i = 0;i<supported_formats_cnt;i++)
      {
        if(!strcmp((pch),supported_formats_tab[i].c_str())) return true;
      }
  }

    return false;
}


void file_browser::set_base_path (string path)
{
    base_path = path;
    current_path = base_path;
    start_path = base_path;
    refresh_file_list (NULL);
}


/**
 * @brief Funkcja slużaca do chodzenia po plikach i katalogach
 * @param idx - indeks elementu na liscie który chcemy wybrać.Jezeli zostanie wybrany katalog,to nastąpi wejscie w niego i odświerzenie listy plików. Wybranie elementu z indeksem zero powoduje wyjście o katalog wyżej.
 * @return true -jesli OK
 */
bool file_browser::select_item(uint16_t idx)
{
browser_item *it;
char *pch = NULL;
char *str = NULL;
  
     if(item_counter==0) return false;
        
     if(idx>item_counter) return false;
  
    it = file_list.get_from_list(idx);
    
    if(it->item_type==1)
    {//plik
	execute_user_cb_on_file_select(it->item_name);
    }
    else
    {//katalog
        if(!strcmp ((char*)it->item_name,".."))
	{
	      //===========================================================
	    int len = 0;
	    string cpy  = current_path;

	    cpy.erase ((cpy.end ()-1));
	    len = cpy.length ();
	    str = new char[len+1];

	    len = cpy.length ();

	    strcpy (str,cpy.data ());
	    pch=strrchr(str,'/');
	    int dif = pch-str;

	    cpy.erase ((cpy.end ()-1));

	    memset(&str[dif],0,(len-dif-1));

	    current_path.clear ();
	    current_path = str;
	    current_path.append ("/");

	    if(str!=NULL) {

		delete[] str;
		str=NULL;
	      }

            base_path = current_path;
            refresh_file_list (NULL);
        }
        else
        {
         //   current_dir.clear();
         //   current_dir.append((char*)it->item_name);
            current_path.append ((char*)it->item_name);
            current_path.append ("/");
            base_path = current_path;
            refresh_file_list (NULL);
            execute_user_cb_on_dir_select();
        }
    }
  
    return true;
    
}


void file_browser::execute_user_cb_on_dir_select(void)
{
    emit sigDirSelect();
}

void file_browser::execute_user_cb_on_file_select(uint8_t *file_name)
{
  FILE * fp = NULL;
  string temp = (char*)file_name;
  
    fp = fopen("/media/localdata/last_file.dat","wb+");

    if(fp!=NULL){

        fprintf(fp,"%s\n",file_name);
        fclose(fp);
        sync();
    }
    else
    {
        FILE_BROWSER_DEBUG("Blad zapisu do last_file.dat!!!\n");
    }
    
    fp = fopen("/media/localdata/last_dir.dat","wb+");

    if(fp!=NULL){

        fprintf(fp,"%s\n",current_path.c_str());
        fclose(fp);

        sync();
    }
    else
    {
        FILE_BROWSER_DEBUG("Blad zapisu do last_dir.dat!!!\n");
    }

      emit sigFileSelect(current_path + temp);
    
}

void file_browser::execute_user_cb_on_media_remove(uint8_t src_id)
{

        emit sigMediaRemove(src_id);
}
//
//
void file_browser::execute_user_cb_on_media_insert(uint8_t src_id)
{
  emit sigMediaInsert(src_id);
}


///**
// *
// * @param f
// */
//void file_browser::register_user_dir_select_cb(void (*f)(void))
//{
//  on_dir_select_cb = f;
//}
//
///**
// * @brief Funkcja rejestruje callbac uzytkownika który jest wywoływany przy wybraniu pliku z listy
// * @param f
// */
//void file_browser::register_user_file_select_cb(void(*f)(string file_name))
//{
//  on_file_select_cb = f;
//}
//
//
///**
// * @brief Funkcja rejestruje callbac uzytkownika który jest wywoływany prze wyjęciu jakiegoś nośnika
// * @param f
// */
//void file_browser::register_user_on_media_remove_cb(void(*f)(uint8_t src_id,void *p),void *up)
//{
//  on_media_remove_cb = f;
//  on_media_remove_cb_user_data = up;
//}
//
///**
// * @brief Funkcja rejestruje callbac uzytkownika który jest wywoływany prze włożeniu jakiegoś nośnika
// * @param f
// */
//void file_browser::register_user_on_media_insert_cb(void(*f)(uint8_t src_id,void *p),void *up)
//{
//  on_media_insert_cb = f;
//  on_media_insert_cb_user_data = up;
//}


///**
// * @brief Funkcja zwraca absolutną ścieżkę pliku z listy
// * @param idx
// * @return
// */
//uint8_t* file_browser::get_file_absolute_path(uint16_t idx)
//{
//    string temp;
//    
//    temp.clear();
//    
//    if(get_file_name_by_index(idx)!=NULL){
//    
//        temp.append(current_path);
//        temp.append((char*)get_file_name_by_index(idx));
//        return (uint8_t*)temp.c_str();
//    }
//    
//    return NULL;
//}


/**
 * @brief Funkcja sprawdza czy śą wlożone jakies karty lub urządzenia USB i odswierza listę plików
 */
void file_browser::check_source_status ()
{
  char buf[10];
  memset (&buf,0,10);
  FILE *fd;

  if(!access ("/tmp/sd1_card_detect",F_OK))
    {
        fd = fopen("/tmp/sd1_card_detect","r");
        fgets (buf,10,fd);
        sd1_in_flag = (bool)atoi (buf);
        fclose(fd);
    }
  else
  {
      sd1_last_flag = false;
      sd1_in_flag = false;
  }


  memset (&buf,0,10);

  if(!access ("/tmp/sd2_card_detect",F_OK))
    {
        fd = fopen("/tmp/sd2_card_detect","r");
        fgets (buf,10,fd);
        sd2_in_flag = (bool)atoi (buf);
        fclose(fd);  
    }
  else
  {
      sd2_last_flag = false;
      sd2_in_flag = false;
  }


  memset (&buf,0,10);

  if(!access ("/tmp/usb1_detect",F_OK))
    {
        fd = fopen("/tmp/usb1_detect","r");
        fgets (buf,10,fd);
        usb1_in_flag = (bool)atoi (buf);
        fclose(fd);
    }
  else
    {
      usb1_last_flag = false;
      usb1_in_flag = false;
    }


  memset (&buf,0,10);

  if(!access ("/tmp/usb2_detect",F_OK))
    {
        fd = fopen("/tmp/usb2_detect","r");
        fgets (buf,10,fd);
        usb2_in_flag = (bool)atoi (buf);
        fclose(fd);
    }
  else
    {
      usb2_last_flag = false;
      usb2_in_flag = false;
    }
  
 
        if(sd1_in_flag==true && sd1_last_flag==false)
        {
            sd1_last_flag = sd1_in_flag;
//            select_source(current_source);
//            refresh_file_list();
            execute_user_cb_on_media_insert(SOURCE_SD1);
            return;
        }
        else
        {
            if(sd1_in_flag==false && sd1_last_flag==true)
            {
                sd1_last_flag = sd1_in_flag;
                execute_user_cb_on_media_remove(SOURCE_SD1);

                system("umount -f /media/sd1/");
//                select_source(current_source);
//                refresh_file_list();
                return;
            }
        }
  
  
  if(sd2_in_flag==true && sd2_last_flag==false)
        {
            sd2_last_flag = sd2_in_flag;
          //  select_source(current_source);
          //  refresh_file_list();
            execute_user_cb_on_media_insert(SOURCE_SD2);
            return;
        }
        else
        {
            if(sd2_in_flag==false && sd2_last_flag==true)
            {
                sd2_last_flag = sd2_in_flag;

                    execute_user_cb_on_media_remove(SOURCE_SD2);
                    system("umount -f /media/sd2/");
              //      select_source(current_source);
              //      refresh_file_list();
                    return;
            }
        }
  

  if(usb1_in_flag==true && usb1_last_flag==false)
        {
            usb1_last_flag = usb1_in_flag;
//            select_source(current_source);
//            refresh_file_list(NULL);

            execute_user_cb_on_media_insert(SOURCE_USB1);
            return;
        }
        else
        {
            if(usb1_in_flag==false && usb1_last_flag==true)
            {
                usb1_last_flag = usb1_in_flag;

                execute_user_cb_on_media_remove(SOURCE_USB1);
                system("umount -f /media/usb1/");
//                select_source(current_source);
//                refresh_file_list();
                
                if(ipod_flag==true && ipod_mount_point==SOURCE_USB1)
                {
#ifdef IPOD_SUPPORT
                    if(itdb!=NULL) 
                    {
                        itdb_free (itdb);
                        itdb = NULL;
                    }
#endif
                    ipod_mount_point = 0;
                }
                return;
            }
        }
  
  if(usb2_in_flag==true && usb2_last_flag==false)
        {
            usb2_last_flag = usb2_in_flag;
            printf("%s %d #####\n",__FUNCTION__,__LINE__);
            select_source(current_source);
        //    refresh_file_list();
            execute_user_cb_on_media_insert(SOURCE_USB2);
            return;
        }
        else
        {
            if(usb2_in_flag==false && usb2_last_flag==true)
            {
                usb2_last_flag = usb2_in_flag;

                execute_user_cb_on_media_remove(SOURCE_USB2);
                system("umount -f /media/usb2/");
         //       select_source(current_source);
         //       refresh_file_list();
                
                if(ipod_flag==true && ipod_mount_point==SOURCE_USB2)
                {
                   
#ifdef IPOD_SUPPORT
                    if(itdb!=NULL) 
                    {
                        itdb_free (itdb);
                        itdb = NULL;
                    }
#endif                    
                    ipod_mount_point = 0;
                }
                return;
            }
        }
  
  return;

}


///**
// * @brief Wątek sprawdzający włozone nośniki i dostępność IPOD'a w któryms z usb
// * @param p - wksaźnik na klasę file_browser
// * @return 
// */
//void *file_browser::media_check_thread(void* p)
//{
//    file_browser *parent = ((file_browser*)p);
//    
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    
//    while(1)
//    {
//       parent->check_source_status();    
//       
//#ifdef IPOD_SUPPORT
//       parent->check4ipod();
//#endif
//       usleep(250000);
//    }
//    
//    return NULL;
//}


/**
 * @brief Funkcja wybiera źródło
 * @param src - SD1,SD2,USB1,USB2 src = {0,1,2,3} \
 *              #define SOURCE_SD1      0
 *              #define SOURCE_SD2      1
 *              #define SOURCE_USB1     2
 *              #define SOURCE_USB2     3
 *              
 * @return 
 */
bool file_browser::select_source(uint8_t src)
{
    int res = 0;
    
    
 //   printf("select_source:%d\n",src);
    if(src==SOURCE_SD1)
    {
        set_base_path ("/media/sd1/");

        res = refresh_file_list (NULL);
        
        if(res<0) FILE_BROWSER_DEBUG("SD1 refresh error...\n");
        
        current_source = SOURCE_SD1;
        return true;
    }
    
    if(src==SOURCE_SD2)
    {
        set_base_path ("/media/sd2/");


        res = refresh_file_list (NULL);
        
        if(res<0) FILE_BROWSER_DEBUG("SD2 refresh error...\n");
        current_source = SOURCE_SD2;
        return true;
    }
    
    if(src==SOURCE_USB1)
    {
        set_base_path ("/media/usb1/");


        res = refresh_file_list (NULL);
        
        if(res<0) FILE_BROWSER_DEBUG("USB1 refresh error...\n");
        current_source = SOURCE_USB1;
        return true;
    }
    
    if(src==SOURCE_USB2)
    {
        set_base_path ("/media/usb2/");
   

        res = refresh_file_list (NULL);
        
        if(res<0) FILE_BROWSER_DEBUG("USB2 refresh error...\n");
        current_source = SOURCE_USB2;
        return true;
    }
   
    return false;
}

/**
 * @brief Funkcja wyjścia z katalogu o poziom wyżej
 * @return 
 */
bool file_browser::go_back(void)
{
    return select_item(0);
}


/**
 * @brief Funkcja sprawdza dostępność IPOD'a w któryms z gniazd USB
 */

#ifdef IPOD_SUPPORT
void file_browser::check4ipod(void)
{
    GError *error = NULL;

    if(itdb==NULL)
    {
        if(usb1_in_flag==1)
        {
            itdb = itdb_parse ("/media/usb1", &error);

            if (error)
            {
                if (error->message) {
                    g_print("%s\n", error->message);
                }
                g_error_free (error);
                error = NULL;
            }
            ipod_flag = true;
            ipod_mount_point = SOURCE_USB1;
            FILE_BROWSER_DEBUG("Znalazlem IPOD'a ...\n");
            return;
        }

        if(usb2_in_flag==1)
        {
            itdb = itdb_parse ("/media/usb2", &error);

            if (error)
            {
                if (error->message) {
                    g_print("%s\n", error->message);
                }
                g_error_free (error);
                error = NULL;
            }
            ipod_flag = true;
            ipod_mount_point = SOURCE_USB2;
            FILE_BROWSER_DEBUG("Znalazlem IPOD'a ...\n");
            return;
        }
    
        ipod_flag = false;
        ipod_mount_point = 0;
    }
    
    return;
}
#endif 

/**
 * @brief Funkcja zwraca utwór z IPOD'a z pod określonego indeksu
 * @param idx
 * @return NULL jesli nie znalazl
 */
#ifdef IPOD_SUPPORT
Itdb_Track * file_browser::get_ipod_track(uint32_t idx)
{
    uint32_t tmp,i;
    GList *it;
    
    i = 0;
    
    if(ipod_flag==false || itdb==NULL) return NULL;
    
    tmp = g_list_length (itdb->tracks);
    
    if(tmp==0 || idx>tmp) return NULL;
    
    if(idx==0)
    {
        return (Itdb_Track *)itdb->tracks->data;
    }
    
    for (it = itdb->tracks; it != NULL; it = it->next,i++) 
    {
        if(i==idx) return (Itdb_Track *)it->data;
    }
    
    return NULL;
    
}
#endif
/**
 * @Funkcja zwraca liczbe utworów w ipodzie
 * @return 
 */
#ifdef IPOD_SUPPORT
uint32_t file_browser::get_ipod_tracks_cnt(void)
{
    if(itdb==NULL) return 0;
    
    return g_list_length (itdb->tracks);
    
}
#endif
/**
 * @brief Funkcja zwraca całkowitą ściezkę dla utworu mp3 na ipodzie
 * @param tr
 * @return 
 */
#ifdef IPOD_SUPPORT
uint8_t *file_browser::get_ipod_track_abs_path(Itdb_Track *tr)
{   
    if(tr==NULL) return NULL;
    
    memset(path,0,255);
    
    if(ipod_mount_point==SOURCE_USB1) strcpy((char*)path,"/media/usb1");

    if(ipod_mount_point==SOURCE_USB2) strcpy((char*)path,"/media/usb2");
    
    strcpy((char*)&path[11],tr->ipod_path);
    
    for(int i = 0;i<255;i++)
//    while(path !=NULL)
    {
        if(path[i]==0x3a) path[i] = 0x2f;
    }
    
    return path;
}
#endif


void file_browser::media_timerEvent(void)
{
    check_source_status(); 
    
}