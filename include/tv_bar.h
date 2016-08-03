/** 
 * @file   tv_bar.h
 * @author Michał Motyl
 *
 * @date   9 sierpień 2012, 10:22
 */
#include <stdint.h>
#include <QObject>
#include <QtGui>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QString>
#include "struct_define.h"
#include "epg_events_list.h"

#include "overlay.h"

#ifndef TV_BAR_H
#define	TV_BAR_H

class tv_bar : public QMainWindow {

    Q_OBJECT
public:
    tv_bar(QWidget* parent = 0);
    ~tv_bar();
    
    void set_current_channel(QString txt);
    void set_current_channel_number(QString txt);
    void set_current_time(QString txt);
    void set_current_event_name(QString txt);
    void set_record_info(bool f);
    void fill_progress(eit_event *ev);
    
private:
    QLabel *kanal;
    QLabel *czas;
    QLabel *nazwa;
    QLabel *co_leci;
    QLabel *record;
    
    QLabel *time_label;
    QLabel *time_end_label;
    QProgressBar *progress;
    //QGraphicsOpacityEffect *win_alpha;
    
    #define DEFAULT_DELAY   (5*1000000)
};

#endif	/* TV_BAR_H */

