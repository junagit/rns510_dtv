/* 
 * File:   QMediaVideoWindow.cpp
 * Author: michalm
 * 
 * Created on 28 marzec 2013, 12:19
 */

#include "QMediaVideoWindow.h"

QMediaVideoWindow::QMediaVideoWindow(QWidget* parent) : QMainWindow(parent)  
{
    setWindowFlags(Qt::FramelessWindowHint);   
    setFocusPolicy(Qt::NoFocus);
    setFont(QFont( "Arial", 25, QFont::Normal));
    setObjectName("audio_win");
//    setGeometry(0,(735-120),1024,120);
    setGeometry(40,768-120-40,1024-80,120);

    track_progress = new QSlider(Qt::Horizontal,this);
    //track_progress->setGeometry(195,70,(714-40),50);
    track_progress->setGeometry(160,60,624,50);
    
    //tak ,mozna wstawic jakis obraz np.  przesuwajaca sie kulke
    //setStyleSheet("QSlider::handle {image: url(:/resources/image.png);}");
    track_progress->setMinimum(0);
    
    track_duration = new QLabel(this);
    track_duration->setObjectName("simple_label");
    //track_duration->setGeometry(865,70,180,50);
    track_duration->setGeometry(784,60,160,50);
    track_duration->setAlignment(Qt::AlignCenter);
//    track_duration->setStyleSheet("border: none; color:white;");
    
    track_current_time = new QLabel(this);
    //track_current_time->setGeometry(10,70,160,50);
    track_current_time->setObjectName("simple_label");
    track_current_time->setGeometry(0,60,160,50);
//    track_current_time->setStyleSheet("border: none; color:white;");
    track_current_time->setTextFormat(Qt::LogText);
    track_current_time->setAlignment(Qt::AlignCenter);
    
    track_duration->setFont(QFont( "Arial", 32, QFont::Normal));
    track_current_time->setFont(QFont( "Arial", 32, QFont::Normal));
    //============================================
    fb_img = new QLabel(this);
    fb_img->setObjectName("simple_label");
    fb_img->setGeometry(417-40,10,60,60);
    fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_normal.png")));
    
    play_img = new QLabel(this);
    play_img->setObjectName("simple_label");
    play_img->setGeometry(442,10,60,60);
    play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/play_normal.png")));
    
    ff_img = new QLabel(this);
    ff_img->setObjectName("simple_label");
    ff_img->setGeometry(544-40,10,60,60);
    ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_normal.png")));
    //============================================
    currentTimeDisplay(0);
//    setGeometry(2,(735-120),1024,120);
}


QMediaVideoWindow::~QMediaVideoWindow() 
{
    
}




 void QMediaVideoWindow::setTrackLen(int val)
 {
     track_progress->setMinimum(0);
     track_progress->setMaximum(val);
     totalTimeDisplay(val/1000);
//     printf("QMediaVideoWindow::setTrackLen:%d\n",val);
     
 }

 void QMediaVideoWindow::setTrackPos(int val)
 {
     if(track_progress->value()!=val)
     {
        track_progress->setValue(val);
        currentTimeDisplay(val/1000);
     }
 }

 void QMediaVideoWindow::totalTimeDisplay(int v)
 {
     int hour = 0;
     int min = 0;
     int sec = 0;
     int temp = 0;
     QString t;
     
     hour = (int)(v/3600);
     
     if(hour) 
     {
         temp = v-(hour*3600);
         min = temp/60;
     }
     else
     min = (int)(v/60);
     
     if(min) 
     {
         temp = v-(hour*3600) - (min*60);
         sec = temp%60;
     }
     else
     {
         sec = v%60;
     }
     
       
     if(hour)
     {//wyswietlamy tylko liczbe godzin jesli są
        t.append(QString().sprintf("%02d",hour));
        t.append(":");
     }
     
    t.append(QString().sprintf("%02d",min));
    t.append(":");
    t.append(QString().sprintf("%02d",sec));

     track_duration->setText(t);
//     printf("QMediaVideoWindow::totalTimeDisplay:%s\n",t.toAscii().data());
 }
 
  void QMediaVideoWindow::currentTimeDisplay(int v)
 {
      int hour = 0;
     int min = 0;
     int sec = 0;
     int temp = 0;
     QString t;
     
     hour = (int)(v/3600);
     
     if(hour) 
     {
         temp = v-(hour*3600);
         min = temp/60;
     }
     else
     min = (int)(v/60);
     
     if(min) 
     {
         temp = v-(hour*3600) - (min*60);
         sec = temp%60;
     }
     else
     {
         sec = v%60;
     }
     
     if(hour)
     {//wyswietlamy tylko liczbe godzin jesli są
        t.append(QString().sprintf("%02d",hour));
        t.append(":");
     }
    t.append(QString().sprintf("%02d",min));
    t.append(":");
    t.append(QString().sprintf("%02d",sec));
         
     track_current_time->setText(t);
 }
  
  void QMediaVideoWindow::setPlayState(bool f)
 {
     if(f==true)
     {
         play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/play_selected.png")));
     }
     else
     {
         //play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/play_normal.png")));
         play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/pause_selected.png")));
     }
 }



 void QMediaVideoWindow::setFfState(bool f)
 {
     if(f==true)
     {
         ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_selected.png")));
     }
     else
     {
         ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_normal.png")));
     }
 }

 void QMediaVideoWindow::setFbState(bool f)
 {
     if(f==true)
     {
         fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_selected.png")));
     }
     else
     {
         fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_normal.png")));
     }
 }