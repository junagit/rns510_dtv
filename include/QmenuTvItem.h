/** 
 * @file   QmenuTvItem.h
 * @author michalm
 *
 * @date   10 wrzesień 2012, 10:47
 */
#include <QtGui>

#ifndef QMENUTVITEM_H
#define	QMENUTVITEM_H

typedef void (*valChangeCb) (void*);

class QmenuTvItem : public QLabel
{
    Q_OBJECT
public:
    QmenuTvItem(QWidget *parent = 0,QString txt = "");
    ~QmenuTvItem();
    void selectItem(bool f, bool f2);
    bool isSelected(){ return selected;};
    
    void setPosition(int x,int y);
    
    void addItem(QString txt);
    
    int currentIdx();
    
    void switchItem(int item_idx);
    
    void switchToNextItem(void);
    
    void switchToPrevItem(void);
    
    void                *user_data;
    valChangeCb         cb;
    
    void  setUserCB(valChangeCb f,void *data);
    
private:
    QLabel              *label;
    QLabel              *arrow_left;
    QLabel              *arrow_right;
    
    bool                selected;
    QList<QString>      OptList;
    int                 current_idx;
};

#endif	/* QMENUTVITEM_H */

