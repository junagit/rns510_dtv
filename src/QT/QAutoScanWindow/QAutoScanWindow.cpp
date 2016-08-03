/** 
 * @file   QAutoScanWindow.cpp
 * @author Michał Motyl
 *
 * @date   10 wrzesień 2012, 14:09
 */
#include "rc.h"
#include "play_ctrl.h"
#include "dtv_core.h"
#include "QAutoScanWindow.h"
#include "submenu_scan.h"

#include "manual_scan.h"
#include "dtv_core.h"
#include "app_settings.h"
#include "play_ctrl.h"
#include "tsi.h"
#include "AS_language.h"


QAutoScanWindow::QAutoScanWindow(QWidget* parent) : QMainWindow(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(100,96,720,576);
//    setObjectName("audio_win");
    blockSignals(true);
    
    setObjectName("audio_win_grey");   

    legend = NULL;
    was_scanned = false;
    added_channels_cnt = 0;
    total_channels_found = 0;
    quit = false;
    scanning = false;
    
    qauto_scan_thrd = new QAutoScanThread (this);
    
    frameCanal = new QGroupBox("KANAL",this);
    frameCanal->setGeometry(20,20,300,122);
    frameCanal->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    frameCanal->setAlignment(Qt::AlignVCenter);
    frameCanal->setFont(QFont( "Arial", 30, QFont::Normal));
    
    canal = new QLabel(frameCanal);
    canal->setGeometry(0,5,frameCanal->width(),frameCanal->height());
    canal->setFont(QFont( "Arial", 30, QFont::Normal));
    canal->setStyleSheet("border: none; color:white; background: none");
    canal->setAlignment(Qt::AlignCenter);
    
    frameFreq = new QGroupBox("CZESTOTLIWOSC",this);
    frameFreq->setGeometry(20,162,300,122);
    frameFreq->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    frameFreq->setFont(QFont( "Arial", 30, QFont::Normal));
    
    freq = new QLabel(frameFreq);
    freq->setGeometry(0,5,frameFreq->width(),frameFreq->height());
    freq->setFont(QFont( "Arial", 30, QFont::Normal));
    freq->setStyleSheet("border: none; color:white; background: none");
    freq->setAlignment(Qt::AlignCenter);
    
    frameChCnt = new QGroupBox("LICZBA KANALOW",this);
    frameChCnt->setGeometry(20,304,300,122);
    frameChCnt->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    frameChCnt->setFont(QFont( "Arial", 30, QFont::Normal));
    
    ChCnt = new QLabel(frameChCnt);
    ChCnt->setGeometry(0,5,frameChCnt->width(),frameChCnt->height());
    ChCnt->setFont(QFont( "Arial",30, QFont::Normal));
    ChCnt->setStyleSheet("border: none; color:white; background: none");
    ChCnt->setAlignment(Qt::AlignCenter);
    
    scan_progress = new QSlider(Qt::Horizontal,this);
    scan_progress->setGeometry(20,576-100,680,50);
    scan_progress->setMaximum(57);
    scan_progress->setMinimum(0);
    
    channel_list = new QListWidget(this);
    channel_list->setGeometry(330,20,370,406);
    channel_list->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: rgba(0, 0, 0, 0);");
    
    channel_list->setFont(QFont( "Arial", 30, QFont::Normal));
    channel_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    channel_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   // channel_list->setAutoScroll(true);
    
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    connect(this,SIGNAL(updateProgress(int)),this,SLOT(doUpdateProgress(int)));
    connect(this,SIGNAL(sigShowMessageAfterScan(void)),this,SLOT(doShowMessageAfterScan(void)));
    connect(this,SIGNAL(AddChannelsToList(dvb_service_list*)),this,SLOT(doAddChannelsToList(dvb_service_list*)));
    
//    QString s;
//    s.append("\u25B2 \u25BC\nUP/DOWN");
//    legend2_up_down = new QLabel(s, this);
//    legend2_up_down->setObjectName("menu_item");
//    legend2_up_down->setGeometry(20+152,(576-80)+96,162,60);
//    legend2_up_down->setAlignment(Qt::AlignCenter);
//    
//    
//    QString s2;
//    s2.append("OK\nSTART SCAN");
//    legend1_wybor = new QLabel(s2, this);
//    legend1_wybor->setObjectName("menu_item");
//    legend1_wybor->setGeometry((720-20-162)+152,(576-80)+96,162,60);
//    legend1_wybor->setAlignment(Qt::AlignCenter);
//    
//    legend3_exit = new QLabel("MENU\n EXIT", this);
//    legend3_exit->setObjectName("menu_item");
//    legend3_exit->setGeometry((20+162+10)+152,(576-80)+96,162,60);
//    legend3_exit->setAlignment(Qt::AlignCenter);
//    
//    legend4_back = new QLabel("EXIT\n BACK", this);
//    legend4_back->setObjectName("menu_item");
//    legend4_back->setGeometry((20+162+10+162+10)+152,(576-80)+96,162,60);
//    legend4_back->setAlignment(Qt::AlignCenter);
    
    blockSignals(false);
    
//    QLabel *tv = new QLabel(this);
//    tv->setObjectName("simple_label");
//    tv->setGeometry(0,0,200,70);
//    tv->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
//    tv->setText("EXIT");
//    
//    QLabel *usb_sd = new QLabel(this);
//    usb_sd->setObjectName("simple_label");
//    usb_sd->setGeometry(0,(768-70),200,70);
//    usb_sd->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
//    usb_sd->setText("SCAN");
    
//        AS_Language *language = new AS_Language("#WIN_INFO",8);
//    char **Lang = NULL;
//    Lang=language->GetContext();
//    
//    QLabel *sound = new QLabel(this);
//    sound->setObjectName("simple_label");
//    sound->setGeometry(1024-200,768-70,200,70);
//    sound->setAlignment(Qt::AlignHCenter| Qt::AlignCenter);
//    sound->setText(Lang[7]);
////    sound->setText("DŹWIĘK");
//    
//    QLabel *source = new QLabel(this);
//    source->setObjectName("simple_label");
//    source->setGeometry(1024-200,0,200,70);
//    source->setAlignment(Qt::AlignCenter);
//    source->setText(Lang[6]);
////    source->setText("ŹRÓDŁO");
//    
//    delete language;
    
//    QLabel *header = new QLabel(this);
//    header->setStyleSheet("border: none; color:yellow; background: none;");
//    header->setText("AUTO SCAN");
//    header->setGeometry(112,0+20,800,50);
//    header->setAlignment(Qt::AlignCenter);
//    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
    show();
    scan_progress->show();
//    
//    source->setFont(QFont( "Arial", 35, QFont::Normal));
//    sound->setFont(QFont( "Arial", 35, QFont::Normal));
//    tv->setFont(QFont( "Arial", 35, QFont::Normal));
//    usb_sd->setFont(QFont( "Arial", 35, QFont::Normal));
//    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
//    legend2_up_down->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend1_wybor->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend3_exit->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend4_back->setFont(QFont( "Arial", 20, QFont::Normal));
    
    if(QCANrcTV::menu_from_can==true)
    {
        legend = new QRNS_Buttons(NULL,3);
        legend->show();
    }
    else
    {
        legend = NULL;
    }
}



