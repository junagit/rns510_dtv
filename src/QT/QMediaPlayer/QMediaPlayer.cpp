/* 
 * File:   QMediaPlayer.cpp
 * Author: michalm
 * 
 * Created on 26 marzec 2013, 07:16
 */
#include <QtGui>
#include "QMediaPlayer.h"
#include "GstPlayer.h"
#include "QMostTvList.h"
#include "play_ctrl.h"
#include "overlay.h"
QMediaPlayer *player_ptr = NULL;

extern volatile bool need_update_volume;

QString      QMediaPlayer::current_file_path;
QString      QMediaPlayer::current_file_name;
QString      QMediaPlayer::current_file_dir;
bool         QMediaPlayer::pause_play;
bool         QMediaPlayer::play_flag;
    
QMediaPlayer::QMediaPlayer(QObject* parent) : QObject(parent) 
{
    pause_play = false;
    error_flag = false;
    gstplayer = NULL;
    play_flag = false;
    ff_fb_flag = false;
    ff_fb_flag2 = false;
    ff_fb_flag3 = false;
    small_video_win_show = false;
    speed_up = 0;
    speed_cnt = 0;
    audio_window = NULL;
    video_window = NULL;
    volume_level = 5;
    seek_pos_temp = 0;
    next_prev_file_timer.clear();
    selecting_flag = false;
    file_br = NULL;
    
    track_select_thrd = new QMpStartThrd(this);
    update_timer = new QTimer(this);
    update_timer->setInterval(1000);
    
    connect(update_timer, SIGNAL(timeout()),this,SLOT(onTimer(void)),Qt::QueuedConnection);
    
    
    ff_fb_timer = new QTimer(this);
    ff_fb_timer->setInterval(1500);
    ff_fb_timer->setSingleShot(true);
    ff_fb_timer->stop();
    connect(ff_fb_timer, SIGNAL(timeout()),this,SLOT(onTimerFfFb(void)),Qt::QueuedConnection);
    
    audio_window = new QMediaAudioWindow(NULL);
    audio_window->hide();

    video_window = new QMediaVideoWindow(NULL);
    video_window->hide();
    
    wait = new wait_animation(NULL);
    wait->hide();
    
    ///=================
    //timer który wlacza utwor po czasie
    select_timer = new QTimer(this);
    select_timer->setInterval(1500);
    select_timer->setSingleShot(true);
    select_timer->stop();
    connect(select_timer, SIGNAL(timeout()),this,SLOT(DoTrackSwitch(void)));
    //##############
    
    player_ptr = this;
    update_timer->start();
}



QMediaPlayer::~QMediaPlayer() 
{

    select_timer->stop();
    delete select_timer;
    
    if(gstplayer!=NULL)
    {
        delete gstplayer;
        gstplayer = NULL;
    }

    delete track_select_thrd;

    hide_audio_window();
   // hide_video_window();
    
    delete audio_window;
    delete video_window;
    delete wait;
    delete file_br;
}

/**
 * @brief Funkcja sprawdza czy plik to plik z muzyka lub video
 * @param s
 * @return true - audio,false video
 */
bool QMediaPlayer::isAudioOrVideo(char *s)
{
    char * pch = NULL;
    
    pch=strrchr(s,'.');
    if(pch!=NULL)
    {
          if(!strcmp((pch),".mp3")) return true;
          if(!strcmp((pch),".MP3")) return true;
          
          if(!strcmp((pch),".wav")) return true;
          if(!strcmp((pch),".WAV")) return true;
          
          if(!strcmp((pch),".ogg")) return true;
          if(!strcmp((pch),".OGG")) return true;
          
          if(!strcmp((pch),".avi")) return false;
          if(!strcmp((pch),".AVI")) return false;
          
            if(!strcmp((pch),".mp4")) return false;
            if(!strcmp((pch),".MP4")) return false;
          
            if(!strcmp((pch),".3gp")) return false;
           if(!strcmp((pch),".3GP")) return false;
          
            if(!strcmp((pch),".3g2")) return false;
          if(!strcmp((pch),".3G2")) return false;
          
            if(!strcmp((pch),".mov")) return false;
          if(!strcmp((pch),".MOV")) return false;
          
            if(!strcmp((pch),".mpeg")) return false;
          if(!strcmp((pch),".MPEG")) return false;
          
            if(!strcmp((pch),".m2ts")) return false;
          if(!strcmp((pch),".M2TS")) return false;
    }
    
}


/**
 * 
 * @return 
 */
