/* 
 * File:   QAutoScanThread.h
 * Author: juna
 *
 * Created on 31 grudzień 2013, 08:52
 */

#ifndef QAUTOSCANTHREAD_H
#define	QAUTOSCANTHREAD_H

#include <QtGui>

class QAutoScanThread  : public QThread {
    Q_OBJECT
public:
    QAutoScanThread(QWidget* parent);
    ~QAutoScanThread();
private:
    void run();
};

#endif	/* QAUTOSCANTHREAD_H */

