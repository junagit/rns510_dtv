/** 
 * @file   tv_bar.cpp
 * @author Michał Motyl
 *
 * @date   9 sierpień 2012, 10:22
 */
#include <QtGui/qapplication.h>

#include "app_settings.h"
#include "tv_bar.h"
#include "misc_func.h"
#include "QMoreInfoWindow.h"
#include "dtv_core.h"
#include "play_ctrl.h"

tv_bar::tv_bar(QWidget* parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);     
    setGeometry(40,588-40,1024-80,180);
    setObjectName("audio_win");
    setFocusPolicy(Qt::NoFocus);
    
    kanal = new QLabel(this);
    kanal->setGeometry(10,10,90,55);
    kanal->setAlignment(Qt::AlignCenter);
    kanal->setFont(QFont( "Arial", 30, QFont::Normal));
    kanal->setObjectName("label_type2");
  
    czas = new QLabel(this);
    czas->setGeometry((1024-100)-80,10,90,55);
    czas->setAlignment(Qt::AlignCenter);
    czas->setObjectName("label_type2");
    
#if 0
    nazwa = new QLabel(this);
    nazwa->setGeometry(90,10,800,55);
    nazwa->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    nazwa->setObjectName("label_type2");
    
    co_leci = new QLabel(this);
    co_leci->setGeometry(90,75,800,55+40);
    co_leci->setObjectName("label_type2");
    co_leci->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    progress = new QProgressBar(co_leci);
    progress->setGeometry(120,(95-41)+20-5,800-200-40,10);
    //progress->setStyleSheet("QProgressBar:horizontal {border: 2px solid white; border-radius: 5px; background: black; text-align: left; color:white } QProgressBar::chunk:horizontal {background: lightgrey; color: white}");
    progress->setTextVisible(false);
    progress->setStyleSheet("QProgressBar{border: 2px solid white; border-radius: 5px; background: black; text-align: left; color:white } QProgressBar::chunk {background: #ff6600; color: white}");
    progress->hide();
    
    time_label = new QLabel(co_leci);
    time_label->setGeometry(10,95-41,100,40);
    time_label->setAlignment(Qt::AlignCenter);
    time_label->setStyleSheet("border: none; color:white;");
    
    time_end_label = new QLabel(co_leci);
    time_end_label->setGeometry(800-100-10,95-41,100,40);
    time_end_label->setAlignment(Qt::AlignCenter);
    time_end_label->setStyleSheet("border: none; color:white;");
#else    
    QFrame *frame = new QFrame(this);
    frame->setGeometry(105,10,814-80,65+95);
    frame->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    
    QWidget *horizontalLineWidget = new QWidget(frame);
    //horizontalLineWidget->setFixedHeight(4);
    horizontalLineWidget->setGeometry(20,57,774-80,3);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: #ff6600;"));
    
    nazwa = new QLabel(frame);
    nazwa->setGeometry(20,0,814-40-80,55);
    nazwa->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    nazwa->setStyleSheet("border: none; color:white;");
    
    co_leci = new QLabel(frame);
    co_leci->setGeometry(20,70,814-40,55+40);
    co_leci->setStyleSheet("border: none; color:white;");
    co_leci->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    progress = new QProgressBar(co_leci);
    progress->setGeometry(120,(95-50)+20-5,814-240-40-80,10);
    //progress->setStyleSheet("QProgressBar:horizontal {border: 2px solid white; border-radius: 5px; background: black; text-align: left; color:white } QProgressBar::chunk:horizontal {background: lightgrey; color: white}");
    progress->setTextVisible(false);
    progress->setStyleSheet("QProgressBar{border: 2px solid white; border-radius: 5px; background: black; text-align: left; color:white } QProgressBar::chunk {background: #ff6600; color: white}");
    progress->hide();
    
    time_label = new QLabel(co_leci);
    time_label->setGeometry(10,95-50,100,40);
    time_label->setAlignment(Qt::AlignCenter);
    time_label->setStyleSheet("border: none; color:white;");
    
    time_end_label = new QLabel(co_leci);
    time_end_label->setGeometry(814-100-10-40-80,95-50,100,40);
    time_end_label->setAlignment(Qt::AlignCenter);
    time_end_label->setStyleSheet("border: none; color:white;");
#endif
    
//    record = new QLabel(this);
//    record->setGeometry((1024-80),45,48,48);
//    record->setObjectName("simple_label");
//    set_record_info(false);
    record = NULL;
    show();
    
    QFont temp = QFont( "Arial", 27, QFont::Normal);
    czas->setFont(temp);

    temp = QFont( "Arial", 33, QFont::Normal);
    co_leci->setFont(temp);
    nazwa->setFont(temp);
    kanal->setFont(temp);
}


tv_bar::~tv_bar() 
{
    delete kanal;
    delete nazwa;
    delete czas;
    delete co_leci;
    //delete record;
}




void tv_bar::set_current_channel(QString txt)
{
    if(nazwa!=NULL && txt!=nazwa->text()) nazwa->setText(txt);
}

void tv_bar::set_current_channel_number(QString txt)
{
    if(kanal!=NULL && txt!=kanal->text()) kanal->setText(txt);
}

void tv_bar::set_current_time(QString txt)
{
    if(czas!=NULL && txt != czas->text()) 
    {
        czas->setText(txt);
    }
}

void tv_bar::set_current_event_name(QString txt)
{

    if(co_leci!=NULL && txt!=co_leci->text()) 
    {
        co_leci->setText(co_leci->fontMetrics().elidedText( txt, Qt::ElideRight, co_leci->width()));
    }
}

void tv_bar::fill_progress(eit_event *ev)
{
    struct tm start_time;
    struct tm end_time;
    
    if(ev==NULL) 
    {
        progress->hide();
        time_end_label->clear();
        time_label->clear();
        return;
    }
    
    start_time = *localtime(&ev->start_time);
    end_time = *localtime(&ev->end_time);

    char tt[64];
    memset(tt,0,64);
    sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

    time_label->setText(QString::fromUtf8(tt));
    time_label->setFont(QFont( "Arial", 27, QFont::Normal));

    memset(tt,0,64);
    sprintf(tt,"%d:%02d",end_time.tm_hour,end_time.tm_min);
    time_end_label->setText(QString::fromUtf8(tt));
    time_end_label->setFont(QFont( "Arial", 27, QFont::Normal));
    
    progress->setRange((uint32_t)ev->start_time,(uint32_t)ev->end_time);
    progress->setValue((uint32_t)time(NULL));
    progress->show();
}


/**
 * @brief Funkcja ustawia stan ikony nagrywania na pasku
 * @param f == true->ikona nagrywanie aktywne,false->nagrywanie wył.
 */
void tv_bar::set_record_info(bool f)
{
//    if(f==true)
//    {
//        record->setPixmap(QPixmap(QString::fromUtf8(":/img/recordnormal64x64.png")));
//        record->setStyleSheet("border: none; color:white; background: none");   
//        record->show();
//    }
}