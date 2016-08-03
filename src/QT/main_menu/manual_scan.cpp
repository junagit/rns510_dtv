/** 
 * @file   manual_scan.cpp
 * @author michalm
 *
 * @date   3 wrzesień 2012, 14:44
 */
#include "rc.h"
#include "manual_scan.h"
#include "submenu_scan.h"
#include <QSpinBox>
#include <QFont>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>

#include <QtGui>
#include <QtCore>


#include "dtv_core.h"
#include "play_ctrl.h"
#include "QProgressIndicator.h"
#include "app_settings.h"
#include "tsi.h"
#include "AS_language.h"
#include "QRNS_Buttons.h"
using namespace std;



manual_scan::manual_scan(QWidget* parent) : QMainWindow(parent) 
{
    
    ((play_ctrl*)parentWidget())->manual_scan=true;
    ((play_ctrl*)parentWidget())->most_scan==true;
            
    setWindowFlags(Qt::FramelessWindowHint);    
//    setGeometry(152,96,720,576);
    setGeometry(100,96,720,576);
//    setGeometry(0,0,1024,768);
    setObjectName("audio_win_grey");   
    legend = NULL;
   
    current_freq_idx = 0;
    scan_active = false;
    frameCanal = new QGroupBox("KANAL",this);
    frameCanal->setGeometry(20,20,330,70);
    frameCanal->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    frameCanal->setFont(QFont( "Arial", 20, QFont::Normal));
    frameCanal->setAlignment(Qt::AlignVertical_Mask);
    
    canal = new QLabel(frameCanal);
    canal->setGeometry(10,20,275,50);
    canal->setText(freqToStr( freq_t[current_freq_idx].ch ));
    canal->setFont(QFont( "Arial", 35, QFont::Normal));
    canal->setStyleSheet("border: none; color:white; background: none");

   
    frameFreq = new QGroupBox("CZESTOTLIWOSC",this);
    frameFreq->setGeometry(370,20,330,70);
    frameFreq->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white;");
    frameFreq->setFont(QFont( "Arial", 20, QFont::Normal));
    frameFreq->setAlignment(Qt::AlignVertical_Mask);
    
    freq = new QLabel(frameFreq);
    freq->setText(freqToStr( freq_t[current_freq_idx].freq ) + " kHz");
    freq->setGeometry(10,20,275,50);
    freq->setFont(QFont( "Arial", 35, QFont::Normal));
    freq->setStyleSheet("border: none; color:white; background: none");
    
    channel_list = new QListWidget(this);
    channel_list->setGeometry(20,110,680,365+60+21);
    channel_list->setFont(QFont( "Arial", 35, QFont::Normal));
    channel_list->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: rgba(0, 0, 0, 0);");
    
    scan_timer = new QTimer(this);
    scan_timer->setInterval(1500);
    scan_timer->setSingleShot(true);
    scan_timer->stop();
    connect(scan_timer, SIGNAL(timeout()), SLOT(doScanTimerTimeOut(void)),Qt::QueuedConnection);
    
    found_service = NULL;
    was_scaned = false;
    //rodzicem dla wait_animation musi być niewidizalne okno!!! Inaczej są jakieś problemy z odswierzaniem ekranu
    anim = new wait_animation(parentWidget());
    anim->setGeometry(anim->x()-40,anim->y()+30,anim->width(),anim->height());
//    QString s;
//    s.append("\u25B2 \u25BC\nUP/DOWN");
//    legend2_up_down = new QLabel(s, this);
//    legend2_up_down->setObjectName("menu_item");
//    legend2_up_down->setGeometry(20,(576-80),162,60);
//    legend2_up_down->setAlignment(Qt::AlignCenter);
//    
//    
//    QString s2;
//    s2.append("MENU\nEXIT");
//    legend1_wybor = new QLabel(s2, this);
//    legend1_wybor->setObjectName("menu_item");
//    legend1_wybor->setGeometry((720-20-162),(576-80),162,60);
//    legend1_wybor->setAlignment(Qt::AlignCenter);
//    
//    legend3_exit = new QLabel("+\n DODAJ", this);
//    legend3_exit->setObjectName("menu_item");
//    legend3_exit->setGeometry((20+162+10),(576-80),162,60);
//    legend3_exit->setAlignment(Qt::AlignCenter);
//    
//    legend4_back = new QLabel("EXIT\nBACK", this);
//    legend4_back->setObjectName("menu_item");
//    legend4_back->setGeometry((20+162+10+162+10),(576-80),162,60);
//    legend4_back->setAlignment(Qt::AlignCenter);
    
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
//    usb_sd->setText("DODAJ");
    
//        AS_Language *language = new AS_Language("#WIN_INFO",8);
//    char **Lang = NULL;
//    Lang=language->GetContext();
    
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
//    source->setText("ŹRÓDŁO");
    
//    delete language;
    
//    QLabel *header = new QLabel(this);
//    header->setStyleSheet("border: none; color:yellow; background: none;");
//    header->setText("MANUAL SCAN");
//    header->setGeometry(112,0+20,800,50);
//    header->setAlignment(Qt::AlignCenter);
//    header->setFont(QFont( "Arial", 50, QFont::Normal));
//    
    show();
    
//    legend2_up_down->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend1_wybor->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend3_exit->setFont(QFont( "Arial", 20, QFont::Normal));
//    legend4_back->setFont(QFont( "Arial", 20, QFont::Normal));
    
//    source->setFont(QFont( "Arial", 35, QFont::Normal));
//    sound->setFont(QFont( "Arial", 35, QFont::Normal));
//    tv->setFont(QFont( "Arial", 35, QFont::Normal));
//    usb_sd->setFont(QFont( "Arial", 35, QFont::Normal));
//    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
    channel_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    channel_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    scan_msg = NULL;
    scan_timer->start();
    
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



manual_scan::~manual_scan() 
{   
    delete anim;
    delete frameCanal;
    delete frameFreq;
    delete channel_list;
    delete scan_timer;
    
    if(legend)delete legend;
}

QString manual_scan::freqToStr(int val)
{
    ostringstream ss;
    
    ss << val;
                
    return (QString)ss.str().c_str();
}

void manual_scan::doSigMenuAction(int action)
{
    dtv *p = ((play_ctrl*)parentWidget())->get_dtv_p();
//    printf("manual_scan::doSigMenuAction:0x%x\n",action);

                switch(action)
		  {

//		  case OK:
//                      doScanTimerTimeOut();
//    
//		    break;


                 case RIGHT:   
		 case DOWN:
                     //podczas skanowania nie poruszmy sie po liscie częstotliwości
                     if(scan_active==true) break;
                     
                     if(current_freq_idx<56)
                     {
                         current_freq_idx++;
                         canal->setText(freqToStr( freq_t[current_freq_idx].ch ));
                         freq->setText(freqToStr( freq_t[current_freq_idx].freq ) + " kHz");
                         
                         scan_timer->stop();
                         scan_timer->start();
                     }
                     
                     
                     break;
                     
                 case LEFT:
                 case UP:
                     //podczas skanowania nie poruszmy sie po liscie częstotliwości
                     if(scan_active==true) break;
                     
                     if(current_freq_idx>0)
                     {
                         current_freq_idx--;
                         canal->setText(freqToStr( freq_t[current_freq_idx].ch ));
                         freq->setText(freqToStr( freq_t[current_freq_idx].freq ) + " kHz");
                         
                         scan_timer->stop();
                         scan_timer->start();
                     }
                      
                     
		   break;
                   
                  case OK:
                  case 0x56:
		  case _PLUS:
                  {
                      //jesli przeskanowalismy i nie znaleźlismy zadnych stacji to przycisk
                      //ok powoduje ponowne skanowanie
                      //jezeli zostaly znalezione jakieś stacje to ok spowoduje wyswietlenie 
                      //komunikatu o dodanych stacjach
                      if(scan_active==false) 
                      {
                          //printf("found_service->get_list_count():%d\n",found_service->get_list_count());
                          if(found_service!=NULL)
                          {
                            if(found_service->get_list_count()==0) 
                            {
                                doScanTimerTimeOut();
                                break;
                            }
                          }
                          else
                          {
                              doScanTimerTimeOut();
                              break;
                          }
                      }
                      
                      if(scan_msg!=NULL) break;
                      
                      printf("######### ZAPISZ ZNALEZIONE STACJE ##########\n");
                      if(found_service!=NULL && found_service->get_list_count()>0)
                      {
                          ostringstream num;
                          num << (int)(found_service->get_list_count());
                          QString msg = "Dodano " + (QString)num.str().c_str() + " stacji";
                          bool res = p->DTV_Save_Scan(true);
                          printf("##### DTV_Save_Manual_Scan result:%d\n",res);
                          
                          
                          
                            scan_msg = new QMessageWindow(this,0,msg);
                            scan_msg->show();
                            scan_msg->setGeometry(110,228,500,120);
                            int cnt1 = 30;
                            while(cnt1--)
                            {
                                usleep(100000);
                                QCoreApplication::processEvents ();
                            }
                            num.str(" ");
                            delete scan_msg;
                            scan_msg = NULL;
                            found_service->destroy_list();
                      }
                      else
                      {
                          printf("#### NIC NIE ZAPISUJEMY BO NIC NIE ZNALAZLEM!!!!!!1 ####\n");
                      }
                      
                  }   
		    break;


		  case _MINUS:
                
			
		    break;
                    

                  case 0x0a:
		  case _EXIT_:
                  {
                      if(scan_active==true) break;
                      
//                      tsi_release_all_resource();
                    //przed usunięciem z pamieci tej klasy musimy wznowić odtwarzanie TV
                    if(was_scaned==true)
                    {
                        //UAKTUALNIAMY LISTE KANALOW PO SKANOWANIU
                        ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
                        ((play_ctrl*)parentWidget())->start_sys_timer();
                        ((play_ctrl*)parentWidget())->StartServiceSelect(-1);
                    }
                    
                    submenu_scan *scan = new submenu_scan(parentWidget(),0);
                    ((play_ctrl*)parentWidget())->manual_scan=false;
                    ((play_ctrl*)parentWidget())->most_scan==false;
                    ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_OPEN);
                    delete this;
                  }
		  break;

                  
                case MENU:
                    if(scan_active==true) break;
                    
//                    tsi_release_all_resource();
                    if(was_scaned==true)
                    {
                        //UAKTUALNIAMY LISTE KANALOW PO SKANOWANIU
                        ((play_ctrl*)parentWidget())->UpdateChannelsNumber(false);
                        ((play_ctrl*)parentWidget())->start_sys_timer();
                        ((play_ctrl*)parentWidget())->StartServiceSelect(-1);
                    }
                    
                    ((play_ctrl*)parentWidget())->manual_scan=false;
                    ((play_ctrl*)parentWidget())->most_scan==false;
                    ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_OPEN);
                    delete this;
                break;
                
		default:
		break;
	}

}


