/** 
 * @file   play_ctrl.cpp
 * @author Michał Motyl
 *
 * @date   8 sierpień 2012, 07:27
 */
#include <QtGui/QApplication>
#include <QtCore/qcoreapplication.h>
#include <QString>
#include <QSettings>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <QtGui/qwidget.h>

using namespace std;

#include "QTvServiceManager.h"
#include "dvbtcontrol.h"
#include "play_ctrl.h"
#include "rc.h"
#include "dtv_core.h"
#include "tv_bar.h"
#include "serialport.h"
#include "serialportinfo.h"
#include "QUserServiceList.h"
#include "QAutoScanWindow.h"
#include "gpio_ctrl.h"
#include "tsi.h"
#include "AS_language.h"
#include "version.h"
#include "main_menu_root.h"

bool skip_wdt = false;
extern volatile bool need_update_volume;
extern bool menu_flag;
volatile int cnt1 = 0;
volatile int cnt2 = 10;
volatile int cnt3 = 5*10;
volatile gint last_rendered_frame = -1;

int play_ctrl::current_ch_index = 0;
int play_ctrl::menu_root = false;

play_ctrl::play_ctrl(QWidget* parent) : QMainWindow(parent)
{
    app_settings settings_temp;
    wait4constructor = true;
    tv_init_ok = false;
    need_send_list = false;
    quick_list_show_flag = false;
    opt_win_flag = false;
    
    set_gpio_output(3,22,0);
    
    anim = new wait_animation(this);
 
    media_player = NULL;
    can_rc4tv   = new QCANrcTV(this);
    can_rc4tv->disableCANsignals(true);

    tv_player = new dtv();
    ch_sw     = new QChannelSwitchThrd(this);
    
    setFocusPolicy(Qt::NoFocus);
    //============== Tymczasowy wątek inicjalizacji TV(tuner,itp)===
    init_thrd = new QInitThread(this);
    init_thrd->start(QThread::HighestPriority);
                                
    splash = NULL;
    char buf[255];
    
    if(access("/tmp/crashed", F_OK)!= 0)
    {
        if(settings_temp.get_tv_state()==1)
        {
            //logo "Starting TV"
            QPixmap pixmap1(":/img/Czolowka2b.png"); // Insert your splash page image here
            splash = new QSplashScreen(this,pixmap1);   
        }
        else
        {
            //logo "Starting MEDIA PLAYER"
            QPixmap pixmap2(":/img/Czolowka3b.png"); // Insert your splash page image here
            splash = new QSplashScreen(this,pixmap2);   
        }
        
        memset(buf,0,255);
        sprintf(buf,"\n     JMC-03 V2.1 / ITV-M2_1_02 \n     0.2 %s",__DATE__);
        splash->setFont(QFont( "Arial", 25, QFont::Normal));
        splash->showMessage(QString(buf),Qt::AlignLeft | Qt::AlignTop, Qt::white); 
        splash->show();
    }
    
        //=================================================================================================
    quick_list_show_flag = false;
    //=================================================================================================    
    manual_scan = false;
    current_ch_index = 0;
    total_channles_num = 0;
    current_volume = 1;
    epg_is_actual = false;
    main_menu_show_hide = false;
    mute = false;
    epg_win_show_hide = false;
    ch_switch_timeout = 5*7;//s
    system_info = NULL;
    bad_signal_cnt = BAD_SIGNAL_TIMEOUT;
    no_signal = false;
    selecting_from_most = false;
    freq_was_locked = false;
    current_ch_index_tmp = 0;
    quick_list_show = false;
    mute_icon = NULL;
    most_scan = false;
    need_change = false;
    more = NULL;                
    more_is_showed = false;
    system_info_show_flag = false;
    stop_tv_from_most = false;
    info_was_found = false;
    different_channel = false;
    watchdog_fd = -1;
    //============== RS 232 =========================================
    rs_232 = new SerialPort(this);
    rs_232->setPort("/dev/ttyS2");
    QIODevice::OpenMode mode = QIODevice::ReadWrite | QIODevice::Unbuffered;

    if (rs_232->open(mode)) 
    {
        rs_232->setDataBits(SerialPort::Data8);
        rs_232->setStopBits(SerialPort::OneStop);
        rs_232->setFlowControl(SerialPort::NoFlowControl);
        rs_232->setRate(SerialPort::Rate19200);
        rs_232->setParity(SerialPort::NoParity); 
        rs_232->setReadBufferSize(10);
        
        connect(rs_232, SIGNAL(readyRead()), this, SLOT(procReadyRead()));
    }
    else
    {
//        printf("####### BLAD OTWIERANIA PORTU RS232!!!!!!! #######\n");
        doSigSystemInfo(true,1,QString::fromUtf8("ERROR 001"));
    }
    //==============================================================
    //ustawienia aplikacji
    QCoreApplication::setOrganizationName("JUNA");
    QCoreApplication::setOrganizationDomain("juna.com");
    QCoreApplication::setApplicationName("DTV");
    settings = new app_settings();
    current_ch_index = settings->get_channel_idx();
    
#ifdef VOLUME_CTRL
    current_volume = settings->get_volume();
    //ustaw glosnosc audio zawsze>0
    if(current_volume==0) current_volume=1;
#else
    current_volume = settings->getSystemVolume();
    //ustaw glosnosc audio zawsze>0
    if(current_volume==0) current_volume=0.5;
#endif
    //==============================================================
    setGeometry(0,0,1024,768);
    setWindowFlags(Qt::FramelessWindowHint);

    //=============== tworzymy pasek informacyjny ==================
    main_bar = NULL;
    main_bar_timer = new QTimer(this);
    int cnt_tmp = settings->get_osd_time();
    
    if(cnt_tmp>0) main_bar_timer->setInterval((settings->get_osd_time()*1000));
    
    main_bar_timer->setSingleShot(true);
    main_bar_timer->stop();
    connect(main_bar_timer, SIGNAL(timeout()), SLOT(doMainBarShowTimeOut(void)),Qt::QueuedConnection);

    //==============================================================
    connect(this,SIGNAL(SigSystemInfo(bool,int,QString)),this,SLOT(doSigSystemInfo(bool,int,QString)),Qt::QueuedConnection);
    //########  timer do przelaczania kanalu pracujący w trybie wyzwalania po czasie #########
    ch_sw_timeout = new QTimer(this);
    ch_sw_timeout->setInterval(1500);
    ch_sw_timeout->setSingleShot(true);
    connect(ch_sw_timeout, SIGNAL(timeout()), SLOT(DoChannelSwitch()));
    ch_sw_timeout->stop();
    //############################################################
    debug_timer = new QTimer(this);
    debug_timer->setInterval(TIMER_DELAY);
    connect(debug_timer, SIGNAL(timeout()), SLOT(DoDEBUG()),Qt::QueuedConnection);
    debug_timer->stop();
    
    //czekamy na zakonczenie sie wątku inicjalizacyjnego
    init_thrd->wait();
    delete init_thrd;
    init_thrd = NULL;
//########################################################################################################
    connect(can_rc4tv, SIGNAL(sig_can_rc_code(char)),this,SLOT(do_sig_can_rc_code(char)),Qt::DirectConnection);
    connect(can_rc4tv, SIGNAL(ChSwitchFromMOST(int)),this,SLOT(DoChannelSwitchFromMOST(int)),Qt::QueuedConnection);
    connect(can_rc4tv, SIGNAL(epgFromMOST(bool)),this,SLOT(doEpgFromMOST(bool)),Qt::QueuedConnection);
    connect(can_rc4tv, SIGNAL(sigTvOnOff(bool)),this,SLOT(doSigTvOnOff(bool)),Qt::QueuedConnection);
//########################################################################################################
    //odczyt stanu czy bylismy w TV czy media player
    int set = settings->get_tv_state();
    
    //domyslnie ustaw TV
    set = 1;
    
    if(set==1)
    {//jesli bylismy w TV to zaczynamy odtwarzać TV
        stop_tv_from_most = false;
        can_rc4tv->disableCANsignals(true);

        if(total_channles_num>0)
        {
            can_rc4tv->disableCANsignals(false);
            can_rc4tv->sendServiceList();
            //po inicjalizacji tunera i odczycie settingow zaczynamy odtwarzać ostatni kanał
            ch_sw->start(QThread::HighestPriority);
        }
        else
        {
            can_rc4tv->disableCANsignals(false);
            AS_Language *language = new AS_Language("#WIN_INFO",6);
            char **Lang = NULL;
            Lang=language->GetContext();
                        
            doSigSystemInfo(true,0,Lang[1]);
//            can_rc4tv->disableCANsignals(false);
            delete language;
        }
        debug_timer->start();
        watchdog_ctrl(WDT_OPEN);
    }
    else
    {//wejdź w media player
        while(isSelecting()) usleep(200000);
        debug_timer->stop();
        stop_tv_from_most = true;
        can_rc4tv->disableCANsignals(true);
        media_player = new QMediaPlayerMain(this);
        connect(media_player, SIGNAL(sigTvOnOff(bool)),this,SLOT(doSigTvOnOff(bool)),Qt::QueuedConnection);
        connect(media_player, SIGNAL(menuEnable(bool)),this,SLOT(domenuEnable(bool)));
        this->hide();
    }

    wait4constructor = false;
//########################################################################################################
}