QAutoScanWindow::~QAutoScanWindow() 
{       
    hide();
    delete scan_progress;
    delete frameCanal;
    delete frameFreq;
    
    if(legend)delete legend;
}

void QAutoScanWindow::doSigMenuAction(int action)
{
    dtv *p = ((play_ctrl*)parentWidget())->get_dtv_p();
          
                switch(action)
		  {

                  case 0x56:
		  case OK:
                  {
                    if(scanning==true) break;
                    added_channels_cnt = 0;
                    ((play_ctrl*)parentWidget())->most_scan==true;
                    was_scanned = true;
                    //ustaw flage zeby nie wejsc w skanowanie z most'u - juz skanujemy
             //       ((play_ctrl*)parentWidget())->most_scan = true;
                    
                    printf("SKANOWANIE AUTOMATYCZNE:START... %d\n",((play_ctrl*)parentWidget())->most_scan);
                    
                    p->DTV_TimeUpdateStop();
//                    ((play_ctrl*)parentWidget())->stop_sys_timer();
//                    ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_CLOSE);
                    int cnt2 = 10;
                    
                    while(cnt2--)
                    {
                        usleep(100000);
                        QCoreApplication::processEvents();
                    }

                    //zatrzymujemy odtwarzanie przed skanowaniem  
                    if(p->DTV_Is_Playing()==true) p->DTV_Stop_Play();
                    
                    
                    while (p->DTV_Is_Playing()) usleep(10000);
                    

                    if(tsi_restart()!=TSI_ERR_NONE)
                    {
                        printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
                        tsi_restart();
                    }

                    //pthread_create(&scan_thrd,NULL,auto_scan_thrd,this);  
                    qauto_scan_thrd->start();
                    scanning = true;
                  }
		    break;                

                  case 0x0a:
		  case _EXIT_:
                  {
                      
                      if(scanning==true)
                      {
                        scanning = false;
                        quit = true;
                        //pthread_join(scan_thrd,NULL);
                        while(qauto_scan_thrd->isRunning ()==true)
                          {
                            usleep(100000);
                            QCoreApplication::processEvents ();
                          }
                        quit = false;
                        
                        system("mv /media/localdata/dvb_services_old.dat /media/localdata/dvb_services.dat");
                        sync();
                        ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
//                        ((play_ctrl*)parentWidget())->start_sys_timer();
                        ((play_ctrl*)parentWidget())->StartServiceSelect(((int)0));
//                        ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_OPEN);
                      }
                    
                  //  ((play_ctrl*)parentWidget())->most_scan = false;
                    
                    
                    
//                    if(was_scanned==true)
//                    {
//                        if(total_channels_found>0)
//                        {
//                            system("rm /media/localdata/dvb_services_old.dat");
//                            sync();
//                        }
//                        else
//                        {
//                            system("mv /media/localdata/dvb_services_old.dat /media/localdata/dvb_services.dat");
//                            sync();
//                        }
//                        
//                        ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
//                        ((play_ctrl*)parentWidget())->start_sys_timer();
//                        ((play_ctrl*)parentWidget())->StartServiceSelect(((int)0));
//                    }
                    
                    
                    
                    submenu_scan *scan = new submenu_scan(parentWidget(),1);
                    delete this;
                  }
		  break;
                  
                                    
                case MENU:
                {
                    if(scanning==true)
                    {
                        scanning = false;
                        quit = true;
                        //pthread_join(scan_thrd,NULL);
                        while(qauto_scan_thrd->isRunning ()==true)
                          {
                            usleep(100000);
                            QCoreApplication::processEvents ();
                          }
                        quit = false;
                        
                         system("mv /media/localdata/dvb_services_old.dat /media/localdata/dvb_services.dat");
                        sync();
                    
                        if(((play_ctrl*)parentWidget())->most_scan==false)
                        {
                            ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
//                            ((play_ctrl*)parentWidget())->start_sys_timer();
                            ((play_ctrl*)parentWidget())->StartServiceSelect(((int)0));
//                            ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_OPEN);
                        }
                    }
                    
                    
                    delete this;
                }
                break;
                
		default:
		break;
	}
               

}

