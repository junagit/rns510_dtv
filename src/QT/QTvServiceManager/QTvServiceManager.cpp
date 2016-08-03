/** 
 * @file   QTvServiceManager.cpp
 * @author michalm
 * @brief  Okno zarzadzania kanalami TV 
 * @date   6 wrzesień 2012, 13:31
 */
#include <QtGui>
#include <QtCore>
#include <QCheckBox>

#include "QTvServiceManager.h"
#include "rc.h"
#include "dtv_core.h"
#include "play_ctrl.h"

QTvServiceManager::QTvServiceManager(QWidget* parent) : QMainWindow(parent) 
{
    int ch_cnt = 0;
        
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(100,96,720,576);
    setObjectName("audio_win_grey");

    current_ch_idx = 0;
    selected_idx = -1;
    
    service_management = new service_manager();

    if(service_management->Init_services_data_for_user(true)!=0)
    {//jesli cos pojdzie nie tak przy ladowaniu stacji z pliku to wracamy do poprzedniego menu
        delete service_management;
        main_menu *m = new main_menu(parentWidget(),1); 
        return;
    }

    channel_list = new QListWidget(this);
    channel_list->setGeometry(20,20,680,536);
    channel_list->setObjectName("service_manager");
    channel_list->setFont(QFont( "Arial", 35, QFont::Normal));
    channel_list->setUniformItemSizes(true);
    channel_list->setDragEnabled(false);
    channel_list->setDropIndicatorShown(false);
    channel_list->setWordWrap(false);
    channel_list->setWrapping(false);
    channel_list->blockSignals(true);

    ch_cnt = service_management->get_services_count();
    channels_cnt = ch_cnt;

    for(int i = 0;i<ch_cnt;i++)
    {
        QString item_name_tmp = service_management->DTV_Index_To_Service(i)->getServiceName();
        //dajemy na początku pare spcaji żeby odsunąć text od checkboxa...
        item_name_tmp.prepend("  ");
        //printf("service:%s  %d\n",service_management->DTV_Index_To_Service(i)->getServiceName(),service_management->DTV_Index_To_Service(i)->isVisible());
        QListWidgetItem *tmp = new QListWidgetItem(item_name_tmp,channel_list);

        tmp->setFlags(tmp->flags() | Qt::ItemIsUserCheckable);

        if(service_management->DTV_Index_To_Service(i)->isVisible()==true) tmp->setCheckState(Qt::Checked);
        else
        tmp->setCheckState(Qt::Unchecked);

        tmp->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::AlignAbsolute);
        tmp->setSizeHint(QSize(680,456/10));
    }
    
    QPalette p = channel_list->palette();
    p.setColor(QPalette::Highlight,Qt::darkGray);
    channel_list->setPalette(p);
                        
    channel_list->setCurrentRow(current_ch_idx);
   
    
    show();
    
    channel_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    channel_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
//    QTimer::singleShot(200, this, SLOT(doConnect(void)));
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)),Qt::QueuedConnection);
    
    if(QCANrcTV::menu_from_can==true)
    {
        legend = new QRNS_Buttons(NULL,2);
        legend->show();
    }
    else
    legend = NULL;
    
}



QTvServiceManager::~QTvServiceManager() 
{
    disconnect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    delete channel_list;
    delete service_management;
    
    if(legend) delete legend;
}