void  *manual_scan::scan(void *p)
{
    manual_scan *v;
    v = ((manual_scan*)p);
    ostringstream num;
    
    dtv *pp = ((play_ctrl*)v->parentWidget())->get_dtv_p();
    
    //przed skanowaniem zatrzymujemy odtwarzanie TV i pobieranie epg jesli zostalo uruchomione...
    if(pp->DTV_Is_Playing()==true)
      {
        pp->DTV_Stop_Play();
      }
    
    v->found_service = pp->DTV_Scan((freq_t[v->current_freq_idx].freq*1000),(freq_t[v->current_freq_idx].band*1000));

    if(v->found_service!=NULL)
    {
      v->setUpdatesEnabled(false);
      for(unsigned int i = 0;i<v->found_service->get_list_count();i++)
      {
          
          //ddoajemy numer przed nazwą stacji
          num << (int)(i+1);
          v->channel_list->addItem(QString((QString)num.str().c_str() + " " + QString::fromUtf8(v->found_service->get_from_list(i)->getServiceName())));
          num.str(" ");
      }
      v->channel_list->scrollToBottom();
      v->setUpdatesEnabled(true);
    }
    
    v->stopAnimation();
    v->scan_active = false;
    return NULL;
}

void manual_scan::doScanTimerTimeOut(void)
{
    dtv *p = ((play_ctrl*)parentWidget())->get_dtv_p();
    
    //jezeli juz skanujemy to omijamy
    if(scan_active==true) return;

    ((play_ctrl*)parentWidget())->watchdog_ctrl(WDT_CLOSE);
    p->DTV_TimeUpdateStop();
    ((play_ctrl*)parentWidget())->stop_sys_timer();

    channel_list->clear();

    
    anim->ShowAnim(true);

    if(tsi_restart()!=TSI_ERR_NONE)
    {
        printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
        tsi_restart();
    }
    
    scan_active = true;
    was_scaned = true;
    pthread_create(&scan_thrd,NULL,scan,this);
    pthread_detach(scan_thrd);
}