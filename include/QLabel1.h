/* 
 * File:   QLabel1.h
 * Author: motyl
 *
 * Created on 2 marzec 2014, 11:05
 */
#include <QWidget>
#ifndef QLABEL1_H
#define	QLABEL1_H

class QLabel1  : public QWidget{
    Q_OBJECT
public:
    QLabel1(QWidget *parent = 0);
    virtual ~QLabel1();
    bool do_alpha;
    void setPixmapNormalState(QPixmap *pixmap);
    void setPixmapSelectedState(QPixmap *pixmap);
    void setPixmapHighlightState(QPixmap *pixmap);
    void setHighlightMode(bool mode){
      highlightMode = mode;  
    };
    void setIcon(QPixmap *icon);
    
    void setAlignment(Qt::AlignmentFlag f)
    {
        align = f;
    };
    
    void setState(const int &f)
    {
        state = f;
        update();
    };
    int getState()
    {
        return state;
    }
    void setText(const QString &t);
//    QString text()
//    {
//      return txt;  
//    };
    
    void clear()
    {
      state = 0;
      txt.clear();
      PixmapNormalState = NULL;
      PixmapSelectedState = NULL;
      PixmapHighLightState = NULL;
      icon = NULL;
//      repaint();
    };
    
    void setBackgroundColor(const QColor &c)
    {
        normalBgColor = c;
    };
private:
    int         state;//normal,highlight/selected
    QPixmap     *PixmapNormalState;
    QPixmap     *PixmapSelectedState;
    QPixmap     *PixmapHighLightState;
    QPixmap     *icon;
    QString     txt;
    Qt::AlignmentFlag align;
    QColor      selectedBgColor;
    QColor      normalBgColor;
    QColor      font_color;
    QColor      font_color_selected;
    int text_x_pos,text_y_pos;
    bool        selectMode;
    bool        highlightMode;

protected:
    void paintEvent(QPaintEvent *event);
//    virtual void enterEvent(QEvent*)
//    {
//        state = true;
//        repaint();
//    }
//    
//
//    virtual void leaveEvent(QEvent*)
//    {
//        state = false;
//        repaint();
//    }


    
};

#endif	/* QLABEL1_H */

