/* 
 * File:   QMediaPlayer.h
 * Author: michalm
 *
 * Created on 26 marzec 2013, 07:16
 */
#include <QtGui>
#include <Qt/QtCore>

#include "GstPlayer.h"

#include "QMpStartThrd.h"
#include "QMediaAudioWindow.h"
#include "QMediaVideoWindow.h"
#include "file_browser.h"
#include "wait_animation.h"
#include "rc.h"

#ifndef QMEDIAPLAYER_H
#define	QMEDIAPLAYER_H

class QMediaPlayer : public QObject{
    friend class QMpStartThrd;
    friend class QMediaAudioWindow;

    Q_OBJECT
public:
    QMediaPlayer(QObject* parent = 0);
    ~QMediaPlayer();
    void                setFile(QString file_dir, QString file);
    void                mute(bool f)
    {
        if(gstplayer!=NULL)
        {
            gstplayer->mute(f);
        }
    };
    
    void                addItemToFileList(browser_item *item_temp,bool do_show_list)
    {
        if(audio_window)audio_window->addItemToList(item_temp,do_show_list);
    };
    
//    void                update_src_txt(int src_id);
    int                 ff(void);
    int                 ff(int speed);
    int                 fb(void);
    int                 fb(int speed);
    bool                play(void);
    bool                stop(void);
    bool                pause(void);
    bool                next(bool do_it_now, bool do_highlight);
    bool                prev(bool do_it_now, bool do_highlight);
    void                seekTo(int pos);
    void                hide_media_window(bool hide_all);
    bool                mediaWindowShowed(void){ return media_window_flag;};
    void                show_media_window();
    static bool         isPaused(){ return pause_play; };
    static bool         isPlaying();
    bool                isError(){ return error_flag;};
    void                vol_inc();
    void                vol_dec();
    void                show_video_window(void);
    void                highlightCurrentItem(int idx,bool font_change)
    {
        if(audio_window) audio_window->highlightCurrentTrack(idx, font_change);
    }
    
    int getCurrentIdx()
    {
      if(audio_window) 
      {
          return audio_window->getCurrentIdx();
      }
      
      return -1;
    };
    
    void                setSelectedItem(int idx)
    {
        if(audio_window) audio_window->setSelectedItem(idx);
    }
    
//    void highlightNextItem(int idx)
//    {
//        if(audio_window) audio_window->highlightNext();
//    };
//    void highlightPrevItem(int idx)
//    {
//        if(audio_window) audio_window->highlightPrev();
//    };
    
    
    void                clearFileList(void)
    {
      if(audio_window)audio_window->clearFileList();  
    };
//    void                mute(bool f);
    void                set_eq(int mode)
    {
      if(gstplayer!=NULL) gstplayer->eq_set_equalizer(mode);  
    };
    bool                selecting()
    {
      if(track_select_thrd!=NULL) return track_select_thrd->isRunning();
      
      return true;
    };
//    void                toggleAudioView(QString current_dir,bool mode)
//    {
//        if(audio_window!=NULL) audio_window->toggleView(current_dir,mode);
//    };
    
    void                setTitle(const QString &title,int src_id)
    {
        if(audio_window)audio_window->setTrackTitle(title,src_id);
    };
    
    int                 getCurrentTime()
    {
        int temp;
        if(play_flag && gstplayer!=NULL)
        {
            gstplayer->getCurrentPosition(&temp);
            
            return temp;
        }
        
        return 0;
    };
    
    bool                isVideo()
    { 
        if(music_video==true) return false;
        return true; 
    };
    
    int                 trackName2idx(const QString &track_dir,const QString &track_name);
    
    bool                findLastTrack(void);
    static QString&      getCurrentFileName(){ return current_file_name; };
    static QString&      getCurrentFileDir(){ return current_file_dir; };
    void                sendEOS()
    {
      emit sigEOS();  
    };
    
private:
    static bool       pause_play;
    volatile bool       selecting_flag;
    volatile bool       media_window_flag;
    volatile bool       small_video_win_show;
    volatile int        small_video_win_time;
    volatile int        pos;
    volatile int        track_len;
    volatile bool       music_video;
    volatile bool       error_flag;
    static bool       play_flag;
    volatile bool       ff_fb_flag;
    volatile bool       ff_fb_flag2;
    volatile bool       ff_fb_flag3;
    volatile int        currentFileIsAudio;
    volatile int        seek_pos_temp; 
    static bool         isAudioOrVideo(char *s);
    volatile int        volume_level;
    volatile int        mPlayCompleteFlag;
    volatile int        speed_up;
    volatile int        speed_cnt;
    GstPlayer           *gstplayer;
    static QString      current_file_path;
    static QString      current_file_name;
    static QString      current_file_dir;
    wait_animation      *wait;
    QMpStartThrd        *track_select_thrd;
    QMediaAudioWindow   *audio_window;
    QMediaVideoWindow   *video_window;
    QTimer              *update_timer;
    QTimer              *ff_fb_timer;
    QTimer              *select_timer;
    file_browser        *file_br;

    
    QString             next_prev_file_timer;
    void                update_video_window();
    void                update_audio_window();
    void                show_audio_window(void);
    void                hide_audio_window(void);

    
    void                hide_video_window(void);
    
    static int          testGstPlayerCB(int event);
    
    signals:
    void                sigEOS(void);
    void                sigEOFF(void);
    void                sigMediaPrepared(void);

    
    
    
    private slots:
        void onTimer(void);
        void onTimerFfFb(void);
        void DoTrackSwitch(void);
        
    
};

#endif	/* QMEDIAPLAYER_H */

