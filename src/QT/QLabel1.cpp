/* 
 * File:   QLabel1.cpp
 * Author: motyl
 * 
 * Created on 2 marzec 2014, 11:05
 */
#include <QPaintEvent>
#include <QPainter>
#include "QLabel1.h"
#include <stdio.h>

/**
 *      KOMPONENT QLabel1
 * 
 *      -wyswietlanie tekstu
 *      -wyswietlanie tekstu z grafiką w tle
 *      -wyswietlanie tekstu z tłem pod tekstem i ikoną z lewej strony tekstu
 *      -zmiana tła w zalezności od stanu 
 *      - dodać pozycję rysowania ikony(z lewej lub prawej strony)
 */


QLabel1::QLabel1(QWidget *parent) : QWidget(parent) 
{
    PixmapNormalState = NULL;
    PixmapSelectedState = NULL;
    icon = NULL;
    state = false;
    align = Qt::AlignVCenter;
    font_color = Qt::white;
    font_color_selected = Qt::white;
    selectedBgColor = Qt::red;
    setAlignment(align);
    do_alpha = true;
    highlightMode = false;
    selectMode = true;
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    
}

QLabel1::~QLabel1() {
}

void QLabel1::paintEvent(QPaintEvent *e)
{
  Q_UNUSED(e);
  
  QPainter qpainter (this);

  qpainter.setRenderHint(QPainter::NonCosmeticDefaultPen,true);

  if(state)
  {
      if(state==1)
      {
            //rysujemy podświetlenie elementu
            if(PixmapHighLightState)
            {
                qpainter.drawPixmap(rect(),*PixmapHighLightState);
            }
            else
            {
                if(!highlightMode)
                {
                    qpainter.fillRect(rect(),Qt::red);
                }
                else
                {
                   qpainter.setPen(QPen(QColor(0xff,0x66,0x00), 4));
                   qpainter.drawLine(10,4,this->width(),4);
                   qpainter.drawLine(10,height()-4,this->width(),height()-4);
                }
            }
      }
      else
      {
          if(state==2)
          {
              //rysujemy podświetlenie elementu
            if(PixmapSelectedState)qpainter.drawPixmap(rect(),*PixmapSelectedState);
            else
            {
                if(!selectMode)qpainter.fillRect(rect(),Qt::red);
                else
                {
                   qpainter.setPen(QPen(Qt::white, 4));
                   qpainter.drawLine(10,0,this->width(),0);
                   qpainter.drawLine(10,height()-4,this->width(),height()-4);
                }
            }
          }
      }
  }
  else
  {
      //rysujemy nie podswietlony element
      if(PixmapNormalState)qpainter.drawPixmap(0,0,*PixmapNormalState);
      else
      {
          if(do_alpha==false)qpainter.fillRect(rect(),normalBgColor);
      }
  }
  
  
  if(txt.isEmpty()==false)
  {//rysujemy tekst
      if(icon)
      {//jesli mamy ikone to musimy zacząć rysować tekst o konca bitmapy
          //jesli ikona miesci sie w komponencie to rysujemy ikonę
          // i tekst - w przeciwnym przypadku rysujemy tylko tekst
          if(width()>=icon->width())
          {
                qpainter.drawPixmap(4,(height()-icon->height())/2,*icon);
                qpainter.setPen(font_color);
                qpainter.drawText((icon->width()+10),text_y_pos,txt);
          }
          else
          {
               qpainter.drawText(rect(),align,txt);
          }
      }
      else
      {
        //rysujemy tylko tekst  
        if(state) qpainter.setPen(font_color_selected);
        else
        qpainter.setPen(font_color);
        
        qpainter.drawText(rect(),align,txt);
      }
  }

}


void QLabel1::setPixmapNormalState(QPixmap *pixmap)
{
    PixmapNormalState = pixmap;
}
    
void QLabel1::setPixmapSelectedState(QPixmap *pixmap)
{
    PixmapSelectedState = pixmap;
}

void QLabel1::setPixmapHighlightState(QPixmap *pixmap)
{
    PixmapHighLightState = pixmap;
}
   
void QLabel1::setIcon(QPixmap *ico)
{
    icon = ico;
    
}

void QLabel1::setText(const QString &t)
{
    QString temp = t;
    if(icon)
    {
        txt=fontMetrics().elidedText(temp, Qt::ElideRight,width()-(icon->width()+10));
    }
    else
    {
        txt = txt=fontMetrics().elidedText(temp, Qt::ElideRight,width());
    }
//  printf("txt:%s\n",txt.toAscii().data());

  text_y_pos = (height()-fontMetrics().height());
//  printf("text_y_pos:%d %d\n",text_y_pos,fontMetrics().height());
  text_y_pos+=(fontMetrics().height()>>1);
//  printf("text_y_pos:%d %d\n",text_y_pos,fontMetrics().height());
};