play_ctrl::~play_ctrl() 
{
    delete can_rc4tv;
    delete tv_player;
 
    if(main_bar!=NULL) delete main_bar;
    
#ifdef VOLUME_CTRL
    if(volume_window!=NULL) delete volume_window;
    
    delete volume_timer;
#endif
    delete main_bar_timer;
    
    rs_232->close();
    delete rs_232;
    delete ch_sw_timeout;    
    delete settings;
}



bool play_ctrl::parse_rc_cmd(char code)
{
bool res = false;
int saved_ch_idx;

                  switch(code)
		  {
                       case  P0:
                       case  P1:
                       case  P2:
                       case  P3:
                       case  P4:
                       case  P5:
                       case  P6:
                       case  P7:
                       case  P8:
                       case  P9:
                           
                        if(epg_win_show_hide==true) break;

                        if(main_menu_show_hide==true) break;

                        if(isSelecting()==true) break;
                           
                        ch_idx_tmp.append(rcCode2digit(code));
                        current_ch_index_tmp = ch_idx_tmp.toInt(NULL,10);
                        saved_ch_idx = current_ch_index;
                        
                            if(current_ch_index_tmp<total_channles_num && current_ch_index_tmp>=0)
                            {
                                if(current_ch_index_tmp>0) current_ch_index_tmp--;
                                
                                current_ch_index = current_ch_index_tmp;
                              
                                doSigShowMainBar();
                            }
                            else
                            {
                                ch_sw_timeout->stop(); 
                                current_ch_index = saved_ch_idx;
                                current_ch_index_tmp = 0;
                                break;
                            }
                                ch_sw_timeout->stop(); 
                                ch_sw_timeout->start(); 
                          break;
                               


		 case DOWN:
                 case UP:

                     //zmiana kanału
                     if(tv_init_ok==false) 
                    {
                        PLAY_CTRL_DEBUG("TV nie zainicjalizowane!!!\n");
                        return false;
                    }
                     
                     if(total_channles_num==0) 
                     {// na wypadek gdy gry nie liczba stacji = 0, nie możemy przełączać
                        return false;
                     }

                     if(epg_win_show_hide==true) return false;
                     
                     if(quick_list_show==false)
                     {
                        if(!(epg_win_show_hide==true || main_menu_show_hide==true))
                        {
                            //jezeli jestesmy w trakcje wybierania kanalu to
                           //nie zwiekszamy indeksu kanalu do przelaczenia
                           //if(selecting==true) break;

                           if(more_is_showed==true) show_more_info(false);
                           
                           if(code==DOWN)
                           {
                                if(current_ch_index>0) current_ch_index--;
                                else
                                {
                                    current_ch_index = total_channles_num-1;
                                }
                           }else
                           {
                               current_ch_index = (current_ch_index+1)%total_channles_num;
                           }
                           //uaktualniamy informacje na pasku
                           doSigShowMainBar();
                           
                           ch_sw_timeout->stop(); 
                           ch_sw_timeout->start(); 
                           res = true;
                        }
                     }
                     else
                     {
//                         if(code==DOWN) quick_list->nextItem();
//                         else
//                           quick_list->previousItem();  
                     }
		    break;
            
                   
                   case LEFT:
                   case RIGHT:
                       skip_wdt = true;

#ifdef VOLUME_CTRL

                       if(tv_init_ok==false) 
                        {
                            PLAY_CTRL_DEBUG("TV nie zainicjalizowane!!!\n");
                            return false;
                        }
                       
                        if(epg_win_show_hide==true) break;

                        if(main_menu_show_hide==true) break;

                            if(code==LEFT) 
                            {
                                if(current_volume>0)current_volume--;
                            }
                            else
                            {
                                if(current_volume<10) current_volume++;
                            }
                            
                            //jesli bylo mute to je kasujemy i ustawiamy glosnosc
                            if(tv_player->DTV_Is_Playing()==true) 
                            {
                                mute = false;
                                showMute(mute); 
                                tv_player->DTV_Set_Volume((double)current_volume);
                            }

                       doSigShowVolumeWindow(current_volume);
#endif
		    break;

		 
                  case INFO:
                  {//sekwencja załaczania: belka główna -> dodatkowe info -> off
                    if(tv_init_ok==false || total_channles_num==0) 
                    {
                        PLAY_CTRL_DEBUG("TV nie zainicjalizowane!!!\n");
                        return false;
                    }  
                    //jesli pokazane jest okno epg to ni pokazujemy okna podlaszania lub glownego paska
                    if(main_menu_show_hide==true || epg_win_show_hide==true) break;
                    
                    bool bar_state = false;
                    
                    if(main_bar!=NULL) bar_state = true;

                    if(bar_state==true) 
                    {
                        doSigHideMainBar();
                        show_more_info(true);
                    }
                    else
                    {
                        if(more_is_showed==true) show_more_info(false);
                        else
                        {
                            doSigShowMainBar();
                            
                            if(main_bar!=NULL) bar_state = true;
                            else
                            bar_state = false;
                            //jesli nadal po przycisnieciu info nie jest pokazane to oznacza ze zostało
                            //wylaczone weswietlanie paska w menu i pokazujemy dodatkowe info?
                            if(bar_state==false) 
                            {
                                //jesli jest brak sygnalu i pokazujemy more_info to chowamy info o braku sygnalu
                                if(system_info_show_flag==true) doSigSystemInfo(false,0,"");
                                
                                show_more_info(true);
                            }
                        }
                    }              
                  }
                   break;
                   
                    case  _MUTE:
                    {
                        if(tv_player->DTV_Is_Playing()==true)
                        {
                            mute = !mute;
                            tv_player->DTV_Mute(mute);         
                            showMute(mute);      
                        }
                    }
                    break;

		  case _EXIT_:
                     
                      showMute(false);
                      if(main_menu_show_hide==true)
                      {
                          emit sigMenuAction(_EXIT_);//cofamy sie w menu
                          break;
                      }
                      
		  break;


                case _TV_R:
                    doStartMediaMplayer();

                break;
               
                    
                case MENU:

                    if(tv_init_ok==false) 
                    {
                        PLAY_CTRL_DEBUG("TV nie zainicjalizowane!!!\n");
                        return false;
                    }
                                           

                    if(more_is_showed==true) show_more_info(false);
                    
                    if(system_info_show_flag==true) doSigSystemInfo(false,0,"");

                    //jesli jestesmy w skanowaniu wywolanym z most'a to musimy wyjść z tego skanowania
                    if(most_scan==true)
                    {
                        most_scan = false; 
                        emit sigMenuAction(MENU);
                        usleep(100000);
                    }
                    
                    if(main_menu_show_hide==false)
                    {
                        main_menu_show_hide = true;
               
                        doSigHideMainBar();
                        doSigCreateMainMenu();
                        menu_flag = true;
                    }
                    else
                    {
                        main_menu_show_hide = false;
                        //jesli nie ma sygnalu to pokazujemy info
                        //bo przed wejsciem do menu je chowamy
                        AS_Language *language = new AS_Language("#WIN_INFO",6);
                        char **Lang = NULL;
                        Lang=language->GetContext();
                       // QMutexLocker ml(&mMutex);
                        if(no_signal==true && total_channles_num>0) 
                        {
                            //chowamy okienko more
                            if(more_is_showed==true) show_more_info(false);
                            
                            doSigSystemInfo(true,0,QString::fromUtf8(Lang[0]));
                        }
                        else
                        { 
                            if(total_channles_num==0) doSigSystemInfo(true,0,QString::fromUtf8(Lang[1]));
                        }
                        
                        emit sigMenuAction(MENU);
                        delete language;
                        menu_flag = false;
                        QCANrcTV::menu_from_can = false;
                    }
                break;
                

		default:
//                    if(code==0x56)do_sig_can_rc_code(0x55);
//                    else
//                    PLAY_CTRL_DEBUG("NIEZNANA KOMENDA... 0x%x\n",code);
		break;
	}

    return res;
                 
}

