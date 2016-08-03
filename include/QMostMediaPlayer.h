/* 
 * File:   QMostMediaPlayer.h
 * Author: michalm
 *
 * Created on 26 wrzesień 2013, 09:23
 */
#include <QtGui>

#include "QMostFileBrowser.h"
#include "QMostMediaCtrlList.h"
#include "QMediaPlayer.h"
#include "QRNS_Buttons.h"

#ifndef QMOSTMEDIAPLAYER_H
#define	QMOSTMEDIAPLAYER_H

class QMediaPlayerMain : public QObject{
    Q_OBJECT
    
public:
    QMediaPlayerMain(QObject* parent = NULL);;
    ~QMediaPlayerMain();
    void                                MediaPlayerStop(void);
    void                                MediaPlayerResume(void);
    void                                canEnable(bool f){
        if(f)
        {
            if(can_interface==NULL)
            {
                can_interface = new QCanInterface(this);
                can_interface->initCAN();
            }
        }
        else
        {
            delete can_interface;
            can_interface = NULL;
        }
    }
    
    void customEvent(QEvent* e)
    {
        if(e==NULL) return;
        
        if(e->type() == (QEvent::Type)1002)
        {	
            onCanRecieve(QCanInterface::getData());
        }
    };
    volatile bool                       connected;
private:
        QFileBrowser                    *main_file_browser;
        QMediaPlayer                    *media_player;
        QTimer                          *pause_timer;
        QTimer                          *delay_timer;
        QCanInterface                   *can_interface;
        QString                         depth_title[10];
        QRNS_Buttons                    *legned;
        
        volatile bool                   selecting;
        volatile bool                   seekToLastPosition;
        volatile bool                   timeToQuit;
        volatile int                    current_source_id;//zmienna okreslajaca id zródła aktualnego utworu
        volatile int                    current_browser_item;
        volatile int                    selected_browser_item;
        volatile bool                   block_src_event;
        volatile bool                   media_view;
        volatile bool                   init_flag;
        volatile bool                   wait4constructor;
        volatile bool                   block_stop_resume;
        volatile int                    current_dir_depth,selected_file_dir_depth;
        
        void                            setChangeSrc(int src_id);
        void                            quitToTv(void);
        bool                            initFileBrowserFromSavedData(const uint8_t &from_src_id);
        void                            onCanRecieve(QByteArray *data);
        void                            handleMediaStop(void);
        void                            handleMediaResume(void);
        void                            updateFileList(void);
private slots:
        void                            doSigMenuAction(int action);
        void                            on_file_selected(const QString &file,const QString &dir);
        void                            on_dir_selected(const QString &dir);
        void                            onMediaPrepared(void);
        void                            onEOS(void);
        void                            onMediaRemove(uint8_t src_id);
        void                            onMediaInsert(uint8_t src_id);
        void                            doPause(void);
        void                            doDelay(void);
        void                            doBackToCurrentTrack(void);
//        void                            doReturn2ctrl(void);
        
signals:
        void                            sigMenuAction(int action);
        void                            sigTvOnOff(bool state);
        void                            return2ctrl(void);
        void                            menuEnable(bool en);
};

#endif	/* QMOSTMEDIAPLAYER_H */

