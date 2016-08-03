/** 
 * @file   newClass.cpp
 * @author michalm
 * @brief  Okno podgłaszania dla TV
 * @date   10 sierpień 2012, 07:10
 */
#include <QtGui/QApplication>
#include <QProgressBar>
#include "volume_bar.h"
#include "overlay.h"
#include "app_settings.h"
#include "misc_func.h"

volume_bar::volume_bar(QWidget* parent) : QMainWindow(parent)
{

    setWindowFlags(Qt::FramelessWindowHint);
//    setWindowFlags(Qt::Widget);    
    setGeometry(0,10,720,70);
    setFocusPolicy(Qt::NoFocus);
    setObjectName("volume");

    b = new QProgressBar(this);
    b->setGeometry(78,20,(720-90),30);
    b->setObjectName("volume_progress");
    b->setValue(0);
    b->setMaximum(10);
    b->setMinimum(0);
    b->setTextVisible(false);
    
    val_txt = new QLabel("");
    val_txt->setParent(this);
    val_txt->setGeometry(8,20,60,30);
    val_txt->setObjectName("volume_txt");
    
    show();
    
    val_txt->setFont(QFont( "Arial", 25, QFont::Normal));
    val_txt->setAlignment(Qt::AlignCenter);
}



volume_bar::~volume_bar() 
{
    delete b;
    delete val_txt;
}





//void volume_bar::set_win_alpha(qreal v)
//{
//    win_alpha_value = v;
//   // win_alpha->setOpacity(v);
//}