void play_ctrl::update_bar_info()
{
ostringstream ss;
char tt[32];
AS_Language *language = new AS_Language("#WIN_INFO",6);
char **Lang = NULL;
Lang=language->GetContext();
    
eit_event *current_event = NULL;
time_t t = time(NULL);
struct tm tm1 = *localtime(&t);

    if(main_bar==NULL) return;

    int tmp = (current_ch_index + 1);
    memset(tt,0,32);
    sprintf(tt,"%02d:%02d",tm1.tm_hour,tm1.tm_min);
    ss << tmp;//+ 1 bo indeksy na liście zaczynają sie od zera 0->1 1->2 itd;
    
    main_bar->set_current_time((QString)tt);
    main_bar->set_current_channel_number((QString)ss.str().c_str());
  
   
    dvb_service *s = tv_player->DTV_Index_To_Service(current_ch_index);
    
    if(s!=NULL)
    {
        main_bar->set_current_channel(QString::fromUtf8(s->getServiceName()));
        current_event = tv_player->DTV_Get_Current_Event(s);

        if(current_event!=NULL) 
        {
                //wysylamy do mostu aktualną pozycję
                //dziala tak jak belka na dole ekranu
            if(current_event->title!=NULL) main_bar->set_current_event_name(QString::fromUtf8(current_event->title));
            
            if(tv_player->DTV_TimeIsSet()==false)
                {
                    //jezeli czas jet nieustawiony to trzeba schować pasek postepu i godzinę rozp. i zakonczenia    
                    main_bar->fill_progress(NULL);
                }
                else
                main_bar->fill_progress(current_event);
                
            delete language;
            return;
        }
        else
        {  
            current_event = tv_player->DTV_Get_Current_Event_From_File(s);
            if(current_event!=NULL)
            {
                if(current_event->title!=NULL)main_bar->set_current_event_name(QString::fromUtf8(current_event->title)); 
                
                if(tv_player->DTV_TimeIsSet()==false)
                {
                    //jezeli czas jet nieustawiony to trzeba schować pasek postepu i godzinę rozp. i zakonczenia
                    main_bar->fill_progress(NULL);
                }
                else
                main_bar->fill_progress(current_event);
                
                delete language;
                return;
            }
            else
            {
                main_bar->set_current_event_name(QString::fromUtf8(Lang[2]));
             
                //jezeli nie ma informacji to trzeba schować pasek postepu i godzinę rozp. i zakonczenia
                main_bar->fill_progress(NULL);
            }
        }
    }
    delete language;
}





