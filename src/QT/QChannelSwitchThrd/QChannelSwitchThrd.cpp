/* 
 * File:   QChannelSwitchThrd.cpp
 * Author: Michał Motyl
 * 
 * Created on 6 luty 2013, 07:30
 */
#include "AS_language.h"
#include "QChannelSwitchThrd.h"
#include "play_ctrl.h"

QChannelSwitchThrd::QChannelSwitchThrd(QWidget* parent) : QThread(parent)
{
    
}


QChannelSwitchThrd::~QChannelSwitchThrd() 
{
    
}

void QChannelSwitchThrd::run()
{
    uint16_t res;
    play_ctrl *p = ((play_ctrl*)parent());
    
    res = p->tv_player->DTV_Service_Select((uint16_t)p->current_ch_index);
    
    QByteArray info = p->getTimeRangeAndStationTitle();
    
    p->can_rc4tv->set_pos_on_can((uint16_t)p->current_ch_index, info.data());

    //jezeli pokazane jest okno more to je chowamy gdy zaczynamy zmieniac kanał
//    if(p->more_is_showed==true)p->show_more_info(false);
    
    
    if(res==0)
    {
        p->freq_was_locked = true;    

        p->no_signal = false;
        
        if(p->mute==true)
        {
            p->tv_player->DTV_Mute(true);
        }
        else
        {
            double vol = p->settings->getSystemVolume();

            p->tv_player->DTV_Set_Volume(vol);
        }
        
        p->tv_player->DTV_EPG_Present_Following_Start_Update();
    }
    else
    {
        //jesli brak sygnalu lub blad odtwarzania przez gst
        if(res==RET_NO_SIGNAL || res==RET_PLAY_ERROR)
        {
            p->freq_was_locked = false;
        //    QMutexLocker ml(&p->mMutex);
            p->no_signal = true;
            p->anim->ShowAnim(false);
            p->different_channel = false;
            
            AS_Language *language = new AS_Language("#WIN_INFO",6);
            char **Lang = NULL;
            Lang=language->GetContext();

            
//            if(p->more_is_showed==true) p->show_more_info(false);

            if(p->main_menu_show_hide==false && p->quick_list_show_flag==false)
            {
                emit p->SigSystemInfo(true,0,QString::fromUtf8(Lang[0]));
            }
            
            delete language;
            
            p->bad_signal_cnt = 4;
            this->exit(0);
            return;
        }
        
        //jesli brak stacji na liście
        if(res==RET_DTV_LIST_EMPTY && p->tv_player->DTV_Get_Service_Cnt()==0)
        {
            p->anim->ShowAnim(false);
            //brak zapisanych stacji
            //emit p->SigSystemInfo(true,0,QString::fromUtf8("BRAK ZAPISANYCH STACJI"));
            AS_Language *language = new AS_Language("#WIN_INFO",6);
            char **Lang = NULL;
            Lang=language->GetContext();
            
            if(p->main_menu_show_hide==false && p->quick_list_show_flag==false)emit p->SigSystemInfo(true,0,QString::fromUtf8(Lang[1]));
            
            delete language;

            this->exit(0);
            return;
        }
    }

    //if(more_flag==true)p->show_more_info(true);
    
    //jesli wybralismy kanał to po przelacznieu zapisujemy kanał na ktorym byliśmy
    p->settings->save_channel_idx(p->current_ch_index);

        
    p->anim->ShowAnim(false);
    this->exit(0);
    return;
}