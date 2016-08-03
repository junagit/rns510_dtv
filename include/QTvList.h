/* 
 * File:   QTvList.h
 * Author: michalm
 *
 * Created on 4 luty 2014, 12:24
 */

#ifndef QTVLIST_H
#define	QTVLIST_H

#include <QtGui>
#include <stdint.h>
#include "QLabel1.h"
#include "QMostInterface.h"

class QTvItem {
public:
    QTvItem(){};
    virtual ~QTvItem(){};
    QString    name;
    uint32_t   id;
    uint32_t   flags;
private:

};

#define ITEMS_TO_SHOW 6

class QTvList : public QMainWindow {
     Q_OBJECT
public:
    QTvList(QWidget* parent = 0);
     ~QTvList();

    void addItem(const QString &item_name,int &type);
    bool showList(void);
    
    void customEvent(QEvent* e)
    {
        if(e==NULL) return;
        
        if(e->type() == (QEvent::Type)1002)
        {	
            onCanRecieve(QCanInterface::getData());
        }
    };
    volatile bool       connected;
signals:
    void                sigTvOnOff(bool state);
    void                ChSwitchFromMOST(int ch);
    void                epgFromMOST(bool on_off);
    void                scanFromMOST(bool on_off);
    void                ChSwitchFromMOST_next_prev(bool up_down);
    void                startMediaMplayer();
    void                winDestroy(QObject *p);
        
public slots:
        void                            doSigMenuAction(int action);
private slots:
        void                            doTimer(void);
private:
    bool moveUp(void);
    bool moveDown(void);
    bool showListPortion();
    void highlightCurrentService(void);
    void onCanRecieve(QByteArray *data);
    QTimer             *hide_timer;
    int                 current_item;
    int                 selected_item;
    QLabel1              *items[6];
    QLabel              *arrow_up;
    QLabel              *arrow_down;
    QLabel              *info;
    bool                was_highlighted;
    QList<QTvItem*>     list;
    QPixmap             *image_curr[6];
    QPixmap             *image_highlight[6];
    QPixmap             *background;
    volatile uint32_t   curr_win_idx;
    volatile uint32_t   win_idx_max;
    QCanInterface       *can_interface;
    int                 saved_channels;
    
    void highlightIdx(const int &idx,bool f);
};

#endif	/* QTVLIST_H */

