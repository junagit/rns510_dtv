/** 
 * @file   QMoreInfoWindow.cpp
 * @author michalm
 *
 * @date   23 październik 2012, 08:56
 */
#include <QtGui>


#include <QtCore>
#include <QtCore/qcoreapplication.h>
#include "QMoreInfoWindow.h"
#include "dtv_core.h"
#include "play_ctrl.h"
#include "app_settings.h"
#include "rc.h"
#include "AS_language.h"
#include "QMostInterface.h"

void *p2 = NULL;

QMoreInfoWindow::QMoreInfoWindow(QWidget* parent,void *p) : QMainWindow(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);    
//    setGeometry(112,56,800,655);
//    setObjectName("audio_win");
    setGeometry(0,0,1024,768);
    setFocusPolicy(Qt::NoFocus);
    
    QPalette palette;
    //palette.setBrush(QPalette::Background,QBrush(QPixmap(":/img/background_mp.png")));
    palette.setBrush(QPalette::Background,QBrush(QPixmap(":/img/Lista_7a_Skala2.png")));
    setPalette(palette);

    last_title.clear();
    last_desc.clear();
    last_event_id = -1;
    most = NULL;
//    QPalette transparentPallete;
//    transparentPallete.setColor(QPalette::Window, QColor(255, 255, 255, 255));
//    setPalette(transparentPallete);
    
    AS_Language *language = new AS_Language("#WIN_INFO",8);
    char **Lang = NULL;
    Lang=language->GetContext();
    
    sound = new QLabel(this);
    sound->setObjectName("simple_label");
    sound->setGeometry(1024-200,768-70,200,70);
    sound->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
//    sound->setText("DŹWIĘK");
    sound->setText(Lang[7]);
    
    source = new QLabel(this);
    source->setObjectName("simple_label");
    source->setGeometry(1024-200,0,200,70);
    source->setAlignment(Qt::AlignCenter);
    //source->setText("ŹRÓDŁO");
    source->setText(Lang[6]);
    
    tv = new QLabel(this);
    tv->setObjectName("simple_label");
    tv->setGeometry(0,0,200,70);
    tv->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
    tv->setText("EXIT");
    
    usb_sd = new QLabel(this);
    usb_sd->setObjectName("simple_label");
    usb_sd->setGeometry(0,(768-70),200,70);
    usb_sd->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
    usb_sd->setText("USB/SD");
    
    p2 = p;
    QLabel *header = new QLabel(this);
    header->setStyleSheet("border: none; color:yellow; background: none;");
    header->setText("INFO");
    header->setGeometry(112,0+20,800,50);
    header->setAlignment(Qt::AlignCenter);
    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
    title = new QLabel(this);
    title->setObjectName("simple_label");
    title->setGeometry(20+112,20+50+56,760,50);
    title->setAlignment(Qt::AlignLeft|Qt::AlignHCenter);
    
    progress = new QProgressBar(this);
    progress->setObjectName("progress_bar1");
    progress->setGeometry(150+112,100+50+56,500,10);
    //progress->setStyleSheet("QProgressBar:horizontal {border: 2px solid white; border-radius: 5px; background: black; text-align: left; color:white } QProgressBar::chunk:horizontal {background: lightgrey; color: white}");
    progress->setTextVisible(false);
    
    title->setFont(QFont( "Arial", 35, QFont::Normal));
    is_showed = false;
    
    time_label = new QLabel(this);
    time_label->setObjectName("simple_label");
    time_label->setGeometry(20+112,80+50+56,122,50);
    time_label->setAlignment(Qt::AlignCenter);
    
    time_end_label = new QLabel(this);
    time_end_label->setObjectName("simple_label");
    time_end_label->setGeometry(800-122-20+112,80+50+56,122,50);
    time_end_label->setAlignment(Qt::AlignCenter);
    
    txt = new QPlainTextEdit(this);
    txt->setObjectName("more_info_text");
    txt->setGeometry(20+112,155+50+56,760,415-28);
    txt->setReadOnly(true);
    txt->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //txt->setStyleSheet("border: none; color:white; border-radius: 8px; background: solid black;");
    
    txt->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border: 2px solid white; border-radius: 5px; background: solid black;width: 12px;} QScrollBar::handle:vertical { border: none; background: lightgrey; min-height: 20px;} QScrollBar::add-line:vertical {background: solid black; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;} QScrollBar::sub-line:vertical {background: solid black; height: 0px; subcontrol-position: top; subcontrol-origin: margin; } ");
    txt->setUndoRedoEnabled(false);
    txt->setOverwriteMode(false);

    hide();
    connect((QWidget*)p,SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    QWidget *horizontalLineWidget = new QWidget(this);
    horizontalLineWidget->setGeometry(20+112,135+50+56,760,3);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: #ff6600;"));
    
//    most = new QCanInterface(this);    
//    most->initCAN();
    delete language;
}



QMoreInfoWindow::~QMoreInfoWindow() 
{
    delete txt;
    if(most)delete most;
    delete title;
    delete time_label;
}

