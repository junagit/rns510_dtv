/** 
 * @file   tv_main_menu.cpp
 * @author michalm
 *
 * @date   30 sierpień 2012, 10:28
 */
#include "rc.h"
#include "tv_main_menu.h"

tv_main_menu::tv_main_menu(QWidget* parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(50,50,640,480);
    setObjectName("main_menu_window");
    current_menu_window = NULL;
    win_alpha_value = 0.7;
    
    win_alpha = new QGraphicsOpacityEffect(NULL);
    win_alpha->setOpacity(win_alpha_value);
    setGraphicsEffect(win_alpha);
    
    //root = new menu_root_win(this);
   // current_menu_window = root;
}



tv_main_menu::~tv_main_menu() 
{
    
    
}

void tv_main_menu::show_main_menu()
{
    
    
}

void tv_main_menu::rc_controll(char code)
{
    if(current_menu_window!=NULL) current_menu_window->rc_controll(code);
              
}