bool QMediaPlayer::play(void)
{
    if(gstplayer==NULL) return false;

    if(current_file_path.isEmpty()==true) return false;

    if(pause_play==true)
    {
        pause_play = false;
        gstplayer->seekTo(pos);
        gstplayer->start();

        if(isVideo()==false)audio_window->setPlayState(true);
        else
            video_window->setPlayState(true);
        
        return true;
    }
    
    //update_timer->stop();
    if(update_timer->thread ()->isRunning ())
    {
//        printf("update timer is running...\n");
        usleep(10000);
        update_timer->stop();
        QCoreApplication::processEvents ();
    }
    
    selecting_flag = true;
    
    while(track_select_thrd->isRunning())
    {
        //gstplayer->reset();
        usleep(500000);
        printf("czekam na zakonczenie watku...\n");
    }
    
    gstplayer->stop();

        if(gstplayer!=NULL)
            {
                delete gstplayer;
                gstplayer = NULL;
            }
        
        if(gstplayer==NULL)
            {
                music_video = isAudioOrVideo(current_file_path.toUtf8().data());

                if(music_video==true)
                {
                    gstplayer = new GstPlayer(MediaType_Audio);
                }
                else
                {
//                    printf("#### CREATE VIDEO PLAYER ####\n");
                    gstplayer = new GstPlayer(MediaType_Video);
                }
          //      show_audio_window();
           }
         gstplayer->setGstPlayerCB(&testGstPlayerCB);
        track_select_thrd->start();
    
    
        if(isVideo()==false)
        {
//            audio_window->setTrackTitle(current_file_name,current_file_dir,-1);
            audio_window->setTrackPos(0);
            audio_window->setPlayState(true);
        }
        else
        {
//            audio_window->setTrackTitle(current_file_name,current_file_dir,-1);
            video_window->setTrackPos(0);
            video_window->setPlayState(true);
        }
        
    
    return true;
}


/**
 * 
 * @return 
 */
bool QMediaPlayer::stop(void)
{
    if(gstplayer==NULL) return false;
    
   // printf("##### QMediaPlayer::stop####\n");
    if(update_timer->thread ()->isRunning ())
    {
//        printf("update timer is running...\n");
        usleep(10000);
        update_timer->stop();
        QCoreApplication::processEvents ();
    }
    
    
    ff_fb_flag = false;
    ff_fb_flag3 = false;
    ff_fb_flag2 = false;
    selecting_flag = false;
    while(track_select_thrd->isRunning())
    {
        //gstplayer->reset();
        usleep(500000);
//        printf("czekam na zakonczenie watku...\n");
    }

//    gstplayer->stop();
//    gstplayer->reset();
    if(pause_play==true)
    {
        gstplayer->stop();
        gstplayer->reset();
        pause_play = false;
    }


    if(gstplayer->isPlaying())
    {
        gstplayer->stop();
        gstplayer->reset();
        pause_play = false;
        play_flag = false;
        
        
        if(isVideo()==false)
        {
            audio_window->setTrackPos(0);
            audio_window->setPlayState(false);
        }
        else
        {
            video_window->setTrackPos(0);
            video_window->setPlayState(false);
        }
        return true;
    }

    return false;
}


/**
 * 
 * @return 
 */
bool QMediaPlayer::pause(void)
{
    if(gstplayer==NULL) return false;
        
    if(pause_play==false)
    {
        pause_play = true;
        gstplayer->getCurrentPosition((int*)&pos);
        gstplayer->pause();
        
        if(isVideo()==false)
        {
            audio_window->setPlayState(false);
        }
        else
        {
            video_window->setPlayState(false);
        }
        
        return true;
    }

    return false;
}



/**
 * 
 * @param file
 */
void  QMediaPlayer::setFile(QString file_dir, QString file)
{
    current_file_path.clear();
    current_file_name.clear();
    current_file_dir.clear();
    
    current_file_path.append(file_dir);
    current_file_name.append(file);
    current_file_dir.append(file_dir);
    next_prev_file_timer = file;

    ff_fb_flag2 = false;
    ff_fb_flag3 = false;
    ff_fb_flag = false;
    selecting_flag = false;
    
    if(gstplayer!=NULL)
    {   
        delete gstplayer;
        gstplayer = NULL;
    }
    
    current_file_path.append(file);
    printf("current_file_path:%s\n",current_file_path.toAscii().data());
    
    if(gstplayer==NULL)
    {
        music_video = isAudioOrVideo(current_file_path.toUtf8().data());

        if(music_video==true)
        {
            gstplayer = new GstPlayer(MediaType_Audio);
       //     show_audio_window();
//            audio_window->highlightCurrentTrack(pos_in_dir);
        }
        else
        {
            gstplayer = new GstPlayer(MediaType_Video);
            //show_audio_window();
//            audio_window->highlightCurrentTrack(pos_in_dir);
        }
        
         gstplayer->setGstPlayerCB(&testGstPlayerCB);
    }
 
}

