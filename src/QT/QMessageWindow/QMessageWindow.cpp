/** 
 * @file   QMessageWindow.cpp
 * @author michalm
 *
 * @date   7 wrzesień 2012, 07:43
 */

#include "QMessageWindow.h"

/**
 * @brief Okno inforamcyjne z ikonami
 * @param parent
 * @param type = 0 ikona typu info,type=1 ikona typu warning
 */
QMessageWindow::QMessageWindow(QWidget* parent,int type,const QString &msg) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);    
    setWindowFlags(Qt::Widget);       
    setGeometry(262,324,500,120);
    setObjectName("audio_win");
    
//    img = new QLabel(this);
//    img->setGeometry(5,5,110,110);
//    img->setObjectName("simple_label");
//    
//    if(type==0) img->setPixmap(QPixmap(QString::fromUtf8(":/img/Info-icon.png")));
//    if(type==1) img->setPixmap(QPixmap(QString::fromUtf8(":/img/warning72x72.png")));

    info = new QLabel(this);
    info->setObjectName("simple_label");
    info->setText(msg);
    info->setGeometry(5,5,495,115);
    info->setAlignment(Qt::AlignCenter);
 
    show();
    info->setFont(QFont( "Arial", 40, QFont::Normal));
}


QMessageWindow::~QMessageWindow() 
{
   // delete img;
    delete info;
}