extern bool menu_flag;
void play_ctrl::doSigCreateMainMenu(void)
{
//    main_menu *m = new main_menu(this,0);   
    main_menu_root *m = new main_menu_root(this);   
//    connect(m, SIGNAL(startMediaMplayer(void)),this,SLOT(doStartMediaMplayer(void)));
    menu_flag = true;
}

void play_ctrl::doSigSystemInfo(bool f,int type,QString msg)
{
    if(f==true)
    {
        if(system_info_show_flag==true) return;
        
        if(system_info==NULL)
        {
          system_info = new QMessageWindow(this,type,msg);
          system_info_show_flag = true;
        }  
    }
    else
    {
        if(system_info_show_flag==false) return;
        
        if(system_info!=NULL)
          {
              delete system_info;
              system_info = NULL;
              system_info_show_flag = false;
          }
    }
}


void play_ctrl::procReadyRead()
{
    bool res;

     if ((rs_232->bytesAvailable() > 0)) 
     {
        QByteArray data = rs_232->read(4);
        
        if(data.at(0)==0x50)
        {
            //jesli jestesmy w oodtwarzaczu to wyslij sygnal z kodem klawisza pilota
            if(stop_tv_from_most==true)
            {
                if(data.at(1)==0x57)
                {
                    set_gpio_output(0,21,0);
                    sleep(1);
                    app_quit();
                    return;
                }
//===============================================================================================                
                //jezeli jestesmy w odtwarzaczu mediow i ktos chce wejsc w menu glowne
                if(main_menu_show_hide==false && data.at(1)==MENU)
                {
//                    //printf("WLACZ MENU GLOWNE PODCZAS MEDIA PLAYER...\n");
//                    #ifdef ENABLE_MOST
//                    //rozlaczamy sygnal od sterowania pilotem dla klasy mostu
//                    disconnect(this,SIGNAL(sigMenuAction(int)),media_player,SLOT(doSigMenuAction(int)));
//                    #endif
//                    //==========================================================
//                    //linie ponizej powoduja wylonienie sie okna menu glownego 
//                    //dla ktorego rodzicem jest klasa player_ctrl
//                    showNormal();
//                    raise();
//                    activateWindow();
//                    //==========================================================
//                    parse_rc_cmd(data.at(1));
//                    
//                    return;
                }
                else
                {
//                   if(main_menu_show_hide==true && data.at(1)==MENU) 
//                   {
//                      // printf("WYLACZ MENU GLOWNE PODCZAS MEDIA PLAYER...\n");
//                       emit sigMenuAction(data.at(1));
//                       main_menu_show_hide=false;
//                       //odlaczamy sygnal od sterowania pilotem dla klasy mostu
//                       
//                       connect(this,SIGNAL(sigMenuAction(int)),media_player,SLOT(doSigMenuAction(int)),Qt::QueuedConnection);
//                       return;
//                   }
//                   else
                   {
                       emit sigMenuAction(data.at(1));
                       return;
                   }
                }
//===============================================================================================                
                emit sigMenuAction(data.at(1));
                return;
            }
            else
            {
                if(main_menu_show_hide==true || epg_win_show_hide==true || more_is_showed==true)
                {
                   if(main_menu_show_hide==true && data.at(1)==MENU) 
                   {
                       can_rc4tv->disableCANsignals(false);
                       main_menu_show_hide=false;
                       menu_flag = false;
                       QCANrcTV::menu_from_can = false;
                   }
                   else
                   {
                       if(data.at(1)==MENU) can_rc4tv->disableCANsignals(false);
                       
                        if(more_is_showed==true && data.at(1)==INFO) 
                        {
                            parse_rc_cmd(data.at(1));
                            return;
                        }
                        else
                        {
                            if(epg_win_show_hide==true && data.at(1)==EPG) 
                            {
                                parse_rc_cmd(data.at(1));
                                return;
                            }
                        }
                   }
                   
                    emit sigMenuAction(data.at(1));
                }
                else
                {
                    res = parse_rc_cmd(data.at(1));
                }
            }
        }
        else
        {
            if(data.at(1)==0x57)
            {
                set_gpio_output(0,21,0);
                sleep(1);
                app_quit();
            }
        }
    } 
}


