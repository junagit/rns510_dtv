/** 
 * @file   player_ctrl_most.cpp
 * @author Michał Motyl
 *
 * @date   9 październik 2012, 13:07
 */

#include <iostream>
#include <stdint.h>
#include <QtGui/QApplication>
#include <QtCore/qcoreapplication.h>
#include <QString>
#include <QSettings>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

#include "dvbtcontrol.h"
#include "play_ctrl.h"
#include "rc.h"
#include "dtv_core.h"
#include "tv_bar.h"
#include "serialport.h"
#include "serialportinfo.h"
#include "QUserServiceList.h"
#include "QAutoScanWindow.h"
#include "AS_language.h"

extern volatile int cnt3 ;
extern volatile gint last_rendered_frame;
extern volatile int cnt1;
extern volatile int cnt2;
extern bool menu_flag;

void play_ctrl::MOST_TV_STOP_PLAY()
{
    if(main_menu_show_hide==true || most_scan==true) parse_rc_cmd(MENU);
    
    debug_timer->stop();
    
    tv_player->DTV_Force_Stop_Play();
    
    doSigSystemInfo(false,0,"");
    doSigHideMainBar();
    doSigHideVolumeWindow();
    stop_tv_from_most = true;
}

void play_ctrl::MOST_TV_RESUME_PLAY()
{
    if(tv_player!=NULL)
    {
        if(tv_player->DTV_Is_Playing()==true) return;
    }

    no_signal = false;
    different_channel = true;
    bad_signal_cnt = BAD_SIGNAL_TIMEOUT;
    StartServiceSelect(-1);
    debug_timer->start(250);
    stop_tv_from_most = false;
}


void play_ctrl::doEpgFromMOST(bool on_off)
{
    if(wait4constructor==true || tv_init_ok==false) return;
    
    if(most_scan==true) return;
 
    while(isSelecting()==true)usleep(100000);
    
    if(more_is_showed==true) 
    {       
        show_more_info(false);
    }
    else
    {
        if(total_channles_num==0) return;
        
        if(no_signal==true) return;
        
        //schowaj szybką listę
        if(quick_list_show_flag==true)
        {
            //delete tv_list;
            //tv_list = NULL;
            tv_list->hide();
            quick_list_show_flag = false;  
        }  
        
        show_more_info(true);
    }
    
}

void play_ctrl::doScanFromMOST(bool on_off)
{
    if(wait4constructor==true || tv_init_ok==false) return;
    
    if(manual_scan==true) return;//jezeli jestesmy w skanowaniu manualnym to wychodzimy
    
    while(isSelecting()==true)usleep(100000);
    
    if(debug_timer->thread ()->isRunning ())
      {
//        printf("sys timer is running...\n");
        usleep(10000);
//        emit SigSystemInfo(false,0,"");
        stop_sys_timer();
        QCoreApplication::processEvents ();
      }
    
    
    if(most_scan==false)
    {
         //schowaj szybką listę
         if(quick_list_show_flag==true)
        {
            tv_list->hide();
            quick_list_show_flag = false;  
            tv_list->connected = false;
        } 
        
        
        most_scan = true;
       // main_menu_show_hide = true;
        watchdog_ctrl(WDT_CLOSE);
        stop_sys_timer();
        
        tv_player->DTV_Stop_Play();
        
        if(main_menu_show_hide==true) { parse_rc_cmd(MENU); }
        
        doSigSystemInfo(false,0,"");
        
        if(more_is_showed==true) show_more_info(false);
    
        QAutoScanWindow *as = new QAutoScanWindow(this);
        
        cnt2 = 10;
        while(cnt2--)
        {
            usleep(10000);
            QCoreApplication::processEvents();
        }
        
        emit sigMenuAction((int)OK);
    }
    else
    {

        last_rendered_frame = -1;
        cnt3 = 5*10;
        cnt1 = 0;
        cnt2 = 10;
       
        emit sigMenuAction((int)MENU);
        
        int cnt = 10;
            
            while(cnt--) {
                usleep(100000);
                QCoreApplication::processEvents();
            }
        
            UpdateChannelsNumber(false);
            
            need_change = true;
            start_sys_timer();
            watchdog_ctrl(WDT_OPEN);
            most_scan = false;      
    }
}