void QMoreInfoWindow::show_info()
{
    dtv *p = ((play_ctrl*)p2)->get_dtv_p();
    //dvb_service *temp = NULL;
    AS_Language *language = new AS_Language("#WIN_INFO",6);
    char **Lang = NULL;
    struct tm start_time;
    struct tm end_time;
    Lang=language->GetContext();
    

    if(is_showed==false)
    {
        current_service = p->DTV_GetCurrent_Service();
        
        if(current_service!=NULL && p->DTV_TimeIsSet()==true)
        {
            current_event = p->DTV_Get_Current_Event(current_service);
            
            if(current_event==NULL) current_event = p->DTV_Get_Current_Event_From_File(current_service);
            
            if(current_event!=NULL)
            {
                if(last_event_id==-1 || last_event_id!=current_event->event_id)
                {
                    start_time = *localtime(&current_event->start_time);
                    end_time = *localtime(&current_event->end_time);

                    last_start_time = current_event->start_time;
                    last_end_time = current_event->end_time;
                    last_event_id = current_event->event_id;

                    char tt[64];
                    memset(tt,0,64);
                    sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                    time_label->setText(QString::fromUtf8(tt));
                    time_label->setFont(QFont( "Arial", 35, QFont::Normal));

                    memset(tt,0,64);
                    sprintf(tt,"%d:%02d",end_time.tm_hour,end_time.tm_min);
                    time_end_label->setText(QString::fromUtf8(tt));
                    time_end_label->setFont(QFont( "Arial", 35, QFont::Normal));

                    title->setAlignment(Qt::AlignLeft);
                    title->setFont(QFont( "Arial", 35,QFont::Bold));
                    title->setText(fit2Label(QString::fromUtf8(current_event->title),title));
                    title->setFont(QFont( "Arial", 35,QFont::Bold));
                    
                    last_title = QString::fromUtf8(current_event->title);
                    
                    QString tmp =  QString::fromUtf8(current_event->des);
                    last_desc = tmp;
                    txt->setPlainText(tmp);
                    txt->setFont(QFont( "Arial", 35, QFont::Normal));
                }
               
                    progress->setRange((uint32_t)current_event->start_time,(uint32_t)current_event->end_time);
                    progress->setValue((uint32_t)time(NULL));
                    progress->show();
            }
            else
            {
                if(last_title.isEmpty()==false && p->DTV_TimeIsSet()==true)
                {
                    if(title->text().isEmpty()==true)
                    {
                        start_time = *localtime(&last_start_time);
                        end_time = *localtime(&last_end_time);

                        char tt[64];
                        memset(tt,0,64);
                        sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                        time_label->setText(QString::fromUtf8(tt));
                        time_label->setFont(QFont( "Arial", 35, QFont::Normal));

                        memset(tt,0,64);
                        sprintf(tt,"%d:%02d",end_time.tm_hour,end_time.tm_min);
                        time_end_label->setText(QString::fromUtf8(tt));
                        time_end_label->setFont(QFont( "Arial", 35, QFont::Normal));


                        title->setAlignment(Qt::AlignLeft);
                        title->setFont(QFont( "Arial", 35,QFont::Bold));
                        title->setText(fit2Label(last_title,title));
                        title->setFont(QFont( "Arial", 35,QFont::Bold));
                        txt->setPlainText(last_desc);
                        txt->setFont(QFont( "Arial", 35, QFont::Normal));
                    }
                        progress->setRange((uint32_t)last_start_time,(uint32_t)last_end_time);
                        progress->setValue((uint32_t)time(NULL));
                        progress->show();
                }
                else
                {
                    title->setAlignment(Qt::AlignCenter);
                    title->setText(QString::fromUtf8(Lang[2]));
                    title->setFont(QFont( "Arial", 35,QFont::Bold));
                    
                    progress->hide();
                    progress->setValue(0);
                }
            }
        }
        else
        {
            title->setAlignment(Qt::AlignCenter);
            title->setText(QString::fromUtf8(Lang[2]));
            title->setFont(QFont( "Arial", 35,QFont::Bold));
            
            progress->setValue(0);
            progress->hide();
        }
        show();
    
        is_showed=true;
        
        source->setFont(QFont( "Arial", 35, QFont::Normal));
        sound->setFont(QFont( "Arial", 35, QFont::Normal));
        tv->setFont(QFont( "Arial", 35, QFont::Normal));
        usb_sd->setFont(QFont( "Arial", 35, QFont::Normal));
    }
   
    delete language;
    
    if(most==NULL)
    {
        most = new QCanInterface(this);    
        most->initCAN();
    }
}



void QMoreInfoWindow::doSigMenuAction(int action)
{
        switch(action)
          {
            case 0x54:
            case _PLUS:
            {
                int temp = txt->verticalScrollBar()->value();
                temp+=txt->verticalScrollBar()->singleStep();
                txt->verticalScrollBar()->setValue(temp);      
//                        txt->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);   
            }   
            break;

            case 0x55:
            case _MINUS:
            {
                int temp = txt->verticalScrollBar()->value();
                temp-=txt->verticalScrollBar()->singleStep();
                txt->verticalScrollBar()->setValue(temp);
//                txt->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);   
            }
            break;

            default:
            break;
        }
}

void QMoreInfoWindow::onMOSTrecieve(QByteArray *data)
{
    if(data==NULL) return;
    
    switch(data->at(0))
    {
        case 0x85:
            
        emit sigQuitFromMore();
        if(most)
        {
            delete most;
            most=NULL;
        }
        break;   
        
//###################################################################
        case 0x80://galka w lewo
        doSigMenuAction(0x55);
        break;

        case 0x81:
        doSigMenuAction(0x54);
        break;

        
        case 0x83:
        emit sigQuitFromMore();
        
        if(most)
        {
            delete most;
            most=NULL;
        }
        break;
        
            
        case 0x84:
        emit startMediaMplayer();  
        if(most)
        {
            delete most;
            most=NULL;
        }
        break;
        
        default:
        break;
    }
}

QString QMoreInfoWindow::fit2Label(const QString &s,QLabel *l)
{
    QFont myFont = QFont( "Arial", 35, QFont::Normal);
    QFontMetrics fm(myFont);
    QString res = s;
    QString res2;            
    
    int wdth = fm.width(res);
    
    if(wdth>l->width())
    {
        res2 = l->fontMetrics().elidedText( res, Qt::ElideRight, l->width());
        return res2;
    }
    else
    return s;             
}