/* 
 * File:   QMenuInfoWindow.cpp
 * Author: michalm
 * 
 * Created on 22 styczeń 2014, 07:09
 */
#include "rc.h"
#include "QMenuInfoWindow.h"
#include "main_menu.h"
#include "version.h"
#include "QOtherOptWindow.h"
const unsigned char completeVersion[] =
{
   // 'V',
   // VERSION_MAJOR_INIT,
   // '.',
    VERSION_MINOR_INIT,
    ' ',
    BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
    '-',
    BUILD_MONTH_CH0, BUILD_MONTH_CH1,
    '-',
    BUILD_DAY_CH0, BUILD_DAY_CH1,
    ' ',
//    BUILD_HOUR_CH0, BUILD_HOUR_CH1,
//    ':',
//    BUILD_MIN_CH0, BUILD_MIN_CH1,
//    ':',
//    BUILD_SEC_CH0, BUILD_SEC_CH1,
    '\0'
};

QMenuInfoWindow::QMenuInfoWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(152,96,720,576);
    setObjectName("audio_win");  
    char buf[255];
    QString text;
    
    text.append("DEVICE:JMC 03-v2.1 \n");

    text.append("TV2.");
    
    QString temp = (const char*)completeVersion;
    text.append(temp);
    text.append("\n");
    text.append("INTERFACE VERSION: TV2.01");
    
    QFile qss2("/usr/version_info.txt");
    qss2.open(QFile::ReadOnly);
    text.append(qss2.readAll());
    qss2.close();
    
    info.setParent(this);
    info.setGeometry(2,2,718,574);
    info.setObjectName("simple_label");
    info.setAlignment(Qt::AlignCenter);
    info.setText(text);
    info.setFont(QFont( "Arial", 30, QFont::Normal));
    
    show();
    info.setFont(QFont( "Arial", 30, QFont::Normal));
    info.show();
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
}



QMenuInfoWindow::~QMenuInfoWindow() 
{
    
}

void QMenuInfoWindow::doSigMenuAction(int action)
{
      switch(action)
	{
              case _EXIT_:
              { 
                //main_menu *m = new main_menu(parentWidget(),4);   
                  QOtherOptWindow *o_opt = new QOtherOptWindow(parentWidget(),3);
                 
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
