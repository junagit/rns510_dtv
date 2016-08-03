/** 
 * @file   player_ctrl_thrads.cpp
 * @author Michał Motyl
 *
 * @date   9 sierpień 2012, 07:04
 */

#include <iostream>
#include <stdint.h>


#include "play_ctrl.h"
#include "rc.h"
#include "dtv_core.h"

/**
 * @brief Funkcja tworzy wątek przełączania kanału
 */
void play_ctrl::StartServiceSelect(int from_idx)
{
        if(most_scan==true || tv_player==NULL) return;
        
//        printf("StartServiceSelect:%d\n",current_ch_index);
        
        //jesli from_idx jest >=0 to resetujemy current_ch_index do wartosci w from_idx
        //przydatne gdy skonczylismy skanowanie automatyczne i chcemy zaczącć odtwarzanie kanalu od jakiejs pozycji
        if(from_idx>=0) current_ch_index = (uint16_t)from_idx;
        
        ch_idx_tmp.clear();


        //uaktualnij liczbe kanalów,potrzebne przy wznowieniu odtwarzania TV przy wyjsciu 
        //ze skanowania 
        total_channles_num = tv_player->DTV_Get_Service_Cnt();
       
        if(isSelecting()==false)//sprawdzanie tej flagi żeby zapobiegać tworzeniu się niepotrzebbnych wątków
        {
            if(debug_timer->thread ()->isRunning ())
              {
//                printf("sys timer is running...\n");
                usleep(10000);
                QCoreApplication::processEvents ();
              }
                //printf("##### %s %d different_channel:%d\n",__FUNCTION__,__LINE__,different_channel);
               if(freq_was_locked==true && signal_is_bad()==false) 
               {
                   doSigSystemInfo(false,0,"");
                   anim->doShowAnimation(true);    
               }
               else
               {
                   if(signal_is_bad()==true && different_channel==true)
                   {
                       //jesli jest brak sygnalu i zmienimy kanał to pokaż 1 raz animację
                       different_channel=false;
                       doSigSystemInfo(false,0,"");
                       anim->doShowAnimation(true);   
                   }
                   else
                   {
                       if(different_channel==true)
                       {
                           different_channel=false;
                            doSigSystemInfo(false,0,"");
                            anim->doShowAnimation(true);   
                       }
                   }
               }
                
    
            ch_sw->start();
            
            if(signal_is_bad()==false)
            {
                //przed przelaczeniem kanalu chowamy pasek volume i main_bar ale gdy nie wybieramy z most-u
                //if(selecting_from_most==false) doSigHideMainBar();
                doSigHideVolumeWindow();
                selecting_from_most = false;
            }
        }
        else
        {
            //jezeli jestesmy w trakcie przelaczenia kanalu to ustaw flage ze musimy ponownie 
            //wybrac kanal po zakoczeniu wyierania obecnego kanalu
            //ustawiamy ta flage tylko gdy sygnał był dobry,inaczej gdy mamy wybrany kanał
            //na którym nie ma sygnału to po przejsciu na kanał z listy na którym jest sygnał
            //program zaczynał odtwarzać wybrany kanał by po chwili wybrać ten sam kanał
            if(signal_is_bad()==false)need_change = true;
//            need_change = true;
            PLAY_CTRL_DEBUG("======= QChannelSwitchThrd is running!!! ========\n");
        }
        
}

