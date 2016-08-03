/* 
 * File:   QMpStartThrd.h
 * Author: michalm
 *
 * Created on 27 marzec 2013, 12:37
 */
#include <QtGui>

#ifndef QMPSTARTTHRD_H
#define	QMPSTARTTHRD_H

class QMpStartThrd : public QThread {
    Q_OBJECT
public:
    QMpStartThrd(QObject* parent);
     ~QMpStartThrd();
private:
    void run();
};

#endif	/* QMPSTARTTHRD_H */