void play_ctrl::DoChannelSwitch()
{
    //ustaw flage żeby wyświetlić rozetkę gdy przełaczamy kanał z pilota
    //Przypadek:brak sygnału i chcemy przełaczyc kanał o jeden do przodu lub do tyłu
    different_channel = true;
    StartServiceSelect(-1);
}


void play_ctrl::DoDEBUG()
{   
    gint rendered;
    watchdog_ctrl(WDT_REFRESH);

    //migamy diodą na płytce
    set_gpio_value(3,22,!get_gpio_value(3,22));
    
    if(tv_init_ok==false) return;
    
    if(most_scan==true) return;
    
    if(isSelecting()) 
    {
        cnt1 = 0;
        return;
    }
    
    //jesli bylismy w trakcie przelaczania kanalu to zostaje ustawiona flaga
    //oznaczajaca ze musimy przelaczyc kanal
    if(need_change==true)
    {
        need_change=false;
        //StartServiceSelect(-1);
        ch_sw_timeout->singleShot(10,this,SLOT(DoChannelSwitch()));
        return;
    }
    
    //jezeli wyslwietlone jest okno more to uaktualniamy info co 2 sekundy
    if(more_is_showed==true)
    {
        if(!cnt2--)
        {
            cnt2 = 10;
            more->update_info();
        }
    }
    
//================================================================================
    if(tv_player->DTV_Is_Playing() && isSelecting()==false)
    {
        if(tv_player->DTV_need_resrtart())
        {
            cnt1++;
            if(cnt1>=10)
            {
               cnt1 = 0;
               tv_player->DTV_Stop_Play();
               if(tsi_restart()!=TSI_ERR_NONE)
               {
                   printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
                   tsi_restart();
               }
               
               if(need_change==true) need_change=false;
               
               ch_sw_timeout->singleShot(50,this,SLOT(DoChannelSwitch()));
               return;
            }
        }
        else
        {
            if(cnt1>0)cnt1--;
        }
    }
//================================================================================   
    HandleTvSignal();
//================================================================================   
//obsluga bug-a modułu TSI SSD1938
//gdy odbierane dane są niprawidlowe to crc dancyh odbieranych
//z TSI jest niepoprawne. Gdy wystepuje efekt "ćwierkania" to wystepuje caly czas blad crc
//licznik crc jest zwiekaszany w funkcji pobierajacej epg present/following
    if(tv_player->DTV_Is_Playing())
    {
        if(need_update_volume==true)
        {
           need_update_volume = false;
           double vol = settings->getSystemVolume();
           tv_player->DTV_Set_Volume(vol);
        }
        
//================================================================================
//ZAPEZPIECZENIE DLA NASTĘPUJĄCEGO PRZYPADKU:
//podczas odtwarzania może wystapić zatrzymanie obrazu,natomiast
//audio jest ok. Sposobem na wykrycie tego bledu jest sprawdzanie
//licznika zliczajacego ilość wyświtlonych ramek. Jeżeli ta liczba się
//zmienia to teoretycznie wszystko jest OK. Jęzeli ten licznik się nie
//zmieni w ciągu 10s to uznajemy to za bład i przelaczamy kanał
        if(last_rendered_frame>0)
        {
            tv_player->dvb_player->GetRenderedFrames(&rendered);
            
            if(rendered!=last_rendered_frame)
            {
                last_rendered_frame = rendered;
                cnt3 = 5*10;
            }
            else
            {
                if(!cnt3--)
                {
                    cnt3 = 5*10;
                    last_rendered_frame = -1;
                    
                    tv_player->DTV_Stop_Play();
                    if(tsi_restart()!=TSI_ERR_NONE)
                    {
//                        printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
                        tsi_restart();
                    }
                    //StartServiceSelect(-1);
                    if(need_change==true) need_change=false;

                    ch_sw_timeout->singleShot(50,this,SLOT(DoChannelSwitch()));
                    return;
                }
            }
        }
        else
        {
            tv_player->dvb_player->GetRenderedFrames(&rendered);
            last_rendered_frame = rendered;
       //     printf("GetRenderedFrames:%d\n",rendered);
        }
//================================================================================
        if(isSelecting()==false)
        {
            int crc1 = tv_player->epg.get_crc_err_cnt1();
            int crc2 = tv_player->epg.get_crc_err_cnt2();
            
            if(crc1>8 || crc2>8)
            {
                printf("#### CHYBA SIE WYJEBALEM!!!!\n");
                
                tv_player->DTV_Stop_Play();
                if(tsi_restart()!=TSI_ERR_NONE)
                {
                    printf("@@@@ tsi_restart()!=TSI_ERR_NONE\n");
                    tsi_restart();
                }
                
                tv_player->epg.clear_err_cnt1();
                tv_player->epg.clear_err_cnt2();
                
                //StartServiceSelect(-1);
                if(need_change==true) need_change=false;
                
                ch_sw_timeout->singleShot(50,this,SLOT(DoChannelSwitch()));
                return;
            }
        }
    }
    else
    {
        tv_player->epg.clear_err_cnt1();
        tv_player->epg.clear_err_cnt2();
    }
    
//================================================================================
    if(signal_is_bad()==true) 
    {
        if(!(main_menu_show_hide==true || epg_win_show_hide==true || more_is_showed==true || quick_list_show_flag==true)) 
        {
            if(tv_player->DTV_Is_Playing()==false && isSelecting()==false) 
            {
                AS_Language *language = new AS_Language("#WIN_INFO",6);
                char **Lang = NULL;
                Lang=language->GetContext();
    
                if(more_is_showed==true) show_more_info(false);
                
                doSigSystemInfo(true,0,QString::fromUtf8(Lang[0]));
                delete language;
            }
        }
        else
        {//zabezpieczenie - jezeli nie ma żadnych stacji to żeby nie schował komunikatu
            doSigSystemInfo(false,0,QString::fromUtf8(" "));
        }
        return;
    }
    else
    {//zabezpieczenie - jezeli nie ma żadnych stacji to żeby nie schował komunikatu
        if(isSelecting()==false && total_channles_num>0)
        {
            if(system_info_show_flag)doSigSystemInfo(false,0,"");
        }
        else
        if(total_channles_num==0 && !(main_menu_show_hide==true || epg_win_show_hide==true || more_is_showed==true || quick_list_show_flag==true))
        {
            //żeby wkółko nie odsieżać tego samego napisu sprawdzamy flagę czy okno jest wyswietlone
            if(system_info_show_flag==false)
            {
                AS_Language *language = new AS_Language("#WIN_INFO",6);
                char **Lang = NULL;
                Lang=language->GetContext();
                doSigSystemInfo(true,0,QString::fromUtf8(Lang[1]));
                delete language;
                return;
            }
        }
    }

    if(tv_player->DTV_TimeIsSet()==true && tv_player->DTV_Is_Playing()==true)
    {
        if(isSelecting()==false)
        {
            if(tv_player->DTV_GetCurrent_Service()==NULL) return;

            if(tv_player->DTV_EPG_Is_Ready_For_Service(tv_player->DTV_GetCurrent_Service())==false || tv_player->DTV_EPG_Is_Actuall_For_Service(tv_player->DTV_GetCurrent_Service())==false)
            {
                //stacja posiada epg?
                if(tv_player->DTV_GetCurrent_Service()->haveEPG()==true)
                {
                    tv_player->DTV_EPG_Start_Update();
                }
            }
        }
    }
    else
    {
        if(isSelecting()==false && tv_player->DTV_Is_Playing()==true && tv_player->DTV_TimeIsSet()==false) tv_player->DTV_TimeUpdate();
    }
    
}


