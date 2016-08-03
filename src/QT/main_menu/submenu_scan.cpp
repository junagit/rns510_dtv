/** 
 * @file   submenu_scan.cpp
 * @author michalm
 *
 * @date   3 wrzesień 2012, 13:51
 */
#include "main_menu.h"
#include "rc.h"
#include "submenu_scan.h"
#include "manual_scan.h"
#include "QAutoScanWindow.h"
#include "app_settings.h"
#include "AS_language.h"
#include "QMostTvList.h"
submenu_scan::submenu_scan(QWidget* parent,int to_select) : QMainWindow(parent) 
{
    AS_Language *language = new AS_Language("#SUBMENU_SCAN",2);
    char **Lang = NULL;
    Lang=language->GetContext();
    
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(100,96,720,576);
//    setObjectName("audio_win_grey");
    
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0,0));
    setPalette(pal);
    
    
    setFocusPolicy(Qt::NoFocus);
    
    block_once = true;
    current_item = 0;
    
    for(int i = 0;i<2;i++) 
    {
        mm_items[i] = NULL;
        mm_items[i] = new QLabel(this);
        mm_items[i]->setObjectName("menu_item");
    }
    
    mm_items[0]->setText(QString::fromUtf8(Lang[0]));
    mm_items[1]->setText(QString::fromUtf8(Lang[1]));
    delete language;
    
    show();
    int y = 20;
    for(int i = 0;i<2;i++) 
    {
        mm_items[i]->setGeometry(20,y,680,60+30);
        mm_items[i]->setFont(QFont( "Arial", 30, QFont::Normal));
        highlightIdx(i,false);
        y+=70+30;
    }
    
   
    if(to_select>=0 && to_select<2) current_item = to_select;
    
    highlightIdx(current_item,true);
    
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    if(QCANrcTV::menu_from_can==true)
    {
        legned = new QRNS_Buttons(NULL);
        legned->show();
    }
    else
    {
        legned = NULL;
    }
}



submenu_scan::~submenu_scan() 
{
    
    for(int i = 0;i<2;i++) 
    {
        delete mm_items[i];
        mm_items[i] = NULL;
    }
 
    if(legned)delete legned;

}


void submenu_scan::doSigMenuAction(int action)
{
                switch(action)
		  {

		  case OK:

                      if(current_item==0)
                      {
                          manual_scan *ms = new manual_scan(parentWidget());
                          delete this;
                      }
                      
                      if(current_item==1)
                      {
                          QAutoScanWindow *as = new QAutoScanWindow(parentWidget());
                          delete this;
                      }
                      
		    break;

                 case 0x55:    
		 case DOWN:
                     
                     if(current_item<1)
                     {
                         highlightIdx(current_item,false);
                         current_item++;
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
                                          
		   break;
                   
                   
		  case _EXIT_:
                  {
                    main_menu *m = new main_menu(parentWidget(),0);   
                    delete this;
                  }
		  break;
                  
                  
                    
                case MENU:
                    delete this;
                break;
                
		default:
		break;
	}
    

    
}


//void submenu_scan::highlightIdx(int idx,bool f)
//{
//    parentWidget()->setUpdatesEnabled(false);
//    if(f)mm_items[idx]->setStyleSheet("background-color: rgb( 100,0,0 )");
//    else
//        mm_items[idx]->setStyleSheet("background-color: black");
//    parentWidget()->setUpdatesEnabled(true);
//}

void submenu_scan::highlightIdx(int idx,bool f)
{
QPalette	palette = mm_items[idx]->palette();
    
palette.setColor(QPalette::Foreground,QColor(Qt::white));//kolor czcionki

if (f)
palette.setColor(QPalette::Background,QColor(128,0,0));
else
palette.setColor(QPalette::Background,QColor(Qt::black));
    

mm_items[idx]->setAutoFillBackground(true);
mm_items[idx]->setPalette(palette);
mm_items[idx]->update();
}