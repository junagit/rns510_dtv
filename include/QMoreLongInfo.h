/* 
 * File:   QMoreLongInfo.h
 * Author: michalm
 *
 * Created on 27 marca 2014, 07:27
 */
#include <QtGui>
#include "eit_struct_define.h"
#ifndef QMORELONGINFO_H
#define	QMORELONGINFO_H

class QMoreLongInfo : public QMainWindow {
    Q_OBJECT
public:
    QMoreLongInfo(QWidget* parent = 0,eit_event *ev = NULL,QString service_name = "");
    void    updateInfo(eit_event *ev,const char *service_name);
    void    scrollUp(void);
    void    scrollDown(void);
    virtual ~QMoreLongInfo();
private:
    QLabel              *title;
    QProgressBar        *progress;
    QLabel              *time_label;
    QLabel              *time_end_label;
    QPlainTextEdit      *txt;
    QLabel              *header;
};

#endif	/* QMORELONGINFO_H */