/**
 * 
 */
void QMediaPlayer::show_audio_window(void)
{
    if(audio_window->isVisible()==false)audio_window->show();
//    if(current_file_name.isEmpty()==false) audio_window->setTrackPath(current_file_name);

    
}

/**
 * 
 */
void QMediaPlayer::hide_audio_window(void)
{
//    if(audio_window!=NULL)
//    {
//        delete audio_window;
//        audio_window = NULL;
//    }
    audio_window->hide();
}


/**
 * 
 */
void QMediaPlayer::show_video_window(void)
{
    if(video_window==NULL)
    {   
//        printf("show_video_window\n");
        video_window = new QMediaVideoWindow(NULL);
        small_video_win_show = true;
    }
    else
    {
        if(video_window->isVisible()==true)
        {
            video_window->hide();
            small_video_win_show = false;
        }
        else
        {
            video_window->show();
            small_video_win_show = true;
        }
        
    }
    
    
}

/**
 * 
 */
void QMediaPlayer::hide_video_window(void)
{
//    if(video_window!=NULL)
//    {
//        delete video_window;
//        video_window = NULL;
//    }
    video_window->hide();
}

/**
 * 
 */
void QMediaPlayer::update_video_window()
{
int temp,duration;
int to_display;

gstplayer->getCurrentPosition(&temp);
gstplayer->getDuration(&duration);

   if(video_window!=NULL)
    {
        if(ff_fb_flag==false)
        {
            video_window->setTrackPos(temp);
        }
        else
        {
            if(ff_fb_flag2==true)
            {
                
                to_display = temp+seek_pos_temp;
                
               if(to_display>=duration) to_display = duration;
            }
            else
            {
                if(ff_fb_flag3==true)
                {
                    to_display = temp - seek_pos_temp;
                    
                    if(to_display<0) to_display = 1000;
                }
            }
                
                video_window->setTrackPos(to_display);
        }
    }
}

/**
 * 
 */
void QMediaPlayer::update_audio_window()
{
int temp;
int to_display;
int duration;

gstplayer->getCurrentPosition(&temp);
gstplayer->getDuration(&duration);

   if(audio_window!=NULL)
    {
        if(ff_fb_flag==false)
        {
            audio_window->setTrackPos(temp);
        }
        else
        {
            if(ff_fb_flag2==true)
            {
                to_display = temp+seek_pos_temp;
                
               if(to_display>=duration) to_display = duration;
            }
            else
            {
                if(ff_fb_flag3==true)
                {
                    to_display = temp - seek_pos_temp;
                    
                    if(to_display<=0) to_display = 1000;
                }
            }
                
                audio_window->setTrackPos(to_display);
        }
    }
}

/**
 * 
 */
void QMediaPlayer::onTimer(void)
{
    app_settings settings;
    
    if(gstplayer!=NULL)
    {
        if(gstplayer->isPlaying()==true)
        {
            if(need_update_volume==true)
            {
                need_update_volume = false;
                double vol = settings.getSystemVolume();
                gstplayer->vol_set(vol); 
            }
            //usaktualniamy okno z paskiem postepu
          if(ff_fb_flag==false)  
          {
            if(isVideo()==false) update_audio_window(); 
            else
            {
                if(media_window_flag==true) update_audio_window(); 
                else
                update_video_window();
               //obsluga howania paska video gdy odtwarzany jest plik wideo
               if(small_video_win_show==true)
               {
                   if(small_video_win_time==0)
                   {
                       small_video_win_time = 4;
                       small_video_win_show = false;
                       hide_video_window();
                   }
                   else
                   {
                       small_video_win_time--;
                   }
               }
               else
               {
                   small_video_win_time = 4;
               }
            }
          }
        }
    }
    
    //jesli dostalismy blad od gstreamer-a to wysyłamy do MOST liste z plikami
    if(error_flag==true)
    {
        printf("##### wykrylem blad !!!!\n");
        error_flag = false;

    }
}


