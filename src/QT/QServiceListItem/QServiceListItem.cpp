/** 
 * @file   QServiceListItem.cpp
 * @author michalm
 *
 * @date   28 wrzesień 2012, 12:09
 */

#include "QServiceListItem.h"

QServiceListItem::QServiceListItem(QWidget *parent,QString txt,bool with_checkbox) : QLabel(parent)
{
    QFont f;
    
    setText(txt);
    setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    setFont(QFont( "Arial", 20, QFont::Normal));
    
   
    //setWindowFlags(Qt::FramelessWindowHint); 
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("border: none;");
    setMinimumWidth(300);
    setMinimumHeight(40);
    
    isCheckable = with_checkbox;
    check = NULL;
    checked = false;
    //checkbox 30x30
    if(with_checkbox==true)
    {
      check = new QLabel(this);
      check->setGeometry(this->width()-(DEFAULT_CHECK_W+DEFAULT_MARGIN),DEFAULT_MARGIN,DEFAULT_CHECK_W,DEFAULT_CHECK_H);
     // check->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: black;");
    }
    
    
    show();

    selectItem(false);
    
}



QServiceListItem::~QServiceListItem() 
{
    if(check!=NULL) delete check;
}

void QServiceListItem::selectItem(bool f)
{
//    if(f==true)
//    {
//        setStyleSheet("border: none; background-color: rgb( 100,0,0 )");
//    }
//    else
//    {
//        //setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: black");
//        setStyleSheet("border: none; background-color: black");
//    }
 //   parentWidget()->update();
    
    QPalette	p = this->palette();
    p.setColor(QPalette::Foreground,QColor(Qt::white));
    
if (f)
p.setColor(QPalette::Background,QColor(100,0,0));
else
p.setColor(QPalette::Background,QColor(Qt::black));
   
setAutoFillBackground(true);
setPalette(p);
update();
}