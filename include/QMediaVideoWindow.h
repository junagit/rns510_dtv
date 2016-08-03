/* 
 * File:   QMediaVideoWindow.h
 * Author: michalm
 *
 * Created on 28 marzec 2013, 12:19
 */
#include <QtGui>

#ifndef QMEDIAVIDEOWINDOW_H
#define	QMEDIAVIDEOWINDOW_H

class QMediaVideoWindow  : public QMainWindow{
    friend class QMediaPlayer;
    Q_OBJECT
public:
    QMediaVideoWindow(QWidget* parent = 0);
    ~QMediaVideoWindow();
     void               setTrackLen(int val);                       
     void               setTrackPos(int val);
     
private:
    void               totalTimeDisplay(int v);
    void               currentTimeDisplay(int v);
    void               setPlayState(bool f);
    void               setFfState(bool f);
    void               setFbState(bool f);
     
    QSlider             *track_progress;
    QLabel              *track_duration;
    QLabel              *track_current_time;
    QLabel             *play_img;
    QLabel             *ff_img;
    QLabel             *fb_img;
     
};

#endif	/* QMEDIAVIDEOWINDOW_H */

