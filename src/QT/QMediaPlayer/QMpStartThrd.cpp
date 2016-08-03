/* 
 * File:   QMpStartThrd.cpp
 * Author: michalm
 * 
 * Created on 27 marzec 2013, 12:37
 */
#include <stdio.h>

#include "QMediaPlayer.h"
#include "QMpStartThrd.h"

QMpStartThrd::QMpStartThrd(QObject* parent) : QThread(parent) 
{
    
}



QMpStartThrd::~QMpStartThrd() 
{
    
    
}

void QMpStartThrd::run()
{
    QMediaPlayer *p = ((QMediaPlayer*)parent());
    status_t res; 
    if(p==NULL) return;
    
    if(p->isVideo()==true) p->wait->ShowAnim(true); 
    
     res = p->gstplayer->setDataSource(p->current_file_path.toUtf8().data());
    
     if(res!=GSTPlayer_OK) 
     {
         //przetestowć ten warunek
        printf("setDataSource return ERROR!!!!!\n");
        p->play_flag = false;//ustaw flage że nie odtwarzamy
        p->error_flag = true;//ustaw flage bledu
        p->selecting_flag = false;
        //na wszelki wypadek usuwamy z pamieci cala klase odtwarzarki
        p->wait->ShowAnim(false); 
        //w przypadku nie znlezienia pliku do odtwarzania przez funkcję setDataSource wysyłamy sygnał końca pliku
        //wysyłamy EOS w którym jest zawarta także obsługa błedów
        p->sendEOS();
        
        return;
     }
     
    if(p->gstplayer->prepare() == GSTPlayer_OK)
        {
                if(p->gstplayer->start() != GSTPlayer_OK)
                {
                        printf("**** start fail ****\n");
                        p->play_flag = false;//ustaw flage że nie odtwarzamy
                        p->error_flag = true;//ustaw flage bledu
                        //na wszelki wypadek usuwamy z pamieci cala klase odtwarzarki
                        p->wait->ShowAnim(false); 
                        p->selecting_flag = false;
                        //wysyłamy EOS w którym jest zawarta także obsługa błedów
                        p->sendEOS();
                        return;
                }
                else
                {
                        p->pause_play = false;
                        int val;
                        p->gstplayer->getDuration(&val);
                        p->track_len = val;
        
                        if(p->music_video==true)
                        {
                           if(p->audio_window!=NULL) 
                           {
                               p->audio_window->setTrackLen((val));
 
                           }
                        }
                        else
                        {
                            if(p->video_window!=NULL) p->video_window->setTrackLen((val));
                        }
                        
                        if(p->update_timer->isActive()==false) p->update_timer->start();
                        
                        p->error_flag = false;
                        p->play_flag = true;

                        p->wait->ShowAnim(false); 
                        //wysyłamy EOS w którym jest zawarta także obsługa błedów
                        return;
                }
        }
        else
        {
            
            p->update_timer->stop();
            printf("@@@ PREPARE FAIL!!! @@@@ %s %d\n",__FUNCTION__,__LINE__);
            p->play_flag = false;//ustaw flage że nie odtwarzamy
            p->error_flag = true;//ustaw flage bledu
            p->wait->ShowAnim(false); 
            p->selecting_flag = false;
            //wysyłamy EOS w którym jest zawarta także obsługa błedów
            p->sendEOS();
            return;
        }
    p->wait->ShowAnim(false); 
}