void QMediaPlayer::onTimerFfFb(void)
{
   int temp;
   int total_time;
   int skip_to = 0;
    //jesli wykonał sie callback od timera to znaczy że puściliśmy przycisk przewijania
    
    gstplayer->getCurrentPosition(&temp);
    gstplayer->getDuration(&total_time);
    
    if(ff_fb_flag2==true)
    {
        ff_fb_flag2 = false;
        skip_to = temp + seek_pos_temp + 1500;
            if(skip_to<(total_time-2000))
            {
//                printf("FF TIME OK!!!!\n");
            }
            else
            {
                printf("FF TIME  NOT OK!!!!\n");
               
               ff_fb_flag = false;
               
               if(isVideo()==false)
                {
                    if(audio_window)
                    {
                        audio_window->setFbState(false);
                        audio_window->setFfState(false);
                    }
                }
                else
                {
                    video_window->setFbState(false);
                    video_window->setFfState(false);
                    video_window->hide();
                }
               gstplayer->stop();
               emit sigEOS(); 
            }
    }
    
    if(ff_fb_flag3==true)
    {
        ff_fb_flag3 = false;
        skip_to = temp - seek_pos_temp -1500;
        
        if(skip_to>1000)
        {
//            printf("FB TIME OK!!!!\n");
        }
        else
        {
            skip_to = 1000;
//            gstplayer->stop();
//            gstplayer->reset();
//            gstplayer->start();
//            ff_fb_flag = false;
        }
        
    }
    
    if(ff_fb_flag)
    {
        ff_fb_flag = false;
        
        if(isVideo()==false)
        {
            if(audio_window)
            {
                audio_window->setFbState(false);
                audio_window->setFfState(false);
            }
        }
        else
        {
            video_window->setFbState(false);
            video_window->setFfState(false);
          //  fw1_disable();
            video_window->hide();
        }
        
        if(gstplayer) 
        {
            if(gstplayer->isPlaying())
            {
                if(skip_to)
                {
                    gstplayer->seekTo(skip_to);
                    gstplayer->start();
                }
            }
            else
            {
                gstplayer->stop();
                gstplayer->reset();
                gstplayer->start();
                if(skip_to) gstplayer->seekTo(skip_to);
            }

            seek_pos_temp = 0;
        }
    }
    
    speed_cnt = 0;
    speed_up=0;
    emit sigEOFF();
}

int QMediaPlayer::testGstPlayerCB(int event)
{
    app_settings settings;
    
    if(player_ptr==NULL) return 0;
    
    //printf("####### QMediaPlayer::testGstPlayerCB enter...\n");
    //    fflush(stdout);
    
    if(event == MEDIA_NOP)
    {
        printf("**** testGstPlayerCB event is MEDIA_NOP ****\n");	
    }
    else if(event == MEDIA_PREPARED)
    {
        //added your operations base this event
//        printf("**** testGstPlayerCB event is MEDIA_PREPARED ****\n");
        if(player_ptr->gstplayer) 
        {
            emit player_ptr->sigMediaPrepared();

            double vol = settings.getSystemVolume();
            player_ptr->gstplayer->vol_set(vol); 
            player_ptr->selecting_flag = false;
        }
        
       // if(player_ptr->gstplayer)player_ptr->gstplayer->vol_set((double)0.5);
     //   printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
     //   fflush(stdout);
        return 0;
    }
    else if(event == MEDIA_PLAYBACK_COMPLETE)
    {
        //added your operations base this event
//        printf("**** testGstPlayerCB event is MEDIA_PLAYBACK_COMPLETE ****\n");
	player_ptr->mPlayCompleteFlag = 1;
        
        if(player_ptr->gstplayer)
        {
            player_ptr->gstplayer->stop();
            player_ptr->gstplayer->reset();
            
         //   printf("@@@@@@@2ff_fb_flag2:%d ff_fb_flag3:%d\n",player_ptr->ff_fb_flag2,player_ptr->ff_fb_flag3);
            if(!(player_ptr->ff_fb_flag2 ^ player_ptr->ff_fb_flag3))
            {
                if(player_ptr->ff_fb_flag==false) player_ptr->emit sigEOS();
            }
            
           // if(!(((player_ptr->ff_fb_flag2 ^ player_ptr->ff_fb_flag3) ^ player_ptr->ff_fb_flag)))player_ptr->emit sigEOS();
            player_ptr->speed_cnt = 0;
            player_ptr->speed_up=0;
         //   printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
        //fflush(stdout);
            return 0;
        }
    }
   else if(event == MEDIA_BUFFERING_UPDATE)
    {
        //added your operations base this event
    //    printf("**** testGstPlayerCB event is MEDIA_BUFFERING_UPDATE ****\n");
    }   
    else if(event == MEDIA_FF_COMPLETE)
    {
        //added your operations base this event
     //   printf("**** testGstPlayerCB event is MEDIA_FF_COMPLETE***\n");
        player_ptr->emit sigEOS();
        
        player_ptr->mPlayCompleteFlag = 1;
//        if(player_ptr->gstplayer)
//        {
//            player_ptr->gstplayer->stop();
//            player_ptr->gstplayer->reset();
//        }
        
        player_ptr->ff_fb_timer->stop();
        player_ptr->ff_fb_flag = true;
       // printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
       // fflush(stdout);
        return 0;
    }    
    else if(event == MEDIA_FB_COMPLETE)
    {
        //added your operations base this event
    //    printf("**** testGstPlayerCB event is MEDIA_FB_COMPLETE****\n");
        player_ptr->ff_fb_timer->stop();
        player_ptr->ff_fb_flag = true;
       // printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
       // fflush(stdout);
        return 0;
    }
    else if(event == MEDIA_ERROR)
    {
        //added your operations base this event
        printf("**** MEDIA_ERROR****\n");
        if(player_ptr->gstplayer)
        {
            player_ptr->update_timer->stop();
            usleep(500000);
     
            player_ptr->gstplayer->stop();

            player_ptr->gstplayer->reset();
    
            player_ptr->error_flag = true;
            player_ptr->selecting_flag = false;
            
            if(player_ptr->ff_fb_flag==false)player_ptr->emit sigEOS();
            
        //    printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
       // fflush(stdout);
            return 0;
        }
    }
    else if(event == MEDIA_INFO)
    {
        //added your operations base this event
      //  printf("**** MEDIA_INFO****\n");
      //  printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
      //  fflush(stdout);
        return 0;
    }
    else
    {
        //added your operations base this event
      //  printf("**** testGstPlayerCB event is no define  %d ****\n",event);
      //  printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
      //  fflush(stdout);
        return 0;
    }
 
      //  printf("####### QMediaPlayer::testGstPlayerCB exit...\n");
      //  fflush(stdout);
    return 0;
}

