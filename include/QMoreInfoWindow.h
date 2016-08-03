/** 
 * @file   QMoreInfoWindow.h
 * @author michalm
 *
 * @date   23 październik 2012, 08:56
 */
#include <QtGui>
#include "struct_define.h"
#include "epg_events_list.h"
#include "QMostInterface.h"
#include "QFastList.h"
#include "QMoreLongInfo.h"

#ifndef QMOREINFOWINDOW_H
#define	QMOREINFOWINDOW_H

class QMoreInfoWindow : public QMainWindow{
    Q_OBJECT
public:
    QMoreInfoWindow(QWidget* parent = 0,void *p = NULL);
    ~QMoreInfoWindow();
    void show_info();

    void customEvent(QEvent* e)
    {
        if(e->type() == (QEvent::Type)1002)
        {	
            onMOSTrecieve(QCanInterface::getData());
        }
    };
    
    void update_info()
    {
//        is_showed = false;
        show_info();
    };
    
    static QString fit2Label(const QString &s,QLabel *l);

    signals:
    void sigQuitFromMore(void);
    void ChSwitchFromMOST(int ch);
private:
    void updateInfo(void);
    void hideEvent(QHideEvent *ev)
    {
        is_showed = false;
    };
    
    bool                        is_showed;
    bool                        long_info_flag;
    int                         service_cnt;
    int                         current_item;
    int                         last_event_id;
    int                         info_press_cnt;
    eit_event                   *current_event;
    eit_event                   *next_event;
    
    eit_event                   last_current_event;
    eit_event                   last_next_event;
    
//    dvb_service                 *current_service;
    dvb_service                 *last_service;
    
    QCanInterface               *can;
    QFastList                   *service_list;
    QMoreLongInfo               *long_info;
    QLabel                      *title;
    QLabel                      *title2;
    
    
    
    private slots:
        void doSigMenuAction(int action);
        void onMOSTrecieve(QByteArray *data);
        
};



#endif	/* QMOREINFOWINDOW_H */

