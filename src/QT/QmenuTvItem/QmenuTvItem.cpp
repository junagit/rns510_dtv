/** 
 * @file   QmenuTvItem.cpp
 * @author michalm
 *
 * @date   10 wrzesień 2012, 10:47
 */
#include <QtGui>
#include "QmenuTvItem.h"

QmenuTvItem::QmenuTvItem(QWidget *parent,QString txt) : QLabel(parent)
{
    selected = false;
    current_idx = -1;
    OptList.clear();
    cb = NULL;//callback przy zmianie wartosci jesli uuzytkownik poda funkcję
    user_data = NULL;
    
    setText(txt);
    setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    setFont(QFont( "Arial", 30, QFont::Normal));
    //setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: black");
    setObjectName("menu_item");
    setMinimumWidth(680);
    setMinimumHeight(60+30);
            
    label = new QLabel(this);
    label->setObjectName("simple_label");
    label->setAlignment(Qt::AlignCenter);
    
    //label->setStyleSheet("border: none; color:white; background: none");
    label->setFixedSize(160,50);
    label->setGeometry((this->width()-label->width()),20,label->width(),label->height());
   
    label->show();
    setFont(QFont( "Arial", 30, QFont::Normal));
    label->setFont(QFont( "Arial", 30, QFont::Normal));
}



QmenuTvItem::~QmenuTvItem() 
{
    OptList.clear();

    delete label;
}


void QmenuTvItem::selectItem(bool f, bool f2)
{
    QPalette palette = this->palette();
    
    if(f==true)
    {
        selected = true;
        if(f2)palette.setColor(QPalette::Background,QColor(Qt::gray));
        else
        palette.setColor(QPalette::Background,QColor(128,0,0));
        //setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: rgb( 100,0,0 )");
    }
    else
    {
        palette.setColor(QPalette::Background,QColor(Qt::black));
        //setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: black");
        selected = false;
    }
    
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}


void QmenuTvItem::setPosition(int x,int y)
{
    setGeometry(x,y,width(),height());
};

void QmenuTvItem::addItem(QString txt)
{
    OptList.append(txt);

    if(OptList.count()==1)
    {
        label->setText(OptList.at(0));
        current_idx = 0;
    }

}

int QmenuTvItem::currentIdx()
{
        return current_idx;  
};

void QmenuTvItem::switchItem(int item_idx)
{

    if(OptList.count()>0 && item_idx>=0)
    {
        label->setText(OptList.at(item_idx));
        current_idx = item_idx;
        if(cb!=NULL) cb(user_data);
    }
};


void QmenuTvItem::switchToNextItem(void)
{
    if(selected==false) return;

    if(OptList.count()==0) return;

    if(current_idx<OptList.count()-1)
    {
        current_idx++;
        label->setText(OptList.at(current_idx));
    }
    
    if(cb!=NULL) cb(user_data);

};

void QmenuTvItem::switchToPrevItem(void)
{
    if(selected==false) return;

    if(OptList.count()==0) return;

    if(current_idx>0)
    {
        current_idx--;
        label->setText(OptList.at(current_idx));
        
    }
    
    if(cb!=NULL) cb(user_data);
    
};


void  QmenuTvItem::setUserCB(valChangeCb f,void *data)
{
    cb = f;
    user_data = data;
    
}