void  QMediaPlayer::hide_media_window(bool hide_all)
{
    if(hide_all==true)
    {
        hide_audio_window();
        hide_video_window();
    }
    else
    {
        if(isVideo()==true) hide_audio_window();
    }
    
    media_window_flag = false;
//    system("mm w 0x04105054 0x00004621");
     
}

bool QMediaPlayer::isPlaying()
{
    return play_flag;
}

void QMediaPlayer::seekTo(int pos)
{
    if(gstplayer) gstplayer->seekTo(pos);
}

int QMediaPlayer::ff(void)
{
    if(gstplayer) 
    {
        if(ff_fb_flag==false)
        {
            ff_fb_flag = true;
            
            //gstplayer->getCurrentPosition(&temp);
            //seek_pos_temp = temp;
            seek_pos_temp = 0;
            seek_pos_temp+=1000;     
            ff_fb_flag2 = true;
//            gstplayer->getDuration(&total_time);
            
            if(isVideo()==true)
            {
                 if(video_window->isVisible()==false)
                 {
                    audio_window->hide();
                    video_window->show();
                    video_window->setFfState(true);  
//                    fw1_enable();
                 }
            }
            else
            audio_window->setFfState(true);  
            
        }
        else
        {
            if(speed_cnt++>=20 && speed_up==0)
            {
                speed_cnt = 0;
                speed_up=1;
            }
            else
            {
                if(speed_cnt++>=20 && speed_up==1)
                {
                    speed_cnt = 0;
                    speed_up=2;
                }
                else
                {
                    if(speed_cnt++>=20 && speed_up==2)
                    {
                        speed_cnt = 0;
                        speed_up=3;
                    }
                }
            }
                              

                if(speed_up==0)seek_pos_temp+=1000;
                if(speed_up==1)seek_pos_temp+=2000;
                if(speed_up==2)seek_pos_temp+=4000;
                if(speed_up==3)seek_pos_temp+=8000;
                ff_fb_flag2 = true;

        }
        
        ff_fb_flag3 = false;
        
        if(isVideo()==false)update_audio_window();
        else
        update_video_window();

        ff_fb_timer->stop();
        ff_fb_timer->start();
    }
    
    return 0;
}

