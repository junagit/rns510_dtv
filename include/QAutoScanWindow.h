/** 
 * @file   QAutoScanWindow.h
 * @author Michał Motyl
 *
 * @date   10 wrzesień 2012, 14:09
 */
#include <QtGui>
#include <QUserServiceList.h>
#include "QMessageWindow.h"
#include "QAutoScanThread.h"
#include "dtv_core.h"
#include "QRNS_Buttons.h"
#ifndef QAUTOSCANWINDOW_H
#define	QAUTOSCANWINDOW_H

class QAutoScanWindow : public QMainWindow{
    Q_OBJECT
    friend class QAutoScanThread;
public:
        QAutoScanWindow(QWidget* parent = 0);
        ~QAutoScanWindow();
private:
        QRNS_Buttons                    *legend;
        pthread_t                  scan_thrd;
        QAutoScanThread            *qauto_scan_thrd;
        static void  *auto_scan_thrd(void *p);
        bool                       quit;
        bool                       scanning;
        QRNS_Buttons                    *legned;
        //QProgressBar               *scan_progress;
        QSlider                         *scan_progress;
        int                        total_channels_found;
        int                        added_channels_cnt;
        QGroupBox                  *frameCanal;
        QGroupBox                  *frameFreq;
        QGroupBox                  *frameChCnt;
//        QGroupBox                  *frameBand;

        QLabel                     *canal;
        QLabel                     *ChCnt;
        QLabel                     *freq;
//        QLabel                     *band;
        QListWidget                *channel_list;
        bool                       was_scanned;
        
//        QLabel                          *legend1_wybor;
//        QLabel                          *legend2_up_down;
//        QLabel                          *legend3_exit;
//        QLabel                          *legend4_back;

private slots:
        void doSigMenuAction(int action);
        void doUpdateProgress(int val);
        void doShowMessageAfterScan(void);
        void doAddChannelsToList(dvb_service_list *l);
signals:
        void sigMenuAction(int action);
        void updateProgress(int val);
        void sigShowMessageAfterScan(void);
        void AddChannelsToList(dvb_service_list *l);
};

#endif	/* QAUTOSCANWINDOW_H */

