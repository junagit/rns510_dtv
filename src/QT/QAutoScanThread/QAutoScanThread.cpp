/* 
 * File:   QAutoScanThread.cpp
 * Author: juna
 * 
 * Created on 31 grudzień 2013, 08:52
 */
#include "QAutoScanWindow.h"
#include "submenu_scan.h"
#include "manual_scan.h"
#include "dtv_core.h"
#include "app_settings.h"
#include "play_ctrl.h"
#include "tsi.h"
#include "QAutoScanThread.h"

QAutoScanThread::QAutoScanThread(QWidget* parent) : QThread(parent)
{ 
  
}



QAutoScanThread::~QAutoScanThread () 
{ 

}

void QAutoScanThread::run ()
{
QAutoScanWindow *v;
    v = ((QAutoScanWindow*)parent());
    
    dvb_service_list *temp_list;
    dtv *player = ((play_ctrl*)v->parentWidget())->get_dtv_p();
    
    
    system("mv /media/localdata/dvb_services.dat /media/localdata/dvb_services_old.dat");
    sync();
    
    for(int i = 0;i<57;i++)
    {
        if(v->quit==true) break;
        
        //emit v->updateProgress(i);
        v->doUpdateProgress (i);
        
        temp_list = player->DTV_Scan(freq_t[i].freq*1000,(freq_t[i].band*1000));
       //usleep(200000);
        msleep(200);
        
        if(temp_list!=NULL)
        {
            player->DTV_Save_Scan(false);
            printf("ZNALAZLEM %d KANALOW NA CZESTOTLIWOSCI %d\n",temp_list->get_list_count(),freq_t[i].freq);
            v->total_channels_found += temp_list->get_list_count();
            emit v->AddChannelsToList(temp_list);
            //v->doAddChannelsToList (temp_list);
            //usleep(800000);
            msleep (800);
        }
    }
    
    v->scanning = false;
    
    if(v->quit==false) emit v->sigShowMessageAfterScan();
    
    exit(0);
  
}