void play_ctrl::doChSwitchFromMOST_next_prev(bool up_down)
{
    if(wait4constructor==true || tv_init_ok==false) return;
    
    if(more_is_showed==true) return;
    
    different_channel=true;
    
    if(up_down==true)parse_rc_cmd(UP);
    else
    parse_rc_cmd(DOWN);
}


void play_ctrl::doSigTvOnOff(bool state)
{
    
    printf("doSigTvOnOff %d\n",state);
    //if(wait4constructor==true || isSelecting()==true || tv_init_ok==false) return;
    if(wait4constructor==true || tv_init_ok==false) return;
    
    while(isSelecting()==true)usleep(100000);
    
    if(state==true)
    {
        show();
        if(tv_player!=NULL)
        {//zablokuj mozliwość 
            if(tv_player->DTV_Is_Playing()==true) return;
        }

        MOST_TV_RESUME_PLAY();

        can_rc4tv->disableCANsignals(false);
        
        watchdog_ctrl(WDT_OPEN);
        
        last_rendered_frame = -1;
        cnt3 = 5*10;
        cnt1 = 0;
        cnt2 = 10;
        different_channel = true;
    }
    else
    {

        if(most_scan==true)
        {   
            emit sigMenuAction((int)MENU);

            int cnt = 10;
            
            while(cnt--) 
            {
                usleep(100000);
                QCoreApplication::processEvents();
            }
            
            most_scan = false;      
            main_menu_show_hide = false;
            QCANrcTV::menu_from_can = false;
        }

        
        
        MOST_TV_STOP_PLAY();
        
        int cnt = 10;
            
            while(cnt--) 
            {
                usleep(100000);
                QCoreApplication::processEvents();
            }
        
        watchdog_ctrl(WDT_CLOSE);
        
        if(system_info_show_flag==true) doSigSystemInfo(false,0,"");
    
        hide();
        can_rc4tv->disableCANsignals(false);
    }
}


void play_ctrl::doStartMediaMplayer(void)
{
    parse_rc_cmd(MENU);

    menu_flag = false;

    if(wait4constructor==true || tv_init_ok==false) return;

    debug_timer->stop();
    //trzeba zaczekać aż skonczymy wybieranie kanału!!!
    while(isSelecting()==true)usleep(100000);
    
    if(more_is_showed==true)
    {
        doSigQuitFromMore();
    }

    
    if(quick_list_show_flag==true)
    {
        tv_list->hide();
        quick_list_show_flag = false;  
        tv_list->connected = false;
    }
    
    //przed wejsciem w media player trzeba zatrzymac timer
    if(debug_timer->thread ()->isRunning ())
    {
//        printf("sys timer is running...\n");
        usleep(10000);
        stop_sys_timer();
        QCoreApplication::processEvents ();
    }
    
    doSigTvOnOff(false);//wylacz odtwarzanie TV
    doSigSystemInfo(false,0,"");//wylacz okno z powiadomieniami

//    settings->save_tv_state(0);
    debug_timer->stop();
    stop_tv_from_most = true;
    can_rc4tv->disableCANsignals(true);


    if(media_player)
    {
        media_player->MediaPlayerResume();
    }
    else
    {
        media_player = new QMediaPlayerMain(this);
        connect(media_player, SIGNAL(sigTvOnOff(bool)),this,SLOT(doSigTvOnOff(bool)),Qt::QueuedConnection);
        connect(media_player, SIGNAL(menuEnable(bool)),this,SLOT(domenuEnable(bool)));
    }
        
    
    hide();
    update();

}

void play_ctrl::DoChannelSwitchFromMOST(int ch)
{
    if(ch!=current_ch_index) different_channel = true;
    else
    different_channel = false;

    if(isSelecting()==true)
    {
        need_change = true;
        current_ch_index = ch;
        doSigShowMainBar();
        return;
    }

    if(more_is_showed==true)show_more_info(false);
            
    if(quick_list_show_flag==true)
    {
        tv_list->hide();
        quick_list_show_flag = false;  
    }       
    dvb_service *s = NULL;

    can_rc4tv->disableCANsignals(true);

      s = tv_player->DTV_Index_To_Service(ch);

      if(s!=NULL)
      {
          selecting_from_most = true;
          current_ch_index = ch;
          doSigShowMainBar();
          StartServiceSelect(-1);
      }  

   can_rc4tv->disableCANsignals(false);
};