/**
 * @brief Funkcja sprawdza stan sygnalu TV i zatrzymuje lub wznawia odtwarzanie TV po zaniku i pojawieniu sie sygnalu
 *        oraz wyświetla okno informacyjne "BRAK SYGNALU"      
 */
void play_ctrl::HandleTvSignal(void)
{
    uint32_t ber_temp = 0;
    uint32_t sig_temp = 0;
    
    if(isSelecting()==true || total_channles_num==0) return;//jesli przelaczamy kanal lub nie ma zapisanych stacji to nie sprawdzamy sygnalu 
        
    if(freq_was_locked)
    { 
        HW_ReadStatus(&sig_temp,&ber_temp);
        
        if (sig_temp<10) sig_temp=0;
        else
            sig_temp = 1;

        signal_level = sig_temp;
        signal_ber = ber_temp;
    }
    else
    {
        signal_ber = 0;
        signal_level = 0;
    }
   
   // printf("sig_temp:%d ber_temp:%d bad_signal_cnt%d\n",sig_temp,ber_temp,bad_signal_cnt);
    
            if(tv_player->DTV_Is_Playing()==true)
            {
                if(signal_level==0 || signal_ber>=MAX_BER_LEVEL)
                {
                    if(bad_signal_cnt>0)bad_signal_cnt--;//lub odczyt sily sygnalu
                }
                else
                {
                    bad_signal_cnt = BAD_SIGNAL_TIMEOUT;    
                    return;
                }
                
                
                if(bad_signal_cnt==0)
                {
                    DTV_CORE_DEBUG("Braklo sygnalu podczas odtwarzania... Zatrzymuje odtwarzanie!!!\n");
                 //   QMutexLocker ml(&mMutex);
                    no_signal = true;

                    tv_player->DTV_Stop_Play();
                    bad_signal_cnt = 2;      

                    if(more_is_showed==false)
                    {
                        AS_Language *language = new AS_Language("#WIN_INFO",6);
                        char **Lang = NULL;
                        Lang=language->GetContext();

//                        if(more_is_showed==true) show_more_info(false);

                        doSigSystemInfo(true,0,QString::fromUtf8(Lang[0]));
                        delete language;
                    }
                    
                    return;
                }
            }
            else
            {
                if(freq_was_locked==false)
                {
                    if(bad_signal_cnt--==0)
                    {
                        bad_signal_cnt = BAD_SIGNAL_TIMEOUT;
                        StartServiceSelect(-1);     
                        return;
                    }
                }
                else
                {
                    //czekamy na stabilny sygnal
                    if(signal_level>0)
                    {
                        if(signal_ber<MAX_BER_LEVEL)
                        {
                            if(bad_signal_cnt--==0)
                            {
                                bad_signal_cnt = BAD_SIGNAL_TIMEOUT;
                                StartServiceSelect(-1);
                                return;
                            }
                        }
                        else
                        {
                            bad_signal_cnt = 2;
                            return;
                        }
                    }
                    else
                    {
                        bad_signal_cnt = 2;
                        return;
                    }
                }
            }
}

/**
 * @brief Funkcja zamienia kod pilota na string(cyfry 0-9)
 * @param rc_code
 * @return 
 */
char* play_ctrl::rcCode2digit(char rc_code)
{
    if(rc_code==P0) return "0";
    if(rc_code==P1) return "1";
    if(rc_code==P2) return "2";
    if(rc_code==P3) return "3";
    if(rc_code==P4) return "4";
    if(rc_code==P5) return "5";
    if(rc_code==P6) return "6";
    if(rc_code==P7) return "7";
    if(rc_code==P8) return "8";
    if(rc_code==P9) return "9";
    
    return NULL;
}

void play_ctrl::showMute(bool f)
{
    if(f==true)
    {
        if(mute_icon!=NULL) return;
     
        mute_icon = new QLabel(this);
        mute_icon->setObjectName("simple_label");
        mute_icon->setGeometry(640,10,65,65);
        mute_icon->setPixmap(QPixmap(QString::fromUtf8(":/img/mute64x64.png")));
        mute_icon->show();
    }
    else
    {
        if(mute_icon==NULL) return;
 
        delete mute_icon;
        mute_icon = NULL;
    }
   
}