int QMediaPlayer::ff(int speed)
{
    if(gstplayer) 
    {
        if(ff_fb_flag==false)
        {
            ff_fb_flag = true;
            
            seek_pos_temp = 0;
            seek_pos_temp+=5000;     
            ff_fb_flag2 = true;
            
            if(isVideo()==true)
            {
                 if(video_window->isVisible()==false)
                 {
                    audio_window->hide();
                    video_window->show();
                    video_window->setFfState(true);  
//                    fw1_enable();
                 }
            }
            else
            audio_window->setFfState(true);  
            
        }
        else
        {
            
            speed_up = speed;
//            if(speed_cnt++>=20 && speed_up==0)
//            {
//                speed_cnt = 0;
//                speed_up=1;
//            }
//            else
//            {
//                if(speed_cnt++>=20 && speed_up==1)
//                {
//                    speed_cnt = 0;
//                    speed_up=2;
//                }
//                else
//                {
//                    if(speed_cnt++>=20 && speed_up==2)
//                    {
//                        speed_cnt = 0;
//                        speed_up=3;
//                    }
//                }
//            }
                              

                if(speed_up==0)seek_pos_temp+=1000;
                if(speed_up==1)seek_pos_temp+=2000;
                if(speed_up==2)seek_pos_temp+=5000;
                if(speed_up==3)seek_pos_temp+=8000;
                ff_fb_flag2 = true;

        }
        
        ff_fb_flag3 = false;
        
        if(isVideo()==false)update_audio_window();
        else
        update_video_window();

        ff_fb_timer->stop();
        ff_fb_timer->start();
    }
    
    return 0;
}

int QMediaPlayer::fb(void)
{

    if(gstplayer) 
    {
        if(ff_fb_flag==false)
        {
            ff_fb_flag = true;

            seek_pos_temp = 0;
            seek_pos_temp+=1000;
            ff_fb_flag3 = true;
            
            if(isVideo()==true)
            {
                if(video_window->isVisible()==false)
                {
                    audio_window->hide();
                    video_window->show();
                    video_window->setFbState(true);
//                    fw1_enable();
                }
            }
            else
            {
              audio_window->setFbState(true);
            }
            
        }
        else
        {
             if(speed_cnt++>=20 && speed_up==0)
            {
                speed_cnt = 0;
                speed_up=1;
            }
            else
            {
                if(speed_cnt++>=20 && speed_up==1)
                {
                    speed_cnt = 0;
                    speed_up=2;
                }
                else
                {
                    if(speed_cnt++>=20 && speed_up==2)
                    {
                        speed_cnt = 0;
                        speed_up=3;
                    }
                }
            }
             
                

                 if(speed_up==0)seek_pos_temp+=1000;
                if(speed_up==1)seek_pos_temp+=2000;
                if(speed_up==2)seek_pos_temp+=5000;
                if(speed_up==3)seek_pos_temp+=8000; 
                ff_fb_flag3 = true;
            
        }
        
        ff_fb_flag2 = false;
        if(isVideo()==false)update_audio_window();
        else
        update_video_window();
                
        ff_fb_timer->stop();
        ff_fb_timer->start();
    }
    
    return 0;
}

int QMediaPlayer::fb(int speed)
{

    if(gstplayer) 
    {
        if(ff_fb_flag==false)
        {
            ff_fb_flag = true;

            seek_pos_temp = 0;
            seek_pos_temp+=1000;
            ff_fb_flag3 = true;
            
            if(isVideo()==true)
            {
                if(video_window->isVisible()==false)
                {
                    audio_window->hide();
                    video_window->show();
                    video_window->setFbState(true);
//                    fw1_enable();
                }
            }
            else
            {
              audio_window->setFbState(true);
            }
            
        }
        else
        {
            speed_up = speed;
            
//             if(speed_cnt++>=20 && speed_up==0)
//            {
//                speed_cnt = 0;
//                speed_up=1;
//            }
//            else
//            {
//                if(speed_cnt++>=20 && speed_up==1)
//                {
//                    speed_cnt = 0;
//                    speed_up=2;
//                }
//                else
//                {
//                    if(speed_cnt++>=20 && speed_up==2)
//                    {
//                        speed_cnt = 0;
//                        speed_up=3;
//                    }
//                }
//            }
             
                

                 if(speed_up==0)seek_pos_temp+=1000;
                if(speed_up==1)seek_pos_temp+=2000;
                if(speed_up==2)seek_pos_temp+=4000;
                if(speed_up==3)seek_pos_temp+=8000; 
                ff_fb_flag3 = true;
            
        }
        
        ff_fb_flag2 = false;
        if(isVideo()==false)update_audio_window();
        else
        update_video_window();
                
        ff_fb_timer->stop();
        ff_fb_timer->start();
    }
    
    return 0;
}

void QMediaPlayer::vol_inc()
{
    if(volume_level<10) 
    {
        volume_level++;
        gstplayer->vol_set(volume_level);
    }
}

