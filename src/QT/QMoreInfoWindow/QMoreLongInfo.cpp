/* 
 * File:   QMoreLongInfo.cpp
 * Author: michalm
 * 
 * Created on 27 marca 2014, 07:27
 */
#include <QtCore>
#include <QtCore/qcoreapplication.h>
#include "dtv_core.h"
#include "play_ctrl.h"
#include "app_settings.h"
#include "rc.h"
#include "AS_language.h"
#include "QMostInterface.h"
#include "QMoreLongInfo.h"

QMoreLongInfo::QMoreLongInfo(QWidget* parent,eit_event *ev,QString service_name) : QMainWindow(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);    
//    setGeometry(112,56,800,655);
    setGeometry(0,0,1024,768);
    setObjectName("audio_win");
    setFocusPolicy(Qt::NoFocus);
    
    
    QPalette transparentPallete;
    transparentPallete.setColor(QPalette::Window, QColor(255, 255, 255, 255));
    setPalette(transparentPallete);
    
    header = new QLabel(this);
    header->setGeometry(112,30,800,60);
    header->setText(service_name);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("border: none; color:white;");

    
    QWidget *horizontalLineWidget1 = new QWidget(this);
    horizontalLineWidget1->setGeometry(20,51+56,(1024-40),3);
    horizontalLineWidget1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget1->setStyleSheet(QString("background-color: #ff6600;"));
    
    title = new QLabel(this);
    title->setObjectName("simple_label");
    title->setGeometry(20,20+50+56,(1024-40),60);
    title->setAlignment(Qt::AlignLeft|Qt::AlignHCenter);
    
    progress = new QProgressBar(this);
    progress->setObjectName("progress_bar1");
    progress->setGeometry(150,100+50+65,724,10);
    progress->setTextVisible(false);
    
    time_label = new QLabel(this);
    time_label->setObjectName("simple_label");
    time_label->setGeometry(20,80+50+65,122,50);
    time_label->setAlignment(Qt::AlignCenter);
    
    time_end_label = new QLabel(this);
    time_end_label->setObjectName("simple_label");
    time_end_label->setGeometry(1024-122-20,80+50+65,122,50);
    time_end_label->setAlignment(Qt::AlignCenter);
    
    txt = new QPlainTextEdit(this);
    txt->setObjectName("more_info_text");
    txt->setGeometry(20,155+50+56,(1024-40),480);
    txt->setReadOnly(true);
    txt->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txt->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border: 2px solid white; border-radius: 5px; background: solid black;width: 12px;} QScrollBar::handle:vertical { border: none; background: lightgrey; min-height: 20px;} QScrollBar::add-line:vertical {background: solid black; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;} QScrollBar::sub-line:vertical {background: solid black; height: 0px; subcontrol-position: top; subcontrol-origin: margin; } ");
    
    show();
    
//    title->setFont(QFont( "Arial", 45, QFont::Normal));
    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
 //=====================================================================================================
    updateInfo(ev,service_name.toAscii().data());
}



QMoreLongInfo::~QMoreLongInfo() 
{
    
}

void QMoreLongInfo::updateInfo(eit_event *ev,const char* service_name)
{
    struct tm start_time;
    struct tm end_time;
    
    if(ev!=NULL)
    {
        start_time = *localtime(&ev->start_time);
        end_time = *localtime(&ev->end_time);

        char tt[64];
        memset(tt,0,64);
        sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

        time_label->setText(QString::fromUtf8(tt));
        time_label->setFont(QFont( "Arial", 40, QFont::Normal));

        memset(tt,0,64);
        sprintf(tt,"%d:%02d",end_time.tm_hour,end_time.tm_min);
        time_end_label->setText(QString::fromUtf8(tt));
        time_end_label->setFont(QFont( "Arial", 40, QFont::Normal));

        title->setAlignment(Qt::AlignLeft);
        title->setFont(QFont( "Arial", 45,QFont::Bold));
        title->setText(title->fontMetrics().elidedText( ev->title, Qt::ElideRight, title->width()));
        title->setFont(QFont( "Arial", 45,QFont::Bold));


        QString tmp =  QString::fromUtf8(ev->des);
        txt->setPlainText(tmp);
        txt->setFont(QFont( "Arial", 45, QFont::Normal));

        progress->setRange((uint32_t)ev->start_time,(uint32_t)ev->end_time);
        progress->setValue((uint32_t)time(NULL));
        progress->show();
    }
    else
    {
        AS_Language *language = new AS_Language("#WIN_INFO",6);
        char **Lang = NULL;
        Lang=language->GetContext();
        
        title->setAlignment(Qt::AlignCenter);
        title->setText(QString::fromUtf8(Lang[2]));
        title->setFont(QFont( "Arial", 35,QFont::Bold));

        progress->hide();
        progress->setValue(0);
        txt->clear();
        time_label->clear();
        time_end_label->clear();
        delete language;
    }
    
    if(service_name!=NULL)header->setText(service_name);
}

void QMoreLongInfo::scrollUp(void)
{
    int temp = txt->verticalScrollBar()->value();
    temp+=txt->verticalScrollBar()->singleStep();
    txt->verticalScrollBar()->setValue(temp);    
}

void QMoreLongInfo::scrollDown(void)
{
    int temp = txt->verticalScrollBar()->value();
    temp-=txt->verticalScrollBar()->singleStep();
    txt->verticalScrollBar()->setValue(temp);
}