void QTvServiceManager::doSigMenuAction(int action)
{
                switch(action)
		  {

		  case OK:
                      
                      if(selected_idx<0)
                      {
                        selected_idx = current_ch_idx;
              
                        QPalette p = channel_list->palette();
                        p.setColor(QPalette::Highlight,QColor(128,0,0));
                        channel_list->setPalette(p);
                      }
                      else
                      {
                        QPalette p = channel_list->palette();
                        p.setColor(QPalette::Highlight,Qt::darkGray);
                        channel_list->setPalette(p);
                        
                        selected_idx = -1; 
                      }

		    break;

                 case 0x55:
		 case DOWN:
                 {
                     if(selected_idx<0)
                     {
                        if(current_ch_idx<(channels_cnt-1))
                        {
                            current_ch_idx++;
//                            channel_list->setCurrentRow(current_ch_idx);
                            channel_list->setCurrentItem(channel_list->item(current_ch_idx));
                        }
                     }
                     else
                     {
                         if(selected_idx==(service_management->get_services_count()-1)) break;
                         
                         if(service_management->DTV_Service_Movedown(current_ch_idx+1)!=0)
                         {
                             printf("@@@@@@@@@ error DTV_Service_Movedown!!!!!!!\n");
                             break;
                         }
                         channel_list->clear();
                         current_ch_idx++;
                         selected_idx = current_ch_idx;
                         
                         int ch_cnt = service_management->get_services_count();
                         channels_cnt = ch_cnt;
        
                        for(int i = 0;i<ch_cnt;i++)
                        {
                            QString item_name_tmp = service_management->DTV_Index_To_Service(i)->getServiceName();
                            //dajemy na początku pare spcaji żeby odsunąć text od checkboxa...
                            item_name_tmp.prepend("  ");
                            //printf("service:%s  %d\n",service_management->DTV_Index_To_Service(i)->getServiceName(),service_management->DTV_Index_To_Service(i)->isVisible());
                            QListWidgetItem *tmp = new QListWidgetItem(item_name_tmp,channel_list,0);
                            tmp->setFlags(tmp->flags() | Qt::ItemIsUserCheckable);
                            
                            if(service_management->DTV_Index_To_Service(i)->isVisible()==true) tmp->setCheckState(Qt::Checked);
                            else
                                tmp->setCheckState(Qt::Unchecked);
                            
                            tmp->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::AlignAbsolute);
                            tmp->setSizeHint(QSize(680,456/10));
                        }
                         
                         channel_list->setCurrentRow(current_ch_idx);
                     }
                 }
                     break;
                  
                 case 0x54:
                 case UP:
                 {
                     if(selected_idx<0)
                     {
                        if(current_ch_idx>0)
                        {
                            current_ch_idx--;
                            channel_list->setCurrentRow(current_ch_idx);
                        }
                     }
                     else
                     {
                         if(selected_idx==0) break;
                         
                         channel_list->clear();
                         
                         if(service_management->DTV_Service_Moveup(current_ch_idx+1)!=0)
                         {
                             printf("@@@@@@@@@ error DTV_Service_Moveup!!!!!!!\n");
                             break;
                         }
                         
                         current_ch_idx--;
                         
                         int ch_cnt = service_management->get_services_count();
                         channels_cnt = ch_cnt;
                         selected_idx = current_ch_idx;
                         
                        for(int i = 0;i<ch_cnt;i++)
                        {   
                            QString item_name_tmp = service_management->DTV_Index_To_Service(i)->getServiceName();
                            //dajemy na początku pare spcaji żeby odsunąć text od checkboxa...
                            item_name_tmp.prepend("  ");
                            //printf("service:%s  %d\n",service_management->DTV_Index_To_Service(i)->getServiceName(),service_management->DTV_Index_To_Service(i)->isVisible());
                            QListWidgetItem *tmp = new QListWidgetItem(item_name_tmp,channel_list,0);
                            tmp->setFlags(tmp->flags() | Qt::ItemIsUserCheckable);
                            
                            if(service_management->DTV_Index_To_Service(i)->isVisible()==true) tmp->setCheckState(Qt::Checked);
                            else
                                tmp->setCheckState(Qt::Unchecked);
                            
                            tmp->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::AlignAbsolute); 
                            tmp->setSizeHint(QSize(680,456/10));
                        }
                         
                        channel_list->setCurrentRow(current_ch_idx);
                     } 
                 }
		   break;
                   
                    
                  
		  case LEFT:
                  {
    
                      if(selected_idx>=0) break;
                      
                           if(channel_list->item(current_ch_idx)->checkState()==Qt::Unchecked)
                           {
                               channel_list->item(current_ch_idx)->setCheckState(Qt::Checked);
                               if(service_management->DTV_Service_enable_disable(current_ch_idx,true)!=0)
                               {
                                   printf("@@@@@@@@@ error DTV_Service_enable_disable!!!!!!!\n");
                               }
                           }
                  }   
		    break;


		  case RIGHT:
                  {
       
                      if(selected_idx>=0) break;
                      
                            if(channel_list->item(current_ch_idx)->checkState()==Qt::Checked)
                            {
                                channel_list->item(current_ch_idx)->setCheckState(Qt::Unchecked);
                                if(service_management->DTV_Service_enable_disable(current_ch_idx,false)!=0)
                                {
                                    printf("@@@@@@@@@ error DTV_Service_enable_disable!!!!!!!\n");
                                }
                            }
                  }
		    break;
                    

		  case _EXIT_:
                  {
                    ((play_ctrl*)parentWidget())->UpdateChannelsNumber(true);
                    main_menu *m = new main_menu(parentWidget(),1);
                    m->show();
                    delete this;
                    return;
                  }
		  break;
                  
                 
                    
                case MENU:
                    ((play_ctrl*)parentWidget())->UpdateChannelsNumber(true);
                    delete this;
                    return;
                break;
                
		default:
		break;
	}              
                
}