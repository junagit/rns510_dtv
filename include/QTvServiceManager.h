/** 
 * @file   QTvServiceManager.h
 * @author michalm
 *
 * @date   6 wrzesień 2012, 13:31
 */

#include <QtGui>

#include "service_management.h"
#include "QRNS_Buttons.h"

#ifndef QTVSERVICEMANAGER_H
#define	QTVSERVICEMANAGER_H

class QTvServiceManager : public QMainWindow {
    Q_OBJECT
public:
    QTvServiceManager(QWidget* parent = 0);
    ~QTvServiceManager();
private:
    QListWidget                 *channel_list;
    int                         current_ch_idx;
    int                         channels_cnt;
    int                         selected_idx;
    service_manager             *service_management;
    QRNS_Buttons                *legend;
    int                         action_cnt;
//    QLabel                          *legend1_wybor;
//    QLabel                          *legend2_up_down;
//    QLabel                          *legend3_exit;
//    QLabel                          *legend4_back;
public slots:
        void doSigMenuAction(int action);
        void doConnect(void)
        {
            connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
        };
        
signals:
        void sigMenuAction(int action);

};

#endif	/* QTVSERVICEMANAGER_H */

