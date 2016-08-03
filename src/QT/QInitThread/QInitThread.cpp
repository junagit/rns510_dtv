/* 
 * File:   QInitThread.cpp
 * Author: michalm
 * 
 * Created on 6 luty 2013, 08:08
 */

#include "QInitThread.h"
#include "play_ctrl.h"

QInitThread::QInitThread(QWidget* parent) : QThread(parent){
    
}



QInitThread::~QInitThread() {
}

void QInitThread::run()
{
    play_ctrl *p = ((play_ctrl*)parent());
    
    bool res;
    
    res = p->tv_player->DTV_Init();  
    
    if(p->splash!=NULL)
    {
        p->splash->finish(p);
        p->splash->hide();
        p->splash->close();
        delete p->splash;
        p->splash = NULL;
    }
    
    if(res==true)
    {
        p->total_channles_num = p->tv_player->DTV_Get_Service_Cnt();
        p->tv_init_ok = true;
        
    }
    else
    {
        PLAY_CTRL_ERROR_DEBUG("tv_init error!!!!!!!!!\n");
        p->tv_init_ok = false;
        p->total_channles_num = -1;
        emit p->SigSystemInfo(true,1,"Blad inicjalizacji TV");
//        p->m_o_s_t->setMute(true);
    }
    
    
    this->exit(0);
}
