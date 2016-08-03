/** 
 * @file   QMoreInfoWindow.cpp
 * @author michalm
 *
 * @date   23 październik 2012, 08:56
 */
#include <QtGui>


#include <QtCore>
#include <QtCore/qcoreapplication.h>
#include <string.h>
#include "QMoreInfoWindow.h"
#include "dtv_core.h"
#include "play_ctrl.h"
#include "app_settings.h"
#include "rc.h"
#include "AS_language.h"
#include "QMostInterface.h"

void *p2 = NULL;
extern void via_set_scalling(bool flag);

QMoreInfoWindow::QMoreInfoWindow(QWidget* parent,void *p) : QMainWindow(parent) 
{
    via_set_scalling(true);
    
    setWindowFlags(Qt::FramelessWindowHint);    
    setObjectName("audio_win_grey");
    setGeometry(0,0,1024,768);
    setFocusPolicy(Qt::NoFocus);

    long_info_flag = false;
    long_info = NULL;
    info_press_cnt = 0;
    
    memset(&last_current_event,0,sizeof(last_current_event));
    memset(&last_next_event,0,sizeof(last_next_event));
    
    current_event = NULL;
    next_event = NULL;
    last_service = NULL;
    can = NULL;
    
    service_list = new QFastList(this);
    service_list->setGeometry(10,10,1004,580);
    
    title = new QLabel(this);
    title->setObjectName("simple_label");
    title->setGeometry(10,768-150-10,1004,75);
    title->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    
    title2 = new QLabel(this);
    title2->setObjectName("simple_label");
    title2->setGeometry(10,768-75-10,1004,75);
    title2->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    
    QWidget *horizontalLineWidget = new QWidget(this);
    horizontalLineWidget->setGeometry(10,768-10-150-10,1004,4);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: grey;"));
    p2 = p;

    show();
    
    int service_cnt_tmp = 0;

    service_manager service_management;
    service_management.Init_services_data_for_user(false);//załaduj listę ze stacjami
    service_cnt_tmp = service_management.get_services_count();
    
    if(service_cnt_tmp>0)
    {
        for(int i =0;i<service_cnt_tmp;i++) 
        {
            QString temp = " " + QString(service_management.DTV_Index_To_Service(i)->getServiceName());
            service_list->addItem(temp);
        }

        service_list->showList();
        service_list->setSelectedIdx(play_ctrl::get_current_ch_idx());
        service_list->highlightIdx(play_ctrl::get_current_ch_idx());
        
        current_item = play_ctrl::get_current_ch_idx();
        service_cnt = service_cnt_tmp;
    }
    else
    {
        service_list->addItem("BRAK ZAPISANYCH STACJI");
        current_item = 0;
    }
    
    title->setFont(QFont( "Arial", 50, QFont::Normal));
    title2->setFont(QFont( "Arial", 50, QFont::Normal));
    connect((QWidget*)p,SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
}



QMoreInfoWindow::~QMoreInfoWindow() 
{
    if(can)delete can;
    
    delete service_list;
    
    if(last_current_event.title!=NULL)
    {
        free(last_current_event.title);
        last_current_event.title = NULL;
    }

    if(last_current_event.des!=NULL)
    {
        free(last_current_event.des);
        last_current_event.des = NULL;
    }
    
    if(last_next_event.title!=NULL)
    {
        free(last_next_event.title);
        last_next_event.title = NULL;
    }

    if(last_next_event.des!=NULL)
    {
        free(last_next_event.des);
        last_next_event.des = NULL;
    }
}

void QMoreInfoWindow::show_info()
{
    
    
    if(can==NULL)
    {
        can = new QCanInterface(this);    
        can->initCAN();
    }
    
    if(is_showed==false)
    {
        via_set_scalling(true);
        is_showed = true;
        service_list->showList();
        service_list->setSelectedIdx(play_ctrl::get_current_ch_idx());
        service_list->highlightIdx(play_ctrl::get_current_ch_idx());
        current_item = play_ctrl::get_current_ch_idx();
        
        service_manager service_management;
        service_management.Init_services_data_for_user(false);//załaduj listę ze stacjami
        service_cnt = service_management.get_services_count();

        if(service_cnt!=service_list->getListCount())
        {
            service_list->clearList();

            for(int i =0;i<service_cnt;i++) 
            {
                QString temp = " " + QString(service_management.DTV_Index_To_Service(i)->getServiceName());
                service_list->addItem(temp);
            }

            service_list->showList();
            service_list->highlightIdx(play_ctrl::get_current_ch_idx());
            service_list->setSelectedIdx(play_ctrl::get_current_ch_idx());
            current_item = play_ctrl::get_current_ch_idx();
            updateInfo();
        }
        else
        {
            updateInfo();
        }
    

    }
    else
    {
        updateInfo();
    }
    
}



void QMoreInfoWindow::doSigMenuAction(int action)
{
     if(is_showed==false) return;
     
        switch(action)
          {
            case 0x54:
            case UP:
            {
                if(current_item<service_cnt-1)
                {
                    current_item++;
                    service_list->highlightIdx(current_item);
                    updateInfo();
                }
            }   
            break;

            case 0x55:
            case DOWN:
            {
                if(current_item>0)
                {
                    current_item--;
                    service_list->highlightIdx(current_item);
                    updateInfo();
                }
            }
            break;

            default:
            break;
        }
}

void QMoreInfoWindow::onMOSTrecieve(QByteArray *data)
{
    if(data==NULL) return;
    
    if(data->at(0)!=0x88) return;
   
    if(is_showed==false) return;
    
    switch(data->at(1))
    {
        case 0x00://wyjscie z epg
            
            if(long_info_flag==true)
            {
                long_info_flag = false;
                long_info->hide();
                info_press_cnt = 0;
            }

            if(can)
            {
                delete can;
                can=NULL;
            }

            hide();
            via_set_scalling(false);
            emit sigQuitFromMore();
            is_showed = false;
       
        break;   
        
        case 0x03:
        
        if(service_cnt==0) return;
        
        if(long_info_flag==true)
        {
            long_info->scrollDown();
        }
        else
        doSigMenuAction(0x55);
        break;
        
        case 0x04:
         if(service_cnt==0) return;
         
        if(long_info_flag==true)
        {
            long_info->scrollUp();
        }
        else
        doSigMenuAction(0x54);
        break;
        
        case 0x05:
        {
//            if(long_info_flag==true) return;

            if(play_ctrl::get_current_ch_idx()!=current_item)
            {
                if(can)
                {
                    delete can;
                    can=NULL;
                }

                is_showed = false;
                via_set_scalling(false);
                //to okno sie schowa gdy zaczniemy wybierac kanał
                emit ChSwitchFromMOST(current_item);
            }
        }
        break;
           
        
        case 0x06:
        { 
            dtv *p = ((play_ctrl*)p2)->get_dtv_p();
            
            if(p->DTV_TimeIsSet()==false) return;
            
            if(long_info_flag==false)
            {
                if(p->DTV_EPG_Is_Ready_For_Service(p->DTV_Index_To_Service(current_item))==true)
                {
                    if(long_info==NULL)
                    {
                        long_info = new QMoreLongInfo(this,&last_current_event,p->DTV_Index_To_Service(current_item)->getServiceName());
                    }
                    else
                    {
                        long_info->show();
                        long_info->updateInfo(&last_current_event,p->DTV_Index_To_Service(current_item)->getServiceName());
                    }

                        long_info_flag = true;
                        info_press_cnt=1;
                }
                else
                {

                }
            }
            else
            {
                info_press_cnt++;
                
                if(info_press_cnt==2)
                {
                    if(p->DTV_EPG_Is_Ready_For_Service(p->DTV_Index_To_Service(current_item))==true)
                    {
                        long_info->updateInfo(&last_next_event,p->DTV_Index_To_Service(current_item)->getServiceName());
                    }
                    else
                    {
                        long_info->updateInfo(NULL,p->DTV_Index_To_Service(current_item)->getServiceName());
                    }
                }
                
                
                if(info_press_cnt==3)
                {
                    long_info->hide();
                    long_info->updateInfo(NULL,NULL);
                    info_press_cnt = 0;
                    long_info_flag = false;
                }
            }
        }
        break;
           
        
        default:
        break;
    }
}

QString QMoreInfoWindow::fit2Label(const QString &s,QLabel *l)
{
    QString res2;            

        res2 = l->fontMetrics().elidedText( s, Qt::ElideRight, l->width());
        return res2;
}

void QMoreInfoWindow::updateInfo(void)
{
    dvb_service *current_service;
    struct tm start_time;
    AS_Language *language = new AS_Language("#WIN_INFO",6);
    char **Lang = NULL;
    
    Lang=language->GetContext();
    
    dtv *p = ((play_ctrl*)p2)->get_dtv_p();
    
        current_service = p->DTV_Index_To_Service(current_item);
        
        if(current_service && p->DTV_TimeIsSet()==true)
        {
            current_event = p->DTV_Get_Current_Event(current_service);

            if(current_event==NULL) current_event = p->DTV_Get_Current_Event_From_File(current_service);

            if(current_event!=NULL)
            {
                if(last_current_event.event_id!=current_event->event_id || current_service->getServiceID()!=last_service->getServiceID())
                {
                    if(last_current_event.title!=NULL)
                    {
                        free(last_current_event.title);
                        last_current_event.title = NULL;
                    }
                    
                    if(last_current_event.des!=NULL)
                    {
                        free(last_current_event.des);
                        last_current_event.des = NULL;
                    }
              
                    last_current_event.event_id = current_event->event_id;
                    last_current_event.start_time = current_event->start_time;
                    last_current_event.end_time = current_event->end_time;
                    if(current_event->title!=NULL)last_current_event.title = strdup(current_event->title);
                    if(current_event->des!=NULL)last_current_event.des = strdup(current_event->des);
         
                    start_time = *localtime(&last_current_event.start_time);
                    char tt[64];
                    memset(tt,0,64);
                    sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                    QString temp_txt;
                    temp_txt.append(tt);
                    temp_txt.append(" ");
                    temp_txt.append(last_current_event.title);
                    title->setText(fit2Label(temp_txt,title));
                }
            }
            else
            {
                if(last_current_event.title!=NULL && p->DTV_TimeIsSet()==true)
                {
                    if(title->text().isEmpty()==true)
                    {
                        start_time = *localtime(&last_current_event.start_time);
                        char tt[64];
                        memset(tt,0,64);
                        sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                        QString temp_txt;
                        temp_txt.append(tt);
                        temp_txt.append(" ");
                        temp_txt.append(last_current_event.title);
                        title->setText(fit2Label(temp_txt,title));
                    }
                    else
                    title->setText(QString::fromUtf8(Lang[2]));
                }
                else
                title->setText(QString::fromUtf8(Lang[2])); 
            }
//=========================================================================================
            next_event = p->DTV_Get_Next_Event(current_service);

            if(next_event==NULL) next_event = p->DTV_Get_Next_Event_From_File(current_service);

            if(next_event!=NULL)
            {
                if(last_next_event.event_id!=next_event->event_id || current_service->getServiceID()!=last_service->getServiceID())
                {
                    if(last_next_event.title!=NULL)
                    {
                        free(last_next_event.title);
                        last_next_event.title = NULL;
                    }
                    
                    if(last_next_event.des!=NULL)
                    {
                        free(last_next_event.des);
                        last_next_event.des = NULL;
                    }
                    
                    last_next_event.event_id = next_event->event_id;
                    last_next_event.start_time = next_event->start_time;
                    last_next_event.end_time = next_event->end_time;
                    if(next_event->title!=NULL)last_next_event.title = strdup(next_event->title);
                    if(next_event->des!=NULL)last_next_event.des = strdup(next_event->des);
                    
                    start_time = *localtime(&next_event->start_time);
                    char tt[64];
                    memset(tt,0,64);
                    sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                    QString temp_txt;
                    temp_txt.append(tt);
                    temp_txt.append(" ");
                    temp_txt.append(next_event->title);
                    title2->setText(fit2Label(temp_txt,title2));
                }
            }
            else
            {
                if(last_next_event.title!=NULL && p->DTV_TimeIsSet()==true)
                {
                    if(title2->text().isEmpty()==true)
                    {
                        start_time = *localtime(&last_current_event.start_time);
                        char tt[64];
                        memset(tt,0,64);
                        sprintf(tt,"%d:%02d",start_time.tm_hour,start_time.tm_min);

                        QString temp_txt;
                        temp_txt.append(tt);
                        temp_txt.append(" ");
                        temp_txt.append(last_next_event.title);
                        title2->setText(fit2Label(temp_txt,title2));
                    }
                    else
                    title2->setText(QString::fromUtf8(Lang[2]));
                }
                else
                title2->setText(QString::fromUtf8(Lang[2]));
            }
            
            last_service=current_service;
        }
        else
        {
            title->setText(QString::fromUtf8(Lang[2]));
            title2->setText(QString::fromUtf8(Lang[2]));
        }
        
        delete language;
}