void QMediaPlayer::vol_dec()
{
    if(volume_level>0) 
    {
        volume_level--;
        gstplayer->vol_set(volume_level);
    }
}

void QMediaPlayer::show_media_window()
{
    int temp;
    
    int total_time;
    
//    setTitle(current_file_name,trackName2idx(current_file_dir,current_file_name));
    if(isVideo()==true)
    {
        //audio_window->setWindowOpacity(0.80);
//        system("mm w 0x04105054 0x00b04631");
    }
    else
    {
        //audio_window->setWindowOpacity(1.00);
//        system("mm w 0x04105054 0x00004621");
    }

    show_audio_window();

    
    if(isVideo()==true)
    {
        if(current_file_name.isEmpty()==false)
        {
        
            gstplayer->getCurrentPosition(&temp);
            gstplayer->getDuration(&total_time);
            audio_window->setTrackLen(total_time);
            audio_window->setTrackPos(temp);
        
        }
        
        if(isPlaying()==true) audio_window->setPlayState(true);
        else
        {
            if(isPaused()==true) audio_window->setPlayState(false);
        }
        
        if(video_window->isHidden()==false)
        {
            video_window->hide();
        }

    }
    else
    {
//        system("mm w 0x04105054 0x00004621");
    }
 

    media_window_flag = true;

}

bool QMediaPlayer::next(bool do_it_now, bool do_highlight)
{
  int max_files = 0;
  int next_file_idx = 0;
  QString to_compare;

  if(selecting_flag==true) return false;


  if(isPlaying()==true)
  {
      if(file_br==NULL) file_br = new file_browser(this);
      
        file_br->refresh_file_list(current_file_dir);

      max_files = file_br->get_files_cnt();
       
//      printf("%s %d max_files:%d dirs:%d\n",__FUNCTION__,__LINE__,max_files,file_br->get_dir_cnt());

      if(max_files>0)
      {
          for(int i = 0;i<max_files;i++)
          {
              QString temp = file_br->get_file_name_by_index(i);
              
              if(do_it_now==true)
              {
                  to_compare = current_file_name;
              }
              else
              to_compare = next_prev_file_timer;
                 
//              printf("temp:%s to compare:%s\n",temp.toAscii().data(),to_compare.toAscii().data(),i);
              if(temp==to_compare)
              {
                  next_file_idx = i+1;
                  
                  if(next_file_idx>=max_files)next_file_idx = 0;
                  
//                  printf("##### %s %d next_file_idx:%d\n",__FUNCTION__,__LINE__,next_file_idx);
                  
                  if(do_it_now==true)
                  {//przelaczamy plik narychmiast
              //      printf("#### ZNALAZLEM NASTEPNY PLIK: %s\n",file_br->get_file_name_by_index(next_file_idx).toAscii().data());
                    setFile(current_file_dir,file_br->get_file_name_by_index(next_file_idx));
      
                    if(do_highlight==true)
                    {
                      audio_window->highlightCurrentTrack(next_file_idx+file_br->get_dir_cnt(), false);
//                      audio_window->setSelectedItem(next_file_idx+file_br->get_dir_cnt());

                    }
                    //jezeli bedziemy odtwarzac wideo to howamy okno dla mp3
                    if(isVideo()==true)hide_media_window(false);
                    else
                    show_media_window();

                    play();
                    
                    delete file_br;
                    file_br = NULL;
                        
                    return true;
                  }
                  else
                  {//przelaczamy 
//                    printf("#### ZNALAZLEM NASTEPNY PLIK2: %s\n",file_br->get_file_name_by_index(next_file_idx).toAscii().data());
                    next_prev_file_timer = file_br->get_file_name_by_index(next_file_idx);
                    
                    if(do_highlight==true)
                    {
                       audio_window->highlightCurrentTrack(next_file_idx+file_br->get_dir_cnt(), false);
//                       audio_window->setSelectedItem(next_file_idx+file_br->get_dir_cnt());
                    }
                    
                    //tutaj podswietlamy plik
                    select_timer->stop();
                    select_timer->start();
                    
                    delete file_br;
                        file_br = NULL;
                    return true;
                  }
              }
          }
          
          delete file_br;
          file_br = NULL;
      }
  }
  
  return false;
}

