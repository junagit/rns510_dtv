/* 
 * File:   main_menu_root.cpp
 * Author: michalm
 * 
 * Created on 7 kwietnia 2014, 08:07
 */
#include <QtGui>
#include "play_ctrl.h"
#include "rc.h"
#include "main_menu.h"

#include "main_menu_root.h"

main_menu_root::main_menu_root(QWidget* parent) : QMainWindow(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);   
    setGeometry(100,96,720,576);
//    setObjectName("audio_win");
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0,0));
    setPalette(pal);

    play_ctrl::menu_root = 1;
    current_item = 0;
    legned = NULL;
    
    for(int i = 0;i<2;i++) 
    {
        mm_items[i] = NULL;
        mm_items[i] = new QLabel(this);
        mm_items[i]->setObjectName("menu_item");
    }

    mm_items[1]->setText("SETUP");
    mm_items[0]->setText("MEIDA PLAYER");

    int y = 20;
    show();
    //żeby zadziałało podświetlenie etykiety w poniższy sposób trzeba najpierw zrobic show() rodzica
    for(int i = 0;i<2;i++) 
    {
        mm_items[i]->setGeometry(20,y,680,(60+30));
        mm_items[i]->setFont(QFont( "Arial", 40, QFont::Normal));
        highlightIdx(i,false);
        y+=(70+30);
    }
    
    highlightIdx(current_item,true);
    
    connect(this, SIGNAL(startMediaMplayer(void)),parent,SLOT(doStartMediaMplayer(void)));
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    
    if(QCANrcTV::menu_from_can==true)
    {
        legned = new QRNS_Buttons(NULL);
        legned->show();
    }
    else
    legned = NULL;
    
}


main_menu_root::~main_menu_root() 
{
    play_ctrl::menu_root = 0;
    
    if(legned)delete legned;
}

void main_menu_root::highlightIdx(int idx,bool f)
{
    QPalette	palette = mm_items[idx]->palette();

    if (f)
    palette.setColor(QPalette::Background,QColor(128,0,0));
    else
    palette.setColor(QPalette::Background,QColor(Qt::black));
    
    mm_items[idx]->setAutoFillBackground(true);
    mm_items[idx]->setPalette(palette);
}

void main_menu_root::doSigMenuAction(int action)
{
                switch(action)
		  {

		  case OK:

                      if(current_item==1)
                      {
                          if(((play_ctrl*)parentWidget())->stop_tv_from_most==false)
                          {
                              play_ctrl::menu_root = 0;
                              main_menu *menu = new main_menu(parentWidget(),0);
                            delete this;
                            break;
                          }
                      }
                      
                      if(current_item==0)
                      {
//                          printf("##### WCHODZE W ZARZADZANIE PROGRAMAMI #####\n");
                         ((play_ctrl*)parentWidget())->main_menu_show_hide = false;
      
                          emit startMediaMplayer();
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
                
                
		default:
		break;
	}
                
  
    
}