void  *QAutoScanWindow::auto_scan_thrd(void *p)
{
    QAutoScanWindow *v;
    v = ((QAutoScanWindow*)p);
    
    dvb_service_list *temp_list;
    dtv *player = ((play_ctrl*)v->parentWidget())->get_dtv_p();
    
    
    system("mv /media/localdata/dvb_services.dat /media/localdata/dvb_services_old.dat");
    sync();
    
    for(int i = 0;i<57;i++)
    {
        if(v->quit==true) break;
        
        emit v->updateProgress(i);
        
        temp_list = player->DTV_Scan(freq_t[i].freq*1000,(freq_t[i].band*1000));
        usleep(200000);
        
        if(temp_list!=NULL)
        {
            player->DTV_Save_Scan(false);
            printf("ZNALAZLEM %d KANALOW NA CZESTOTLIWOSCI %d\n",temp_list->get_list_count(),freq_t[i].freq);
            v->total_channels_found += temp_list->get_list_count();
            emit v->AddChannelsToList(temp_list);
            usleep(800000);
        }
    }
    
    v->scanning = false;
    
    if(v->quit==false) emit v->sigShowMessageAfterScan();
    
    return NULL;
}


void QAutoScanWindow::doUpdateProgress(int val)
{
    ostringstream CH;
    ostringstream FREQ;
    ostringstream CHCNT;
    ostringstream BAND;
    
    CH << (int)freq_t[val].ch;
    scan_progress->setValue(val);
    
    canal->setText((QString)CH.str().c_str());
    
    FREQ << freq_t[val].freq;
    freq->setText((QString)FREQ.str().c_str()+ " kHz");
    
    CHCNT << total_channels_found;
    ChCnt->setText((QString)CHCNT.str().c_str());

    
    BAND << (int)freq_t[val].band;
//    band->setText((QString)BAND.str().c_str() + " MHz");

}

void QAutoScanWindow::doShowMessageAfterScan(void)
{
    ((play_ctrl*)parentWidget())->most_scan = false;
    
    if(was_scanned==true)
    {
        if(total_channels_found>0)
        {
            system("rm /media/localdata/dvb_services_old.dat");
            system("rm /media/localdata/*.epg");
            sync();
        }
        else
        {
            system("mv /media/localdata/dvb_services_old.dat /media/localdata/dvb_services.dat");
            sync();
        }
        
        ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
        ((play_ctrl*)parentWidget())->start_sys_timer();
        ((play_ctrl*)parentWidget())->StartServiceSelect(((int)0));

    }
    
   
    //trzeba wyczyscic flage że wyszliśmy z menu po zak. skanowania
//    ((play_ctrl*)parentWidget())->main_menu_show_hide = false;
    ((play_ctrl*)parentWidget())->most_scan = false;
    scan_progress->setValue(0);
//    delete this;
                   
}


void QAutoScanWindow::doAddChannelsToList(dvb_service_list *l)
{
    ostringstream num;
    
    if(l!=NULL)
    {
        for(unsigned int i = 0;i<l->get_list_count();i++)
        {
            added_channels_cnt++;
            //ddoajemy numer przed nazwą stacji
            num << (int)added_channels_cnt;
  
            channel_list->addItem(QString((QString)num.str().c_str() + " " + (QString)l->get_from_list(i)->getServiceName()));

            //czyscimy numerek
            num.str(" ");
        }
        
        channel_list->scrollToBottom();
        
    }
    //channel_list->scrollToBottom();
  //  channel_list->scrollToItem (channel_list->item (channel_list->count ()-1));
    //channel_list->setAutoScroll (true);
}