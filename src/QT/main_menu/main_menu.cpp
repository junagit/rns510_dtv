/** 
 * @file   main_menu.cpp
 * @author michalm
 *
 * @date   3 wrzesień 2012, 11:30
 */
#include <QtGui>
#include "rc.h"
#include "main_menu.h"
#include "submenu_scan.h"
#include "QTvServiceManager.h"
#include "QOtherOptWindow.h"
#include "QDateTimeWindow.h"
#include "app_settings.h"
#include "QUpdateWin.h"
#include "QScreenScallingWin.h"
#include "AS_language.h"
#include "play_ctrl.h"
#include "QMenuInfoWindow.h"
#include "QRNS_Buttons.h"
#include "main_menu_root.h"
/**
 * 
 * @param parent
 * @param to_select
 */
main_menu::main_menu(QWidget* parent,int to_select) : QMainWindow(parent) 
{
    AS_Language *language = new AS_Language("#MAIN_MENU",5);
    char **Lang = NULL;
    Lang=language->GetContext();
    
    setWindowFlags(Qt::FramelessWindowHint);   
    //setGeometry(152,96,720,576);
    setGeometry(100,96,720,576);
//    setObjectName("audio_win_grey");
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0,0));
    setPalette(pal);

    current_item = 0;
   
    for(int i = 0;i<2;i++) 
    {
        mm_items[i] = NULL;
        mm_items[i] = new QLabel(this);
        mm_items[i]->setObjectName("menu_item");
    }

    mm_items[0]->setText(QString::fromUtf8(Lang[0]));
//    mm_items[1]->setText(QString::fromUtf8(Lang[1]));
    mm_items[1]->setText(QString::fromUtf8(Lang[2]));

    
    int y = 20;
    show();
    //żeby zadziałało podświetlenie etykiety w poniższy sposób trzeba najpierw zrobic show() rodzica
    for(int i = 0;i<2;i++) 
    {
        mm_items[i]->setGeometry(20,y,680,60+30);
        mm_items[i]->setFont(QFont( "Arial", 30, QFont::Normal));
        highlightIdx(i,false);
        y+=70+30;
    }


    if(to_select>=0) current_item = to_select;
   
    highlightIdx(current_item,true);

    delete language;

    if(QCANrcTV::menu_from_can==true)
    {
        legned = new QRNS_Buttons(NULL);
        legned->show();
    }
    else
    {
        legned = NULL;
    }
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
}



main_menu::~main_menu() 
{

//    play_ctrl::menu_root = 0;
    
    for(int i = 0;i<2;i++) 
    {
        delete mm_items[i];
        mm_items[i] = NULL;
    }

    if(legned)delete legned;

}

void main_menu::doSigMenuAction(int action)
{
                switch(action)
		  {

		  case OK:

                      if(current_item==0)
                      {
                          if(((play_ctrl*)parentWidget())->stop_tv_from_most==false)
                          {
//                            printf("##### WCHODZE W SKANOWANIE #####\n");
   
                            submenu_scan *scan = new submenu_scan(parentWidget());
  
                            delete this;
                          }
                      }
                      
//                      if(current_item==1)
//                      {
////                          printf("##### WCHODZE W ZARZADZANIE PROGRAMAMI #####\n");
//                          
//                          QTvServiceManager *sm = new QTvServiceManager(parentWidget());
//
//                         
//                          delete this;
//                      }
            
//                      if(current_item==2)
//                      {
////                          printf("##### WCHODZE W SKALOWANIE EKRANU #####\n");
//                          play_ctrl::menu_root =false;
//                          QScreenScallingWin *scalling = new QScreenScallingWin(parentWidget());
//                          disconnect(this,SLOT(doSigMenuAction(int)));
//                          delete this;
//                      }
                      
                      if(current_item==1)
                      {
//                          printf("##### WCHODZE W INNE #####\n");

                          QOtherOptWindow *o_opt = new QOtherOptWindow(parentWidget(),-1);
 
                          delete this;
                      }
                      
//                      if(current_item==4)
//                      {
//                          ((play_ctrl*)parentWidget())->main_menu_show_hide = false;
//      
//                          emit startMediaMplayer();
//                      }
                      
		    break;

                 case 0x55:       
		 case DOWN:
                     
                     if(current_item<1)
                     {
                         highlightIdx(current_item,false);
                         current_item++;
                         highlightIdx(current_item,true);
                     }
                     else
                     {
                         highlightIdx(current_item,false);
                         current_item = 0;
                         highlightIdx(current_item,true);
                     }
                     break;
                     
                 case 0x54:    
                 case UP:
                     
                     if(current_item>0)
                     {
                         highlightIdx(current_item,false);
                         current_item--;
                         highlightIdx(current_item,true);
                     }
                     else
                     {
                         highlightIdx(current_item,false);
                         current_item = 1;
                         highlightIdx(current_item,true);
                     }
                                          
		   break;
                                      

                case MENU:
                {
                    delete this;
                }
                break;
                
                case _EXIT_:
                {
                    main_menu_root *m = new main_menu_root(parentWidget());   
//                ((play_ctrl*)parentWidget())->main_menu_show_hide = false;
//                ((play_ctrl*)parentWidget())->can_rc4tv->disableCANsignals(false);
//                if(((play_ctrl*)parentWidget())->media_player) ((play_ctrl*)parentWidget())->media_player->canEnable(true);
                    delete this;
                }
                break;
                
		default:
		break;
	}
                
  
    
}

/**
 * @brief Podswietlenie elementu. Uwaga!korzystanie ze stylesheeet'ow dynamicznie powoduje wolniejsze dzialanie podsiwetlanie elemnetu!! Lepiej jest używać Qpalette do tego celu
 * @param idx
 * @param f
 */
void main_menu::highlightIdx(int idx,bool f)
{
    QPalette	palette = mm_items[idx]->palette();

    if (f)
    palette.setColor(QPalette::Background,QColor(128,0,0));
    else
    palette.setColor(QPalette::Background,QColor(Qt::black));
    
    mm_items[idx]->setAutoFillBackground(true);
    mm_items[idx]->setPalette(palette);
}