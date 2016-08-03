/** 
 * @file   manual_scan.h
 * @author michalm
 *
 * @date   3 wrzesień 2012, 14:44
 */
#include <QtGui>
#include "QRNS_Buttons.h"

#include "wait_animation.h"
#include "dtv_core.h"
#include "QMessageWindow.h"
struct freq_tab_entry {
    uint32_t freq;
    uint8_t  band;
    uint8_t  ch;
};

const struct freq_tab_entry freq_t[57] = {
    //freq kHz,band MHz
//    {50000,7,0},
//    {57000,7,0},
//    {64000,7,0},
    {177500,7,5},
    {184500,7,6},
    {191500,7,7},
    {198500,7,8},
    {205500,7,9},
    {212500,7,10},
    {219500,7,11},
    {226500,7,12},
    
    {474000,8,21},
    {482000,8,22},
    {490000,8,23},
    {498000,8,24},
    {506000,8,25},
    {514000,8,26},
    {522000,8,27},
    {530000,8,28},
    {538000,8,29},
    {546000,8,30},
    {554000,8,31},
    {562000,8,32},
    {570000,8,33},
    {578000,8,34},
    {586000,8,35},
    {594000,8,36},
    {602000,8,37},
    {610000,8,38},
    {618000,8,39},
    {626000,8,40},
    {634000,8,41},
    {642000,8,42},
    {650000,8,43},
    {658000,8,44},
    {666000,8,45},
    {674000,8,46},
    {682000,8,47},
    {690000,8,48},
    {698000,8,49},
    {706000,8,50},
    {714000,8,51},
    {722000,8,52},
    {730000,8,53},
    {738000,8,54},
    {746000,8,55},
    {754000,8,56},
    {762000,8,57},
    {770000,8,58},
    {778000,8,59},
    {786000,8,60},
    {794000,8,61},
    {802000,8,62},
    {810000,8,63},
    {818000,8,64},
    {826000,8,65},
    {834000,8,66},
    {842000,8,67},
    {850000,8,68},
    {858000,8,69},
};

#ifndef MANUAL_SCAN_H
#define	MANUAL_SCAN_H

class manual_scan : public QMainWindow {
    Q_OBJECT
public:
    manual_scan(QWidget* parent = 0);
    ~manual_scan();
private:
    QGroupBox                   *frameCanal;
    QGroupBox                   *frameFreq;
    QRNS_Buttons                    *legend;
//    QLabel                      *legend1_wybor;
//    QLabel                      *legend2_up_down;
//    QLabel                      *legend3_exit;
//    QLabel                      *legend4_back;
    QTimer                      *scan_timer;
    QMessageWindow              *scan_msg;    
    QLabel                      *canal;
    QLabel                      *freq;
    QLabel                      *band;
    QListWidget                 *channel_list;
    int                         current_freq_idx;
    QString                     freqToStr(int val);
    wait_animation              *anim;
    pthread_t                   scan_thrd;
    static void                 *scan(void *p);
    dvb_service_list            *found_service;
    volatile bool               scan_active;
    bool                        was_scaned;
    void stopAnimation()
    {
        setUpdatesEnabled(false);
        anim->ShowAnim(false);
        setUpdatesEnabled(true);
    };
private slots:
        void doSigMenuAction(int action);
        void doScanTimerTimeOut(void);
        
signals:
        void sigMenuAction(int action);
        void sigShowAnimation(bool f);
        
};

#endif	/* MANUAL_SCAN_H */