void play_ctrl::watchdog_ctrl(wdt_ctrl cmd)
{
    int interval = 7;

    if(cmd==WDT_OPEN)
    {
        if(!(watchdog_fd>0))
        {
            watchdog_fd = ::open("/dev/watchdog", O_RDWR);
            printf("@@@@ cmd==WDT_OPEN @@@\n");
            if (-1 == watchdog_fd) 
            {
              fprintf(stderr, "Error: %s\n", strerror(errno));
              return;
            }
            
            if (ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &interval) != 0)
            {
                fprintf(stderr,"Error: Set watchdog interval failed\n");
              //  ::close(fd);
                return;
            }
        }
    }
    
    //off
    if(cmd==WDT_CLOSE)
    {
        if(watchdog_fd>0)
        {
            printf("@@@@ cmd==WDT_CLOSE @@@\n");
            ::write(watchdog_fd, "V", 1);
            ::close(watchdog_fd);
            watchdog_fd = -1;
        }
    }
    
    
    //kick
    if(cmd==WDT_REFRESH)
    {
        if(watchdog_fd>0)
        {       
//            printf("@@@@ cmd==WDT_REFRESH @@@\n");
            ::write(watchdog_fd, "w", 1);
        }
    }
    
    return;
}

void play_ctrl::show_more_info(bool f)
{
     if(most_scan==true) return;
     
     if(main_menu_show_hide==true || epg_win_show_hide==true) return;
     
    if(f==true)
    {
        if(more==NULL)
        {
            can_rc4tv->disableCANsignals(true);
   
            more = new QMoreInfoWindow(NULL,this);
            more->show_info();
            more_is_showed = true;
            can_rc4tv->disableCANsignals(true);
            more->show();
            //WYSIWTL OKNO MORE i wyslij liste do mostu z elemnetami sterujacymi
            connect(more, SIGNAL(sigQuitFromMore(void)),this,SLOT(doSigQuitFromMore(void)));
            connect(more, SIGNAL(ChSwitchFromMOST(int)),this,SLOT(DoChannelSwitchFromMOST(int)));
//            connect(more, SIGNAL(startMediaMplayer(void)),this,SLOT(doStartMediaMplayer(void)));
        }
        else
        {
            more->show();
            more->show_info();
            more_is_showed = true;
            can_rc4tv->disableCANsignals(true);
            //WYSIWTL OKNO MORE i wyslij liste do mostu z elemnetami sterujacymi
            connect(more, SIGNAL(sigQuitFromMore(void)),this,SLOT(doSigQuitFromMore(void)));
            connect(more, SIGNAL(ChSwitchFromMOST(int)),this,SLOT(DoChannelSwitchFromMOST(int)));
//            connect(more, SIGNAL(startMediaMplayer(void)),this,SLOT(doStartMediaMplayer(void)));
        }

        
    }
    else
    {
        if(more!=NULL)
        {
            disconnect(more, SIGNAL(sigQuitFromMore(void)),this,SLOT(doSigQuitFromMore(void)));
            disconnect(more, SIGNAL(ChSwitchFromMOST(int)),this,SLOT(DoChannelSwitchFromMOST(int)));
//            disconnect(more, SIGNAL(startMediaMplayer(void)),this,SLOT(doStartMediaMplayer(void)));

            more_is_showed = false;
            
            if(can_rc4tv!=NULL)
            {
                //wlaczamy sygnaly z most-u do listy TV
                can_rc4tv->disableCANsignals(false);
            }
            more->hide();
        }
    }
}

void play_ctrl::doSigShowMainBar(void)
{
    int cnt_tmp = settings->get_osd_time();
    
    if(cnt_tmp==0) return;

    if(main_bar==NULL)
    {
        main_bar = new tv_bar(this);
        update_bar_info();
        main_bar_timer->setInterval((cnt_tmp*1000));
        main_bar_timer->start();
    }
    else
    {
        main_bar_timer->stop();
        update_bar_info();
        main_bar_timer->setInterval((cnt_tmp*1000));
        main_bar_timer->start();
    }
}

void play_ctrl::doSigHideMainBar(void)
{
    if(main_bar!=NULL)
    {
        main_bar_timer->stop();
        delete main_bar;
        main_bar = NULL;
    }
}

void play_ctrl::doSigShowVolumeWindow(int val)
{
#ifdef VOLUME_CTRL
    if(volume_window==NULL)
    {
        volume_window = new volume_bar(this);
        volume_window->set_volume(val);
        volume_timer->setInterval(3000);
        volume_timer->setSingleShot(true);
        volume_timer->start();
    }
    else
    {
        if(volume_timer->isActive()) volume_timer->stop();

        volume_window->set_volume(val);
        volume_timer->setInterval(3000);
        volume_timer->setSingleShot(true);
        volume_timer->start();
    }
#endif
}
    
void play_ctrl::doSigHideVolumeWindow(void)
{
#ifdef VOLUME_CTRL
    if(volume_window!=NULL)
    {
        if(volume_timer->isActive()) volume_timer->stop();

        delete volume_window;
        volume_window = NULL;
    }
#endif
}

void play_ctrl::UpdateChannelsNumber(bool need_change)
{
    //pobieramy liczbe kanalow przed ponownym zaladowaniem z pliku
    //bedziemy wiedzieli czy jakiś kanal zostal usuniety z listy
    //int total_channles_num_temp = tv_player->DTV_Get_Service_Cnt();
    tv_player->DTV_ReloadChannleList();
    total_channles_num = tv_player->DTV_Get_Service_Cnt();
    
    if(stop_tv_from_most==true) return;//jesli jestesmy w odtwarzaczu to nie odswierzamy listy MOSTu
    
    if(total_channles_num==0)
    {
//      doSigSystemInfo(true,0,"BRAK ZAPISANYCH STACJI");
        AS_Language *language = new AS_Language("#WIN_INFO",6);
        char **Lang = NULL;
        Lang=language->GetContext();
        doSigSystemInfo(true,0,QString::fromUtf8(Lang[1]));
        delete language;
        can_rc4tv->disableCANsignals(false);
        return;
    }
    
    can_rc4tv->sendServiceList();
    
    can_rc4tv->disableCANsignals(false);
    
    if(need_change) StartServiceSelect(0);
};
    
    
    
