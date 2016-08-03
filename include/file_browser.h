/** 
 * @file   file_browser.h
 * @author michalm
 *
 * @date   17 maj 2012, 11:32
 */
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include<glib-2.0/glib.h>
#include <queue>
#include <iostream>

//#include "itdb.h"
//#include "list.h"

#define SOURCE_SD1      0
#define SOURCE_SD2      1
#define SOURCE_USB1     2
#define SOURCE_USB2     3

#define FILE_BROWSER_DEBUG_T       1

#if FILE_BROWSER_DEBUG_T
        #define FILE_BROWSER_DEBUG(fmt, args...)   printf("[FILE_BROWSER][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define FILE_BROWSER_DEBUG(fmt, args...)
#endif

#include <QtGui>
//typedef struct artist_list_entry{
//    
//    GSList *tracks_list;
//}artist_list_entry;
using namespace std;

#ifndef FILE_BROWSER_H
#define	FILE_BROWSER_H

typedef struct browser_item
{
    QString item_name;
    int item_type;
    uint32_t size;
    uint32_t    pos_on_list;
    
} browser_item_t;

class file_browser : public QObject {
    Q_OBJECT
public:
    file_browser(QObject* parent = NULL);
    ~file_browser();
    bool add_supported_extension(const char *extension);
    uint16_t get_dir_cnt(){return dir_counter;}
    uint16_t get_files_cnt(){return file_counter;}
    uint16_t get_all_items_cnt(){return item_counter;}
    QString get_current_path(){return current_path;}
    QString get_current_dir(){ return current_dir;};
    QString get_file_name_by_index(uint32_t index);
    int getCurrentSource(){ return (int)current_source; };
    int refresh_file_list (QString P);
    void set_base_path (QString path);
    bool isRoot(void)
    {
        if(base_path==current_path) return true;
        
        return false;
    }
    //uint8_t * get_item (uint32_t index);
    browser_item *get_item(uint32_t index);
    void enable_src_check(bool en)
    {
      if(en)srcCheck->start();  
      else
          srcCheck->stop();
    };
    bool select_item(uint16_t idx);
//    void register_user_file_select_cb(void(*f)(string file_name));
//    void register_user_dir_select_cb(void (*f)(void));
    
//    void register_user_on_media_remove_cb(void(*f)(uint8_t src_id,void *p),void *up);
//    void register_user_on_media_insert_cb(void(*f)(uint8_t src_id,void *p),void *up);
    
    QString  get_dir (uint32_t index);
//    uint8_t* get_file_absolute_path(uint16_t idx);
    bool select_source(uint8_t src);
    bool go_back(void);
    
    bool get_sd1_status(void)
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
        
        if(sd1_in_flag==true && sd1_last_flag==false)
        {
            sd1_last_flag = sd1_in_flag;
        }
        else
        {
            if(sd1_in_flag==false && sd1_last_flag==true)
            {
                sd1_last_flag = sd1_in_flag;
            }
        }
  
        return sd1_in_flag;
    };
    bool get_sd2_status(void){return sd2_in_flag;};
    
    bool get_usb1_status(void)
    {
        char buf[10];
        memset (&buf,0,10);
        FILE *fd;
  
//        if(ipod_flag==1 && ipod_mount_point==SOURCE_USB1) return 2;
        
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
        
          if(usb1_in_flag==true && usb1_last_flag==false)
        {
            usb1_last_flag = usb1_in_flag;
        }
        else
        {
            if(usb1_in_flag==false && usb1_last_flag==true)
            {
                usb1_last_flag = usb1_in_flag;

            }
        }
        return usb1_in_flag;
    };
    
    bool get_usb2_status(void)
    {
        if(ipod_flag==1 && ipod_mount_point==SOURCE_USB2) return 2;
        
        return usb2_in_flag;
    };
    
#ifdef IPOD_SUPPORT
    Itdb_Track * get_ipod_track(uint32_t idx);
    uint32_t get_ipod_tracks_cnt(void);
    uint8_t *get_ipod_track_abs_path(Itdb_Track *tr);
    //==========================================================================
    uint8_t *get_artist_name(uint16_t artist_idx);
    uint32_t get_artists_list_count(void);
    uint32_t get_unknown_artist_list_tracks_count(void);
    Itdb_Track *get_artist_track(uint16_t artist_idx,uint32_t track_idx);
    uint32_t get_artist_track_count(uint16_t artist_idx);
    Itdb_Track *get_unknown_artist_list_track(uint32_t track_idx);
    //==========================================================================
    uint16_t get_playlist_count(void);
    Itdb_Track *get_track_from_playlist(uint16_t playlist_idx,uint32_t track_idx);
    uint8_t *get_playlist_name(uint16_t playlist_idx);
    uint32_t get_playlist_track_count(uint16_t playlist_idx);
    //==========================================================================
#endif
    
signals:
    void        sigFileSelect(QString file_name);
    void        sigDirSelect(void);
    void        sigMediaRemove(uint8_t src_id);
    void        sigMediaInsert(uint8_t src_id);
    
private:
#ifdef IPOD_SUPPORT
    Itdb_iTunesDB *itdb;
#endif
    struct dirent *current_directory;
    //pthread_t src_check_thrd;
    bool src_check_thrd_init;
    uint32_t item_counter,file_counter,dir_counter;
    QString current_path;
    QString current_dir;
    QString *supported_formats_tab;
//    mm_list *file_list;
    QList<browser_item*> *file_list;
    QString base_path;
    QString start_path;
    uint8_t supported_formats_cnt;

    uint8_t ipod_mount_point;
    
    bool empty_dir_flag;
    bool sd1_in_flag,sd1_last_flag;
    bool sd2_in_flag,sd2_last_flag;
    bool usb1_in_flag,usb1_last_flag;
    bool usb2_in_flag,usb2_last_flag;
    bool ipod_flag;
  ///  bool refresh_sd1_flag,refresh_sd2_flag,refresh_usb1_flag,refresh_usb2_flag;
    int current_source;//miejsce gdzie aktualnie jestesmy(czy na sd1 sd2 lub usb podczas odtważania)
                            //0 - sd1
                            //1 - sd2
                            //3 - usb
    
    
    bool check_supported_extension(char *file_name);
    void execute_user_cb_on_dir_select(void);
    void execute_user_cb_on_file_select(QString file_name);
    
    void execute_user_cb_on_media_remove(uint8_t src_id);
    void execute_user_cb_on_media_insert(uint8_t src_id);
    
#ifdef IPOD_SUPPORT
    void remove_ipod(void)
    {
        if(ipod_flag==true && ipod_mount_point==SOURCE_USB1)
        {
            system("umount -f /media/usb1");
            system("echo 0 > /tmp/usb1_detect");
        }
        
        if(ipod_flag==true && ipod_mount_point==SOURCE_USB2)
        {
            system("umount -f /media/usb2");
            system("echo 0 > /tmp/usb2_detect");
        }
    };
#endif
    
    #ifdef IPOD_SUPPORT
    void check4ipod(void);
#endif
    void create_artist_list(void);
    //uint8_t path[255];
    
    GSList *artist_list;
    GSList *unknown_artist_list_tracks;
    
    GSList *album_list;
    GSList *composer_list;
    void check_source_status();
    QTimer      *srcCheck;

    
private slots:
    void media_timerEvent(void);
    

};

#endif	/* FILE_BROWSER_H */

