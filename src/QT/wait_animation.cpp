/** 
 * @file   wait_animation.cpp
 * @author michalm
 *
 * @date   5 wrzesień 2012, 12:50
 */
#include <QThread>
#include "wait_animation.h"
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

wait_animation::wait_animation(QWidget* parent) : QMainWindow(parent) 
 {
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(((1024/2)-50),((732/2)-50),100,100);
    setStyleSheet("border: none; color:white; background: none");
    setFocusPolicy(Qt::NoFocus);
    
    pi = new QProgressIndicator(this);
    pi->setAnimationDelay(100);
    pi->setGeometry(0,0,100,100);
    pi->setColor(Qt::white);
    pi->stopAnimation();
    
    hide();
    
    connect(this,SIGNAL(sigShowAnimation(bool)),this,SLOT(doShowAnimation(bool)),Qt::QueuedConnection);
 }




wait_animation::~wait_animation() 
{
    delete pi;
}


void wait_animation::doShowAnimation(bool f)
{
    if(f==true)
    {
        show();
        pi->startAnimation();
    }
    else
    {
        pi->stopAnimation();
        hide();
    }
}

void wait_animation::ShowAnim(bool f)
{
    emit sigShowAnimation(f);
}