bool  QMediaPlayer::prev(bool do_it_now, bool do_highlight)
{
//  file_browser *fb = NULL;
  
  int max_files = 0;
  int next_file_idx = 0;
  QString to_compare;
  
  if(selecting_flag==true) return false;
//  printf("QMediaPlayer::prev\n");
  
  if(isPlaying()==true)
  {
      if(file_br==NULL) file_br = new file_browser(this);
      
        file_br->refresh_file_list(current_file_dir);
      
      max_files = file_br->get_files_cnt();
      
//      printf("%s %d max_files:%d dirs:%d\n",__FUNCTION__,__LINE__,max_files,file_br->get_dir_cnt());
      
      if(max_files>0)
      {
          for (int i = max_files; i --> 0; )
          {  
              QString temp = file_br->get_file_name_by_index(i);
              
              
             if(do_it_now==true)
              {
                  to_compare = current_file_name;
              }
              else
              to_compare = next_prev_file_timer;
                 
//              printf("temp:%s to compare:%s\n",temp.toAscii().data(),to_compare.toAscii().data(),i);
              
              if(temp==to_compare)
              {
                  next_file_idx = i-1;
                  
                  if(next_file_idx<0)
                  {
                      next_file_idx = max_files-1;
                  }
                  
                  
                  if(do_it_now==true)
                  {//przelaczamy plik narychmiast
                       // printf("#### ZNALAZLEM NASTEPNY PLIK: %s\n",file_br->get_file_name_by_index(next_file_idx));
                        setFile(current_file_dir,file_br->get_file_name_by_index(next_file_idx));

                        if(do_highlight)
                        {
                            audio_window->highlightCurrentTrack(next_file_idx+file_br->get_dir_cnt(), false);
                        }

                        //jezeli bedziemy odtwarzac wideo to howamy okno dla mp3
                        if(isVideo()==true)hide_media_window(false);
                        else
                        show_media_window();

                        play();
                        
                        delete file_br;
                        file_br = NULL;
                        
                        return true;
                  }
                  else
                  {//przelaczamy 
//                        printf("#### ZNALAZLEM NASTEPNY PLIK 2: %s\n",fb->get_file_name_by_index(next_file_idx));
//                      printf("#### ZNALAZLEM NASTEPNY PLIK2: %s\n",file_br->get_file_name_by_index(next_file_idx).toAscii().data());
                        next_prev_file_timer = file_br->get_file_name_by_index(next_file_idx);
                        
                        if(do_highlight)
                        {
                            audio_window->highlightCurrentTrack(next_file_idx+file_br->get_dir_cnt(), false);
//                            audio_window->setSelectedItem(next_file_idx+file_br->get_dir_cnt());
                        }
                        
                        select_timer->stop();
                        select_timer->start();
                        
                        delete file_br;
                        file_br = NULL;
                        
                        return true;
                  }
              }
          }
          
          delete file_br;
          file_br = NULL;
      }
  }
  
  return false;
}

void QMediaPlayer::DoTrackSwitch(void)
{
    selecting_flag = true;
    
    if(isPlaying())stop();
    
    setFile(current_file_dir,next_prev_file_timer);
    
        //jezeli bedziemy odtwarzac wideo to howamy okno dla mp3
    if(isVideo()==true)hide_media_window(false);
    else
    show_media_window();

    //po wybraniu utworu ustaw indeks do zaznaczenia(nie podświetlenia)
    audio_window->setSelectedItem(audio_window->getCurrentIdx());                
    play();
}

/**
 * @brief Funkcja zamienia nazwe pliku na jego indeks w katalogu
 * @param track_name
 * @param track_dir
 * @return 
 */
int QMediaPlayer::trackName2idx(const QString &track_dir,const QString &track_name)
{
    file_browser *fb = NULL;
    int max_files = 0;
    
   // printf("trackName2idx:%s %s\n",track_dir.toAscii().data(),track_name.toAscii().data());
    
    fb = new file_browser(NULL);
    
    if(fb==NULL) return -1;
    
    fb->set_base_path(track_dir);
    
    max_files = fb->get_files_cnt();

      if(max_files>0)
      {
          for(int i = 0;i<max_files;i++)
          {
           //   printf("trackName2idx:%s %s\n",track_name.toAscii().data(),QString((const char*)fb->get_file_name_by_index(i)).toAscii().data());
              if(track_name==fb->get_file_name_by_index(i)) 
              {
                  delete fb;
                  return i;
              }
          }
          
      }
    delete fb;
    return -1;
}

//void QMediaPlayer::update_src_txt(int src_id)
//{
////    if(src_id==SOURCE_SD1)
////    {
////        if(audio_window)
////        audio_window->source_img2->setText("SD");
////    }
////    
////    if(src_id==SOURCE_USB1)
////    {
////        if(audio_window)
////        audio_window->source_img2->setText("USB");
////    }
//}