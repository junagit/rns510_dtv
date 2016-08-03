/* 
 * File:   NewList.h
 * Author: michalm
 *
 * Created on 19 luty 2014, 11:58
 */

#ifndef NEWLIST_H
#define	NEWLIST_H

#include <QtGui>

class list_item{
public:
    QString name;
    int    type;
};

class NewList : public QWidget{
    Q_OBJECT
public:
    NewList(QWidget* parent = 0);
    virtual ~NewList();
    void addItem(const QString &item_name,int type);
    void highlightItem(int idx);
    void highlightNext(void);
    void highlightPrevious(void);
    void showList();
    void clearList(void);
    int  getCurrentItemIdx(void);
    int getListSize(void)
    {
      return list.size();  
    };
private:
    int current_item;
    int current_item2;
    int max_itemx;
    QList<list_item*> list;
    QLabel  *items[5];
    bool    list_showed;
    void keyPressEvent(QKeyEvent *event);
    void highlightIdx(int idx,bool f);

    
private slots:
    void onTimer(void);
};

#endif	/* NEWLIST_H */

