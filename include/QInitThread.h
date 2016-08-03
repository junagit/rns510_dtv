/* 
 * File:   QInitThread.h
 * Author: michalm
 *
 * Created on 6 luty 2013, 08:08
 */
#include <QtGui>

#ifndef QINITTHREAD_H
#define	QINITTHREAD_H

class QInitThread : public QThread {
    Q_OBJECT
public:
    QInitThread(QWidget* parent);
    ~QInitThread();
private:
    void run();
};

#endif	/* QINITTHREAD_H */