void play_ctrl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}


char *play_ctrl::getCurrentServiceInfo()
{
    eit_event *current_event = NULL;
    
    dvb_service *s = tv_player->DTV_Index_To_Service(current_ch_index);
    qDebug() << "s" << s;
    if(s!=NULL)
    {
        current_event = tv_player->DTV_Get_Current_Event(s);
        qDebug() << "current event" << current_event;
        if(current_event!=NULL) 
        {
            if(current_event->title!=NULL) return current_event->title;
            else
            return NULL;
        }
        else
        {  
            current_event = tv_player->DTV_Get_Current_Event_From_File(s);
            if(current_event!=NULL)
            {
                if(current_event->title!=NULL) return current_event->title;
                else
                return NULL;
            }
            else
            return NULL;
        }
    }
    
    return NULL;
}

void play_ctrl::doSigQuitFromMore(void)
{
    doEpgFromMOST(false);  
}

void play_ctrl::do_sig_can_rc_code(char rc_code)
{
    if(stop_tv_from_most==true)
            {
//===============================================================================================                
                //jezeli jestesmy w odtwarzaczu mediow i ktos chce wejsc w menu glowne
                if(main_menu_show_hide==false && rc_code==MENU)
                {
                    //printf("WLACZ MENU GLOWNE PODCZAS MEDIA PLAYER...\n");
                    //rozlaczamy sygnal od sterowania pilotem dla klasy mostu
                    disconnect(this,SIGNAL(sigMenuAction(int)),media_player,SLOT(doSigMenuAction(int)));
                    //==========================================================
                    //linie ponizej powoduja wylonienie sie okna menu glownego 
                    //dla ktorego rodzicem jest klasa player_ctrl
                    showNormal();
                    raise();
                    activateWindow();
                    //==========================================================
                    (void)parse_rc_cmd(rc_code);
                    
                    return;
                }
                else
                {

                   if(main_menu_show_hide==true && rc_code==MENU) 
                   {
                       emit sigMenuAction(rc_code);
                       main_menu_show_hide=false;
                       QCANrcTV::menu_from_can = false;
                       can_rc4tv->disableCANsignals(true);
                       //podlaczamy sygnal od sterowania pilotem dla klasy mostu
                       connect(this,SIGNAL(sigMenuAction(int)),media_player,SLOT(doSigMenuAction(int)),Qt::QueuedConnection);
                       media_player->canEnable(true);
                       return;
                   }
                   else
                   {

                       emit sigMenuAction(rc_code);
                       return;
                   }
                }
//===============================================================================================                
                emit sigMenuAction(rc_code);
                return;
            }
            else
            {
                if(main_menu_show_hide==true || more_is_showed==true)
                {
                   if(main_menu_show_hide==true && rc_code==MENU) 
                   {
                       main_menu_show_hide=false;
                       QCANrcTV::menu_from_can = false;
                   }
                   else
                   {
                        if(more_is_showed==true && (rc_code==INFO || rc_code==_EXIT_) )
                        {
                            (void)parse_rc_cmd(rc_code);
                            return;
                        }
                   }
                    
                    emit sigMenuAction(rc_code);
                }
                else
                {
                    //jezeli skanujemy i ktos wybierzy przycisk return to wyjdź ze skanowania
                    if(most_scan==true && rc_code==_EXIT_)
                    {
                        doScanFromMOST(false);
                        return;
                    }
                    else
                    {

                        (void)parse_rc_cmd(rc_code);
                    }
                }
                
            }
}

void play_ctrl::doWinDestroy(QObject *p)
{
    if(tv_list)
    {
        tv_list->hide();
        quick_list_show_flag = false; 
        tv_list->connected = false;
        can_rc4tv->disableCANsignals(false);
    }
}

QByteArray play_ctrl::getDigitsFromTime(int hours, int minutes)
{
    const int DEC = 10;
    const int HEX = 16;
    QByteArray timeRange;
    QString digits;
    digits.setNum((hours / DEC), HEX);
    timeRange.append(digits);
    digits.setNum((hours % DEC), HEX);
    timeRange.append(digits);
    digits.setNum((minutes / DEC), HEX);
    timeRange.append(digits);
    digits.setNum((minutes % DEC), HEX);
    timeRange.append(digits);
    return timeRange;
}

QByteArray play_ctrl::getTimeRangeAndStationTitle()
{
    QByteArray timeRange;
    eit_event *current_event = NULL;
    dvb_service *s = tv_player->DTV_Index_To_Service(current_ch_index);
    if(s!=NULL)
    {
        current_event = tv_player->DTV_Get_Current_Event(s);
        if (current_event!=NULL)
        {
            struct tm start_time;
            struct tm end_time;
            start_time = *localtime(&current_event->start_time);
            end_time = *localtime(&current_event->end_time);
            if (current_event->start_time!=NULL && current_event->end_time!=NULL)
            {
                timeRange.append(getDigitsFromTime(start_time.tm_hour, start_time.tm_min));
                timeRange.append(getDigitsFromTime(end_time.tm_hour, end_time.tm_min));
                timeRange.append(getCurrentServiceInfo());
                return timeRange;
            }
            else
            {
                return NULL;
            }
        }
        else
        {  
            current_event = tv_player->DTV_Get_Current_Event_From_File(s);
            if (current_event!=NULL)
            {
                struct tm start_time;
                struct tm end_time;
                start_time = *localtime(&current_event->start_time);
                end_time = *localtime(&current_event->end_time);

                if (current_event->start_time!=NULL && current_event->end_time!=NULL)
                {
                    timeRange.append(getDigitsFromTime(start_time.tm_hour, start_time.tm_min));
                    timeRange.append(getDigitsFromTime(end_time.tm_hour, end_time.tm_min));
                    timeRange.append(getCurrentServiceInfo());
                    return timeRange;
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
    } 
    return NULL;
}
