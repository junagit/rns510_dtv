/** 
 * @file   QOtherOptWindow.h
 * @author michalm
 *
 * @date   10 wrzesień 2012, 11:45
 */
#include <QtGui>
#include "QmenuTvItem.h"
#include "app_settings.h"
#define TV_RECORD_LOCATION_SD_CARD      0
#define TV_RECORD_LOCATION_USB          1
#define MAX_TV_OPT 4

#include "QRNS_Buttons.h"

#ifndef QOTHEROPTWINDOW_H
#define	QOTHEROPTWINDOW_H

class QOtherOptWindow : public QMainWindow {
    
    
    Q_OBJECT
public:
    QOtherOptWindow(QWidget* parent = 0,int to_select = -1);
    ~QOtherOptWindow();
    static const int LANGUAGE_CHANGE_CUSTOM_EVENT=1001;
private:
        QGraphicsOpacityEffect          *win_alpha;
        QLabel                          *legend1_wybor;
        QLabel                          *legend2_up_down;
        QLabel                          *legend3_exit;
        QLabel                          *legend4_back;
        QRNS_Buttons                    *legned;
        QmenuTvItem                     *menu_items[MAX_TV_OPT];
        app_settings                    *application_settings;
        int                             current_item;
        int                             tv_record_location;
        int                             antena_power;
        int                             global_alpha;
        int                             osd_time;
        QOtherOptWindow                 *self_p;
        static void cb1(void *p);
        static void czas_osd_cb(void *p);
        static void win_alpha_cb(void *p);
        static void audio_type_cb(void *p);
        static void sys_vol_cb(void *p);
        static void language_cb(void *p);
        static void sys_info_cb(void *p);
        
private slots:
        void doSigMenuAction(int action);
        
signals:
        void sigMenuAction(int action);


};

#endif	/* QOTHEROPTWINDOW_H */

