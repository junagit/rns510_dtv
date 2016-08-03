/* 
 * File:   QMostMediaPlayer.cpp
 * Author: michalm
 * 
 * Created on 26 wrzesień 2013, 09:23
 */

#include "QMostMediaPlayer.h"
#include "app_settings.h"
#include "rc.h"
#include "play_ctrl.h"

extern void via_set_scalling(bool flag);
bool start_tv_flag  = true;
volatile int action_cnt = 0;
QMediaPlayerMain::QMediaPlayerMain(QObject* parent) : QObject(parent) 
{
    app_settings settings;
    wait4constructor = true;
    bool found_last_file = false;
    main_file_browser = NULL;
    current_browser_item = 0;
    media_player = NULL;
    seekToLastPosition = false;
    selecting = false;
    block_src_event = false;
    timeToQuit = false;
    init_flag = true;
    block_stop_resume = true;
    selected_browser_item = -1;///zainicjalizowac
    connected = true;
    legned = NULL;
    
    
    depth_title[0] = "/";
    depth_title[1] = depth_title[0] + "../";
    depth_title[2] = depth_title[1] + "../";
    depth_title[3] = depth_title[2] + "../";
    depth_title[4] = depth_title[3] + "../";
    depth_title[5] = depth_title[4] + "../";
    depth_title[6] = depth_title[5] + "../";
    depth_title[7] = depth_title[6] + "../";
    depth_title[8] = depth_title[7] + "../";
    depth_title[9] = depth_title[8] + "../";
    
    
    selected_file_dir_depth = current_dir_depth = settings.getDirDepth();
//    printf("current_dir_depth:%d\n",current_dir_depth);
    
    setObjectName("QMediaPlayerMain");

    media_player = new QMediaPlayer(this);
    
    connect(media_player,SIGNAL(sigEOS(void)),this,SLOT(onEOS(void)),Qt::QueuedConnection);
    connect(media_player,SIGNAL(sigMediaPrepared(void)),this,SLOT(onMediaPrepared(void)),Qt::QueuedConnection);
           
    found_last_file = initFileBrowserFromSavedData(settings.getLastMediaSource());
    
    if(found_last_file==true)
    {
        //trezba odtwarzać
        block_src_event = true;
        seekToLastPosition = true;
    }
    
    
    main_file_browser = new QFileBrowser(NULL,"/media/usb1/");
    main_file_browser->enableSourceNotify(false);

    connect(this->parent(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    connect(main_file_browser,SIGNAL(file_selected(const QString &,const QString &)),this,SLOT(on_file_selected(QString,QString)));
    connect(main_file_browser,SIGNAL(dir_selected(const QString &)),this,SLOT(on_dir_selected(QString)));
    connect(main_file_browser,SIGNAL(sigMediaInsert(uint8_t )),this,SLOT(onMediaInsert(uint8_t)));
    connect(main_file_browser,SIGNAL(sigMediaRemove(uint8_t )),this,SLOT(onMediaRemove(uint8_t)));

    //jezeli nie znalezlismy pliku to podlaczamy ten sygnal zeby mozna bylo chodzic po zrodłach
    if(found_last_file==false)main_file_browser->enableSourceNotify(true);
    
    pause_timer = new QTimer(this);
    pause_timer->setInterval(500);
    pause_timer->setSingleShot(true);
    pause_timer->stop();
    connect(pause_timer, SIGNAL(timeout()),this,SLOT(doPause(void)));

    delay_timer = new QTimer(this);
    delay_timer->setInterval(10000);
    delay_timer->setSingleShot(true);
    delay_timer->stop();
    connect(delay_timer, SIGNAL(timeout()),this,SLOT(doDelay(void)));

    if(found_last_file==true)    
    {
        //jesli znaleziono ostatni plik to pokaż okno audio z listą plików
        //i skocz do okreslonego katalogu
        block_src_event = true;
        main_file_browser->setSource(settings.getLastMediaSource());
        main_file_browser->jumpToDir(settings.getLastDir());

        updateFileList();

        int res = main_file_browser->checkFile(settings.getLastFile());

        if(res>=0)
        {
            current_browser_item = selected_browser_item = res + main_file_browser->getDirCnt() ;
            media_player->highlightCurrentItem(current_browser_item,true);  
            media_player->setSelectedItem(current_browser_item);
        }
        //odswież 
        current_dir_depth = selected_file_dir_depth;
        current_source_id = settings.getLastMediaSource();
//                printf("current_dir_depth:%d\n",current_dir_depth);
//                printf("settings.getLastMediaSource():%d\n",settings.getLastMediaSource());
        if(current_dir_depth>0)
        {
            QString current_dir;
            QString temp = settings.getLastDir();
            temp.resize(temp.size()-1);
            current_dir.append(strrchr(temp.toAscii().data(),'/'));
            current_dir.remove("/");

            media_player->setTitle(depth_title[current_dir_depth]+current_dir,settings.getLastMediaSource());
        //    printf("current_dir_depth:%d  %s\n",current_dir_depth,tmp.toAscii().data());
        }
        else
        media_player->setTitle("/",settings.getLastMediaSource());

                
        on_file_selected(settings.getLastFile(),settings.getLastDir());
    }
    else
    {
        updateFileList();
    }
    
    wait4constructor = false;
    delay_timer->start();

    can_interface = new QCanInterface(this);
    can_interface->initCAN();
    legned = new QRNS_Buttons(NULL,1);
    legned->show();
    
    action_cnt = 0;
}



QMediaPlayerMain::~QMediaPlayerMain() 
{
    app_settings settings;
   
    delete legned;
    //przed wyjsciem do tv zapisz pozycje utworu
    if(media_player!=NULL)
    {
        if(media_player->isPlaying()) 
        {     
            settings.saveLastMediaTime(media_player->getCurrentTime());
            media_player->stop();
        }
        
        media_player->hide_media_window(true);
        delete media_player;
    }
    
    delete main_file_browser;
}


/**
 * 
 * @param action
 */
void QMediaPlayerMain::doSigMenuAction(int action)
{
      delay_timer->stop();
      delay_timer->start();
      
//      printf("QMediaPlayerMain::doSigMenuAction action:%d\n",action);
      
        switch(action)
		  {
		  case OK:
//                      //PAUZA
//                      //dodany timer do obslugi pauzy
//                      //pauza jest wykonywana tylko gdy przycisk jest puszczony
//                      //gdy nie bylo tego mechanizmu czesto przychodzaca komenda z pilota powpdowala zawieszenie gstreamera

                      if(media_player)
                      {
                          if(media_player->isPlaying())
                          {
                              QString current_place = media_player->getCurrentFileDir();
                              
                     //        printf("current_place:%s  %d %d\n",current_place.toAscii().data(),current_browser_item,selected_browser_item);
                              
                              if(current_browser_item==selected_browser_item && main_file_browser->getCurrentPath()==current_place)
                              {
                                pause_timer->stop();
                                pause_timer->start();
                              }
                              else
                              {
                                  browser_item *tmp = main_file_browser->get_item(current_browser_item);
                                  
                                  if(tmp)
                                  {//ograniczenie chodzenia po katalogach. Max. zagłebienie w katalogi = 10
                                   //   printf("current_dir_depth:%d\n",current_dir_depth);
                                   //   printf("tmp:%s  type:%d\n",tmp->item_name.toAscii().data(),tmp->item_type);
                                      if(tmp->item_type==0)
                                      {
                                          if(tmp->item_name=="..")
                                          {
                                              if(current_dir_depth>0)
                                              {
                                                  current_dir_depth--;
                                                  main_file_browser->selectItem(current_browser_item);
                                              }
                                          }
                                          else
                                          {
                                            if(current_dir_depth<9) 
                                            {
                                                current_dir_depth++;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                          }
                                      }
                                      else
                                      {
                                        if(tmp->item_name=="..")
                                        {
                                            if(current_dir_depth>0)
                                            {
                                                current_dir_depth--;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                        }
                                        else
                                        main_file_browser->selectItem(current_browser_item);
                                      }
                                  }
                              }
                          }
                          else
                          {
                              if(media_player->isPaused()==false)
                              {
//                                main_file_browser->selectItem(current_browser_item);
//                                selected_browser_item = current_browser_item;
                                  browser_item *tmp = main_file_browser->get_item(current_browser_item);
                                  
                                  if(tmp)
                                  {//ograniczenie chodzenia po katalogach. Max. zagłebienie w katalogi = 10
                                   //   printf("current_dir_depth:%d\n",current_dir_depth);
                                     // printf("tmp:%s  type:%d\n",tmp->item_name.toAscii().data(),tmp->item_type);
                                      if(tmp->item_type==0)
                                      {
                                          if(tmp->item_name=="..")
                                          {
                                              if(current_dir_depth>0)
                                              {
                                                  current_dir_depth--;
                                                  main_file_browser->selectItem(current_browser_item);
                                              }
                                          }
                                          else
                                          {
                                            if(current_dir_depth<9) 
                                            {
                                                current_dir_depth++;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                          }
                                      }
                                      else
                                      {
                                        if(tmp->item_name=="..")
                                        {
                                            if(current_dir_depth>0)
                                            {
                                                current_dir_depth--;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                        }
                                        else
                                        main_file_browser->selectItem(current_browser_item);
                                      }
                                  }
                              }
                              else
                              {
                                  pause_timer->stop();
                                  pause_timer->start();
                              }
                          }
                      }
                      else
                      {
                                 browser_item *tmp = main_file_browser->get_item(current_browser_item);
                                  
                                  if(tmp)
                                  {//ograniczenie chodzenia po katalogach. Max. zagłebienie w katalogi = 10
                                //      printf("current_dir_depth:%d\n",current_dir_depth);
                               //       printf("tmp:%s  type:%d\n",tmp->item_name.toAscii().data(),tmp->item_type);
                                      if(tmp->item_type==0)
                                      {
                                          if(tmp->item_name=="..")
                                          {
                                              if(current_dir_depth>0)
                                              {
                                                  current_dir_depth--;
                                                  main_file_browser->selectItem(current_browser_item);
                                                //  printf("current_dir_depth:%d\n",current_dir_depth);
                                              }
                                          }
                                          else
                                          {
                                            if(current_dir_depth<9) 
                                            {
                                                current_dir_depth++;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                          }
                                      }
                                      else
                                      {
                                        if(tmp->item_name=="..")
                                        {
                                            if(current_dir_depth>0)
                                            {
                                                current_dir_depth--;
                                                main_file_browser->selectItem(current_browser_item);
                                            }
                                        }
                                        else
                                        main_file_browser->selectItem(current_browser_item);
                                        
                                      }
                                  }
                      }
                      
		    break;


		 case DOWN:

                     if(media_player->isVideo()==true)
                     {
                         legned->show();
                         media_player->show_media_window();
                     }
                                                
                     if(current_browser_item<main_file_browser->getAllItemsCnt()-1)
                     {
                        current_browser_item++;
                        media_player->highlightCurrentItem(current_browser_item,false);

                     }
                     break;
                     
                 case UP:
                     //UTWÓR O JEDEN DO TYLU jesli jest wyslana do most'u lista kontrolujaca odtwarzacz
                     
                     if(media_player->isVideo()==true)
                     {
                         media_player->show_media_window();
                         legned->show();
                     }
                     
                     if(current_browser_item>0)
                     {
                        current_browser_item--;
                        media_player->highlightCurrentItem(current_browser_item,false);    
                     }
		   break;
                   
                case RIGHT:
                    //PRZEWIJANIR DO TYLU
                    if(media_player) media_player->ff();
                    break;
                        
                case LEFT:
                    //PRZEWIJANIE DO PRZODU
                    if(media_player) media_player->fb();
                    break;
                        
                                      

                case _TV_R:
                {
                    //wyjscie do tv
                    quitToTv();
                }
                break;
                    
                    case INFO:
                        //pokaż okno video z paskiem postępu
                        if(media_player->isVideo()==true)media_player->show_video_window();       

                        break;
		default:
		break;
	}
}

void QMediaPlayerMain::quitToTv(void)
{
    legned->hide();
    
    if(media_player!=NULL) media_player->hide_media_window(true);
    
    app_settings settings;
            
    while(selecting==true)
    {
        usleep(10000);
        QCoreApplication::processEvents ();
    }
    
    connected = false;
    selecting = false;
    
    if(media_player!=NULL) 
    {          
        doDelay();//przed wyjscie do tv zawsze wracamy do katalogu z którego odtważamy plik
        
        if(media_player->isPlaying()) settings.saveLastMediaTime(media_player->getCurrentTime());

        //ustaw flage ze bedziemy wychodzic i tymsamym zablokuj wejscie do funkcji onMediaPrepared
        //timeToQuit = true;
        media_player->stop();
//        media_player->hide_media_window(true);
        
        if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
        {
            printf("@@@@@@@ SAVE LAST DATA @@@@@@@\n");
            settings.saveLastMediaSource(current_source_id);
            settings.saveDirDepth(current_dir_depth);

            selected_file_dir_depth = current_dir_depth;
            selected_browser_item = current_browser_item;
        }
    }

    disconnect(parent(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    settings.save_tv_state(1);
    
    emit sigTvOnOff(start_tv_flag);
}


bool QMediaPlayerMain::initFileBrowserFromSavedData(const uint8_t &from_src_id)
{
    app_settings settings;
    QString last_file = settings.getLastFile();
    QString last_dir = settings.getLastDir();
    QString temp;
    
    int last_media_source = settings.getLastMediaSource();

    if(last_file.isEmpty()==true || last_dir.isEmpty()==true) return false;
    
    if(from_src_id==SOURCE_SD1) temp = "/media/sd1/";
    
    if(from_src_id==SOURCE_USB1) temp = "/media/usb1/";
    
    if(last_dir.contains(temp,Qt::CaseSensitive)==true)
    {
        if(main_file_browser==NULL) main_file_browser = new QFileBrowser(NULL,temp);
         else
             main_file_browser->setBasePath(temp);
             
        if(main_file_browser->jumpToDir(last_dir)==true)
        {
            //znaleźliśmy ostatni katalog - czy istnieje ostatni plik?
            int res = main_file_browser->checkFile(last_file);
            if(res>=0)
            {
                //zaczynamy odtaważać;
                
                current_source_id = last_media_source;
                printf("##### ZNALAZLEM OSTATNI PLIK. START TO PLAY...current_source_id:%d\n",current_source_id );
                selected_browser_item = res;
                return true;
            }
        }
    }
    else
    return false;
    
//    if(main_file_browser==NULL) 
//    {
//        main_file_browser = new QFileBrowser(NULL,"/media/usb1/");
//        current_source_id = SOURCE_USB1;
//    }
    
    return false;
}

void QMediaPlayerMain::on_file_selected(const QString &file,const QString &dir)
{
    blockSignals(true);
    
    selecting = true;
    
    if(media_player==NULL) 
    {
        media_player = new QMediaPlayer(this);
        connect(media_player,SIGNAL(sigEOS(void)),this,SLOT(onEOS(void)),Qt::QueuedConnection);
        connect(media_player,SIGNAL(sigMediaPrepared(void)),this,SLOT(onMediaPrepared(void)),Qt::QueuedConnection);
    }
    
    if(media_player->isPlaying()==true)media_player->stop();
    
    media_player->setFile(dir,file);
    media_player->mute(true);
    
    //jezeli bedziemy odtwarzac wideo to howamy okno dla mp3
    if(media_player->isVideo()==true)
    {
        media_player->hide_media_window(false);
        if(legned!=NULL)legned->hide();
    }
    else
    {
        if(legned!=NULL)legned->show();
    }


    media_player->play();
   
    media_player->setSelectedItem(current_browser_item);

    
    
}

void QMediaPlayerMain::on_dir_selected(const QString &dir)
{      
    int max_files = main_file_browser->getAllItemsCnt();

    media_player->clearFileList();

    for(int i = 0;i<max_files;i++)
    {
        media_player->addItemToFileList(main_file_browser->get_item(i),false);
    }

    current_browser_item = 0;
 //=============================================================================
    //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
    {
//        printf("selected_browser_item:%d\n",selected_browser_item);
        media_player->setSelectedItem(selected_browser_item);
    }
    else
    {//jezeli nie to ustaw indeks na -1;
        //wtedy nie zostanie zaznaczony żaden element
        media_player->setSelectedItem(-1);
    }
 //=============================================================================   
    media_player->addItemToFileList(NULL,true);//zaktualizuj listę na ekranie   
    media_player->highlightCurrentItem(current_browser_item,false);//podświetl element
    
//    printf("current_dir_depth:%d %s\n",current_dir_depth,main_file_browser->getCurrentDir().toAscii().data());

    if(current_dir_depth>0)
    {
        QString temp = main_file_browser->getCurrentDir();
        media_player->setTitle(depth_title[current_dir_depth]+temp,main_file_browser->getCurrentSource());
    }
    else
    media_player->setTitle("/",main_file_browser->getCurrentSource());
    
}

void  QMediaPlayerMain::MediaPlayerStop(void)
{
    timeToQuit = true;
    connected = false;
    handleMediaStop();
    
    if(main_file_browser)main_file_browser->enableSourceNotify(false);
    //gdy wychodzimy do tv wylaczamy sygnaly od mostu tymczasowo
//    if(main_file_browser)main_file_browser->disableCANsignals(false);
}


void  QMediaPlayerMain::MediaPlayerResume(void)
{
    blockSignals(false);
    
    timeToQuit = false;
    handleMediaResume();
    connected = true;

    if(main_file_browser)main_file_browser->enableSourceNotify(true);
    
    legned->show();
}

/**
 * 
 */
void QMediaPlayerMain::onMediaPrepared(void)
{
    int last_time_pos;
    app_settings settings;
    
//    //jezeli weszlismy do pogramu i musimy zaczac odtwarzanie utworu od pewnego momoentu
//    //jesli podczas wybierania utworu chcemy wyjść do TV

//    printf("####### QMostMediaPlayer::onMediaPrepared start...\n");
    selecting = false;
    blockSignals(false);
    
    if(timeToQuit==true) 
    {   
        timeToQuit = false;
        return;
    }

    if(media_player) media_player->hide_media_window(false);

    //jesli musimy skoczyc do okreslonego czasu w utworze to skocz
    if(seekToLastPosition) 
    {
        last_time_pos = settings.getLastMediaTime();
        
        seekToLastPosition = false;    
        current_dir_depth = settings.getDirDepth();
        
        if(last_time_pos>0) media_player->seekTo(last_time_pos);
        
        media_player->mute(false);
        
        int res = main_file_browser->checkFile(media_player->getCurrentFileName());

        if(res>=0)
        {
            current_browser_item = selected_browser_item = res + main_file_browser->getDirCnt() ;
            media_player->highlightCurrentItem(current_browser_item,true);  
            media_player->setSelectedItem(current_browser_item);
        }
        
        if(current_dir_depth!=0)
        {
            QString current_dir;
            QString temp = media_player->getCurrentFileDir();
            temp.resize(temp.size()-1);
            current_dir.append(strrchr(temp.toAscii().data(),'/'));
            current_dir.remove("/");
            media_player->setTitle(depth_title[current_dir_depth]+current_dir,main_file_browser->getCurrentSource()); 
        }
        else
        media_player->setTitle("/",main_file_browser->getCurrentSource());
        
//        if(settings.getLastMediaSource()==SOURCE_USB1)media_player->update_src_txt(SOURCE_SD1);
//        else
//        media_player->update_src_txt(SOURCE_USB1);
        
//        printf("current_browser_item:%d selected_browser_item:%d\n",current_browser_item,selected_browser_item);
        if(media_player->isVideo()==true)
        {
            if(legned)legned->hide();
        }
    }
    else
    {   //jesli wlaczamy urzadzenie i zaczynamy odtwazrzac ostatni plik to nie potrzeba zapisywać go jescze raz
        //zapis wybranego pliku
        QString last_file_tmp = media_player->getCurrentFileName();

        if(last_file_tmp.isEmpty()==false) settings.saveLastFile(last_file_tmp.toUtf8().data());

        //zapis ostatniego katalogu. PObieramy to klasy QMediaPlayer
        if(media_player->getCurrentFileDir().isEmpty()==false)settings.saveLastDir(media_player->getCurrentFileDir().toAscii().data());
        
        media_player->mute(false);
        
//        printf("##### %s %d\n",__FUNCTION__,__LINE__);
    }

    
    if(media_player->getCurrentFileDir().contains("/media/sd1/",Qt::CaseSensitive)==true)
    {
        current_source_id = SOURCE_SD1;
    }
    else
    {
        if(media_player->getCurrentFileDir().contains("/media/usb1/",Qt::CaseSensitive)==true)current_source_id = SOURCE_USB1;
    }
    
    
    //zapisz te dane jezeli jestesmy w tym samy katalogu
    
    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
    {
//        printf("##### %s %d\n",__FUNCTION__,__LINE__);
        settings.saveLastMediaSource(current_source_id);
        settings.saveDirDepth(current_dir_depth);

        selected_file_dir_depth = current_dir_depth;
        //selected_browser_item = current_browser_item;
        int res = main_file_browser->checkFile(media_player->getCurrentFileName());

        if(res>=0)
        {
            current_browser_item = selected_browser_item = res + main_file_browser->getDirCnt() ;
//            printf("##### %s %d %d \n",__FUNCTION__,__LINE__,current_browser_item);
            media_player->highlightCurrentItem(current_browser_item,true);  
            media_player->setSelectedItem(current_browser_item);
        }
    }
    
    block_src_event = false;
    main_file_browser->enableSourceNotify(true);
}

void QMediaPlayerMain::onEOS(void)
{
    app_settings settings;

    if(media_player)
        {
            if(media_player->isError())
            {
                //jesli dostalismy blad od gstreamer'a podczas przygotowywania do odtwaraania
                //wracamy do katalogu głównego
                //i wysyłamy listę z plikami
                //ostatni utwor byl z bledem to usun ostatnio odtwarzany
                //żeby nie wchodził w kółko ten sam 
                settings.saveLastMediaTime(0);
                settings.saveLastFile("");
                settings.saveLastDir("");
                
                //chowamy okno odtwarzacza
                media_player->hide_media_window(true);
                main_file_browser->setSource(current_source_id);
                return;
            }
        }

    //jezeli utwor sie zakonczyl normalnie to kasujemy pozycje utworu 
    settings.saveLastMediaTime(0);
    sync();

    //odtwarzamy nastepny utwor z katalogu
    if(media_player) 
    {
        if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
        {
            //przed automatyczną zmianą utworu spradz czy jestesmy w tym samym katalogu
            //jezeli jestesmy to można podswietlic następny plik
            //to jest zabezpieczenie gdy chodzimy po katalogach
            media_player->next(true, true);
            
            //pobierz obecny indeks z listy plików
            //funkcja next wybiera automatycznie nastepny utwór z listy
            //i podświetla go automatycznie na liście!!!
            current_browser_item = media_player->getCurrentIdx();
            media_player->setSelectedItem(current_browser_item);
        }
        else
        media_player->next(true, false);
    }

}



void QMediaPlayerMain::onMediaInsert(uint8_t src_id)
{
    app_settings settings;
//    printf("###### %s %d\n",__FUNCTION__,__LINE__);
    //jezeli jestesmy w TV to nie reagujemy na eventy od włozenia nosnika
    if(/*settings.get_tv_state()==1 ||*/ wait4constructor==true || src_id>3) return;

    //zablokuj nosnik gdy mamy wejśc w ostatni odtważany plik
    if(seekToLastPosition==true) return;
    
    printf("@@@ %s %d src_id:%d main_file_browser->getCurrentSource():%d block_src_event:%d current_source_id:%d@@@\n",__FUNCTION__,__LINE__,src_id,main_file_browser->getCurrentSource(),block_src_event,current_source_id);

    //blokujemy na poczatku
    if(block_src_event==true)
    {
        block_src_event = false;
        return;
    }
    
    if(media_player!=NULL)
    {
        if(media_player->isPlaying() /*&& settings.getLastMediaSource()==src_id*/)
        {//jezeli odtważamy plik i jestesmy np. na usb które nie jest włożone to trzeba odświeżyc liste 
            
            if(current_source_id==src_id)
            {
                current_source_id = src_id;
                main_file_browser->setSource(src_id);
                current_browser_item = 0;
                current_dir_depth = 0;

                updateFileList();
                
                media_player->setTitle("/",current_source_id);
            }

        }
        else
        {
            bool res = initFileBrowserFromSavedData(src_id);
            //jesli nie znalezlismy ostatniego katalogu na nosniku to skocz do katalogu glownego wlozonego 
            if(res==false) 
            {
                setChangeSrc(src_id);
            }
            else
            {
                seekToLastPosition = true;
                //skocz do katalogu
                main_file_browser->setSource(settings.getLastMediaSource());
                main_file_browser->jumpToDir(media_player->getCurrentFileDir());
                
                updateFileList();
            
                int res = main_file_browser->checkFile(media_player->getCurrentFileName());

                if(res>=0)
                {
                    current_browser_item = selected_browser_item = res + main_file_browser->getDirCnt() ;
                    media_player->highlightCurrentItem(current_browser_item,true);  
                    media_player->setSelectedItem(current_browser_item);
                }
                //odswież 
                current_dir_depth = selected_file_dir_depth;
                current_source_id = settings.getLastMediaSource();

//                printf("current_dir_depth:%d\n",current_dir_depth);
//                printf("settings.getLastMediaSource():%d\n",settings.getLastMediaSource());
                if(current_dir_depth>0)
                {
                    QString current_dir;
                    QString temp = media_player->getCurrentFileDir();
                    temp.resize(temp.size()-1);
                    current_dir.append(strrchr(temp.toAscii().data(),'/'));
                    current_dir.remove("/");
                   
                    media_player->setTitle(depth_title[current_dir_depth]+current_dir,settings.getLastMediaSource());
                //    printf("current_dir_depth:%d  %s\n",current_dir_depth,tmp.toAscii().data());
                }
                else
                {
                    media_player->setTitle("/",settings.getLastMediaSource());
                }
                
            //    printf("settings.getLastMediaSource():%d\n",settings.getLastMediaSource());
//                if(settings.getLastMediaSource()==SOURCE_USB1)
//                {
//                    media_player->update_src_txt(SOURCE_SD1);
//                }
//                else
//                {
//                    media_player->update_src_txt(SOURCE_USB1);
//                }
                
                on_file_selected(settings.getLastFile(),settings.getLastDir());
            }
        }
    }
    else
    {
        bool res = initFileBrowserFromSavedData(src_id);
        //jesli nie znalezlismy ostatniego katalogu na nosniku to skocz do katalogu glownego wlozonego 
        if(res==false) 
        {
            setChangeSrc(src_id);
        }
        else
        {
            if(media_player==NULL)
            {
                media_player = new QMediaPlayer(this);
                connect(media_player,SIGNAL(sigEOS(void)),this,SLOT(onEOS(void)),Qt::QueuedConnection);
                connect(media_player,SIGNAL(sigMediaPrepared(void)),this,SLOT(onMediaPrepared(void)),Qt::QueuedConnection);
            }
            
            seekToLastPosition = true;
            on_file_selected(settings.getLastFile(),settings.getLastDir());
        }
      }
   }

/**
 * @brief Funkcja wywolywana przy wyjeciu nosnika USB
 * @param src_id - id nośnika
 */
void QMediaPlayerMain::onMediaRemove(uint8_t src_id)
{
    app_settings settings;
    
    //jezeli jestesmy w TV to nie reagujemy na eventy od włozenia nosnika
//    if(settings.get_tv_state()==1) return;
    
    printf("@@@ %s %d src_id:%d main_file_browser->getCurrentSource():%d @@@\n",__FUNCTION__,__LINE__,src_id,main_file_browser->getCurrentSource());
    
    block_src_event = false;
    
    if(media_player)
    {
        if(media_player->isPlaying()==true)
        {
            if(settings.getLastMediaSource()==src_id)
            {
                printf("#### STOP PLAY.... ####\n");
                settings.saveLastMediaTime(media_player->getCurrentTime());
                media_player->stop();//zatrzymaj odtwarzanie
                media_player->setTitle(" ",src_id);
                
                if(src_id==SOURCE_USB1)system("umount -f /media/usb1");
                else
                {
                    if(src_id==SOURCE_SD1)system("umount -f /media/sd1");
                }

                main_file_browser->setSource(src_id);
                updateFileList();
                
                return;
            }
            else
            {       
                if(current_source_id==src_id)
                {
                    current_source_id = src_id;
                    main_file_browser->setSource(src_id);
                    current_browser_item = 0;
                    current_dir_depth = 0;

                    updateFileList();

                    media_player->setTitle("/",current_source_id);
                }
                else
                {

                }
                return;
            }
        }
        else
        {
            if(current_source_id==src_id)
            {
                current_source_id = src_id;
                main_file_browser->setSource(src_id);
                current_browser_item = 0;
                current_dir_depth = 0;

                updateFileList();

                media_player->setTitle("/",current_source_id);
            }
            else
            {

            }
            return;
        }
    }
    else
    {
        if(current_source_id==src_id)
        {
            current_source_id = src_id;
            main_file_browser->setSource(src_id);
            current_browser_item = 0;
            current_dir_depth = 0;

            updateFileList();

            media_player->setTitle("/",current_source_id);
        }
        else
        {
            
        }
        return;
    }
      
    return;
}

void QMediaPlayerMain::doPause(void)
{
    pause_timer->stop();
    
    if(media_player) 
    {
      if(media_player->isPaused())
      {
          media_player->play();
      }
      else
      media_player->pause();
      
  }
}
void QMediaPlayerMain::doDelay(void)
{
   block_stop_resume = false;
   app_settings settings;
   if(media_player)
    {
        if(media_player->isPlaying())
        {
//            printf("@@@@@@@ %s %s\n",main_file_browser->getCurrentPath().toAscii().data(),media_player->getCurrentFileDir().toAscii().data());
            if(main_file_browser->getCurrentPath()!=media_player->getCurrentFileDir())
            {
                main_file_browser->setSource(settings.getLastMediaSource());
                main_file_browser->jumpToDir(media_player->getCurrentFileDir());
                
                updateFileList();
            
                int res = main_file_browser->checkFile(media_player->getCurrentFileName());

                if(res>=0)
                {
                    current_browser_item = selected_browser_item = res + main_file_browser->getDirCnt() ;
                    media_player->highlightCurrentItem(current_browser_item,true);  
                    media_player->setSelectedItem(current_browser_item);
                }
                //odswież 
                current_dir_depth = selected_file_dir_depth;
                current_source_id = settings.getLastMediaSource();
//                printf("current_dir_depth:%d\n",current_dir_depth);
//                printf("settings.getLastMediaSource():%d\n",settings.getLastMediaSource());
                if(current_dir_depth>0)
                {
                    QString current_dir;
                    QString temp = media_player->getCurrentFileDir();
                    temp.resize(temp.size()-1);
                    current_dir.append(strrchr(temp.toAscii().data(),'/'));
                    current_dir.remove("/");
                    
                    media_player->setTitle(depth_title[current_dir_depth]+current_dir,settings.getLastMediaSource());
                //    printf("current_dir_depth:%d  %s\n",current_dir_depth,tmp.toAscii().data());
                }
                else
                media_player->setTitle("/",settings.getLastMediaSource());
                
//                if(settings.getLastMediaSource()==SOURCE_USB1)media_player->update_src_txt(SOURCE_SD1);
//                else
//                media_player->update_src_txt(SOURCE_USB1);
                
            }
            else
            {
                //jezeli jestesmy w tym samym katalogu to tylko podśietl ten plik
                int res = main_file_browser->checkFile(media_player->getCurrentFileName());

                if(res>=0)
                {
                    current_browser_item = selected_browser_item;
//                    printf("##### %s %d selected_browser_item %d\n",__FUNCTION__,__LINE__,selected_browser_item);
                    media_player->highlightCurrentItem(current_browser_item,true);  
                    media_player->setSelectedItem(current_browser_item);
                }
            }
            
            if(media_player->isVideo()==true)
            {
                media_player->hide_media_window(false);
                legned->hide();
            }
        }
    }
}

void QMediaPlayerMain::handleMediaStop(void)
{
    disconnect(parent(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    if(media_player)
    {
        while(media_player->selecting()==true) usleep(500000);
        app_settings settings;
        
        if(media_player->isPlaying()==true)
        {
            settings.saveLastMediaTime(media_player->getCurrentTime());

            media_player->hide_media_window(true);//schowaj okno
            media_player->stop();//zatrzymaj odtwarzanie
        }
    }
}

void QMediaPlayerMain::handleMediaResume(void)
{
//    printf("#### EVT_MEDIA_RESUME ####\n");
    
    bool found_last_file = initFileBrowserFromSavedData(current_source_id);
     
    app_settings settings;
    
    if(found_last_file==true)
    {
        updateFileList();
        on_file_selected(settings.getLastFile(),settings.getLastDir());
        seekToLastPosition = true;
        current_dir_depth = settings.getDirDepth();
    }
    else
    {
        //gdy po wznowieniu odtwarzania nie znajdziemy pliku to wysylamy liste z plikami
        //i przelaczamy sie na nosnik z którego ostatnio bylo odtwarzane
        main_file_browser->setSource(current_source_id);
        updateFileList();
    }
    
    connect(parent(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
   
}


//void QMediaPlayerMain::onCanRecieve(QByteArray *data)
//{
//    if(data==NULL) return;
//    
//    if(data->size()==0) return;
//    
//    play_ctrl *p = ((play_ctrl*)parent());
//    
//    if(p->getMainMenuFlag()==true) return;
//    
//    if(connected==false) return;
//    
//    delay_timer->stop();
//    delay_timer->start();
//
//    switch(data->at(0))
//    {
//       case 0x85:
//             //powrot do aktualnego katalogu z którego jest odtważany utwor
//            doBackToCurrentTrack();
//            break;
//            
//        case 0x86:
//        {
//            //NAST UTWOR
//            //NASTEPNY UTWOR Z LISTY jesli jest wyslana do most'u lista kontrolujaca odtwarzacz
//            bool  ff_flag = false;
//            
//            if(data->at(1)>0) ff_flag = true;
//            
//
//            if(media_player)
//            {
//                if(ff_flag==false)
//                {
//                    if(media_player->isVideo()==true)media_player->show_media_window();
//
//                    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
//                    {//podśietl elemennt na liście jeżeli jesteśmy w tym samym katalogu
//
//    //                    printf("current_browser_item:%d selected_browser_item:%d\n",current_browser_item,selected_browser_item);
//                        media_player->next(false,true);
//                        current_browser_item = media_player->getCurrentIdx();
//                    }
//                    else
//                    {
//                        media_player->next(false,false); 
//                    }
//                }
//                else
//                {
//                    //przewijamy
//                    doSigMenuAction(RIGHT);
//                }
//            }
//        }
//           break;
//           
//        case 0x87:
//        {
//            bool ff_flag = false;
//            
//            if(data->at(1)>0) ff_flag = true;
//            
//            //POPRZEDNI UTWOR
//            if(media_player)
//            {
//                if(ff_flag==false)
//                {
//                    if(media_player->isVideo()==true)media_player->show_media_window();
//
//                    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
//                    {//podśietl elemennt na liście jeżeli jesteśmy w tym samym katalogu
//
//                        media_player->prev(false,true);
//
//                        current_browser_item = media_player->getCurrentIdx();
////                        if(current_browser_item>0)current_browser_item--;
//                    }
//                    else
//                    {
//                        media_player->prev(false,false);
//                    }
//                }
//                else
//                {
//                    doSigMenuAction(LEFT);
//                }
//            }
//        }
//           break;
//           
//            
//       case 0x83:
//            quitToTv();
//            break;
//            
//        case 0x84:
//       //zmiana nosnika
////            printf("zmiana nosnika!!!!!!\n");
//            if(current_source_id==SOURCE_SD1)
//            {
//                current_source_id = SOURCE_USB1;
//                main_file_browser->setSource(SOURCE_USB1);
//                current_browser_item = 0;
//                current_dir_depth = 0;
////=============================================================================
//                //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
//                if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
//                {
//                    media_player->setSelectedItem(selected_browser_item);
//                }
//                else
//                {//jezeli nie to ustaw indeks na -1;
//                    //wtedy nie zostanie zaznaczony żaden element
//                    media_player->setSelectedItem(-1);
//                }
// //=============================================================================  
//                updateFileList();
//                
//                if(media_player)
//                {//zmienieamy nazwe przycisku
//                    media_player->update_src_txt(SOURCE_SD1);
//                }
//                
//                if(main_file_browser->getSourceStatus(SOURCE_USB1)==true)media_player->highlightCurrentItem(current_browser_item,false);
//                else
//                media_player->highlightCurrentItem(-1,false);
//                
////                media_player->highlightCurrentItem(current_browser_item,false);
//                media_player->setTitle("/",current_source_id);
//            }
//            else
//            {
//                current_source_id = SOURCE_SD1;
//                main_file_browser->setSource(SOURCE_SD1);
//                current_browser_item = 0;
//                current_dir_depth = 0;
////=============================================================================
//                //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
//                if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
//                {
////                    printf("selected_browser_item:%d\n",selected_browser_item);
//                    media_player->setSelectedItem(selected_browser_item);
//                }
//                else
//                {//jezeli nie to ustaw indeks na -1;
//                    //wtedy nie zostanie zaznaczony żaden element
//                    media_player->setSelectedItem(-1);
//                }
// //=============================================================================  
//                updateFileList();
//                
//                if(media_player)
//                {
//                    media_player->update_src_txt(SOURCE_USB1);
//                }
//                
//                if(main_file_browser->getSourceStatus(SOURCE_SD1)==true)media_player->highlightCurrentItem(current_browser_item,false);
//                else
//                media_player->highlightCurrentItem(-1,false);
//                
//                media_player->setTitle("/",current_source_id);
//            }
//        break;
//
//        case 0x82:
//            doSigMenuAction(OK);
//        break;
////###################################################################
//        case 0x81://galka w lewo
//            doSigMenuAction(UP);
//        break;
//
//        case 0x80:
//            doSigMenuAction(DOWN);
//        break;
//        
//        case 0x88:
//            //menu
//            canEnable(false);
//            emit menuEnable(true);
//            break;
////###################################################################            
//        default:
//
//            break;
//    }
//    
//
//}

void QMediaPlayerMain::onCanRecieve(QByteArray *data)
{
    if(data==NULL) return;
    
    if(data->size()==0) return;
    
    play_ctrl *p = ((play_ctrl*)parent());
    
    if(p->getMainMenuFlag()==true) return;

    if(connected==false) return;

    delay_timer->stop();
    delay_timer->start();
    

    switch(data->at(0))
    {
            
        case 0x82:
            via_set_scalling(false);
            start_tv_flag = false;
            quitToTv();
            break;
            
       case 0x89:
           
              if(data->at(1)==0x03) 
              {
                  doSigMenuAction(UP);
                  
                  return;
              }
                
                if(data->at(1)==0x04) 
                {
                    doSigMenuAction(DOWN);
                    
                    return;
                }
                
              //OK - przycisk zoom
                if(data->at(1)==0x06) 
                {
                    if(media_player->isVideo()==true)legned->show();
                    
                    doSigMenuAction(OK);
                    return;
                }
              
               //zmiana nośnika
                if(data->at(1)==0x05) 
                {
                    if(media_player->isVideo()==true)legned->show();
                    
                    
                        if(current_source_id==SOURCE_SD1)
                        {
                            current_source_id = SOURCE_USB1;
                            main_file_browser->setSource(SOURCE_USB1);
                            current_browser_item = 0;
                            current_dir_depth = 0;
            //=============================================================================
                            //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
                            if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
                            {
                                media_player->setSelectedItem(selected_browser_item);
                            }
                            else
                            {//jezeli nie to ustaw indeks na -1;
                                //wtedy nie zostanie zaznaczony żaden element
                                media_player->setSelectedItem(-1);
                            }
             //=============================================================================  
                            updateFileList();

                            if(main_file_browser->getSourceStatus(SOURCE_USB1)==true)media_player->highlightCurrentItem(current_browser_item,false);
                            else
                            media_player->highlightCurrentItem(-1,false);

            //                media_player->highlightCurrentItem(current_browser_item,false);
                            media_player->setTitle("/",current_source_id);
                        }
                        else
                        {
                            current_source_id = SOURCE_SD1;
                            main_file_browser->setSource(SOURCE_SD1);
                            current_browser_item = 0;
                            current_dir_depth = 0;
            //=============================================================================
                            //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
                            if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
                            {
            //                    printf("selected_browser_item:%d\n",selected_browser_item);
                                media_player->setSelectedItem(selected_browser_item);
                            }
                            else
                            {//jezeli nie to ustaw indeks na -1;
                                //wtedy nie zostanie zaznaczony żaden element
                                media_player->setSelectedItem(-1);
                            }
             //=============================================================================  
                            updateFileList();

                            if(main_file_browser->getSourceStatus(SOURCE_SD1)==true)media_player->highlightCurrentItem(current_browser_item,false);
                            else
                            media_player->highlightCurrentItem(-1,false);

                            media_player->setTitle("/",current_source_id);
                        }
                    } 
              
              //wychodzimy z media player'a
              if(data->at(1)==0x00) 
              {
                  start_tv_flag = true;
                  via_set_scalling(false);
                  quitToTv();
              }
            break;
                   
        default:

            break;
    }
    

}

void QMediaPlayerMain::updateFileList(void)
{
    if(media_player==NULL)
    {
        media_player = new QMediaPlayer(this);
        connect(media_player,SIGNAL(sigEOS(void)),this,SLOT(onEOS(void)),Qt::QueuedConnection);
        connect(media_player,SIGNAL(sigMediaPrepared(void)),this,SLOT(onMediaPrepared(void)),Qt::QueuedConnection);
    }

    media_player->show_media_window();
    
    int max_files = main_file_browser->getAllItemsCnt();
  
    media_player->clearFileList();
    
//    printf("max_files:%d\n",max_files);
    
    if(max_files)
    {
        for(int i = 0;i<max_files;i++)
        {
    //      printf("i:%d %s\n",i,file_br->get_item(i)->item_name.toAscii().data());
            media_player->addItemToFileList(main_file_browser->get_item(i),false);
        }
    }

    media_player->addItemToFileList(NULL,true);
}


void QMediaPlayerMain::doBackToCurrentTrack(void)
{
//    if(media_player)
//    {
//        if(media_player->isPlaying())
//        {
            //cofamy się do katalogu głównego o 1
            if(current_dir_depth>0)
            {
                current_dir_depth--;
                main_file_browser->selectItem(0);
            }    
//        }
//    }
}


void QMediaPlayerMain::setChangeSrc(int src_id)
{
                current_source_id = src_id;

                if(current_source_id==SOURCE_USB1)
                {
                    current_source_id = SOURCE_USB1;
                    main_file_browser->setSource(SOURCE_USB1);
                    current_browser_item = 0;
                    current_dir_depth = 0;
    //=============================================================================
                    //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
                    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
                    {
                        media_player->setSelectedItem(selected_browser_item);
                    }
                    else
                    {
                        //jezeli nie to ustaw indeks na -1;
                        //wtedy nie zostanie zaznaczony żaden element
                        media_player->setSelectedItem(-1);
                    }
     //=============================================================================  
                    updateFileList();

                    if(main_file_browser->getSourceStatus(SOURCE_USB1)==true)media_player->highlightCurrentItem(current_browser_item,false);
                    else
                    media_player->highlightCurrentItem(-1,false);

    //                media_player->highlightCurrentItem(current_browser_item,false);
                    media_player->setTitle("/",current_source_id);
                }
                
                if(current_source_id==SOURCE_SD1)
                {
                    current_source_id = SOURCE_SD1;
                    main_file_browser->setSource(SOURCE_SD1);
                    current_browser_item = 0;
                    current_dir_depth = 0;
    //=============================================================================
                    //jezeli wejdziemy do katalogu z któ¶ego odtwarzamy utwór to zaznacz go
                    if(main_file_browser->getCurrentPath()==media_player->getCurrentFileDir())
                    {
    //                    printf("selected_browser_item:%d\n",selected_browser_item);
                        media_player->setSelectedItem(selected_browser_item);
                    }
                    else
                    {//jezeli nie to ustaw indeks na -1;
                        //wtedy nie zostanie zaznaczony żaden element
                        media_player->setSelectedItem(-1);
                    }
     //=============================================================================  
                    updateFileList();

//                    if(media_player)
//                    {
//                        media_player->update_src_txt(SOURCE_USB1);
//                    }

                    if(main_file_browser->getSourceStatus(SOURCE_SD1)==true)media_player->highlightCurrentItem(current_browser_item,false);
                    else
                    media_player->highlightCurrentItem(-1,false);

                    media_player->setTitle("/",current_source_id);
                }
}