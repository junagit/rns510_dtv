/* 
 * File:   QChannelSwitchThrd.h
 * Author: Michał Motyl
 *
 * Created on 6 luty 2013, 07:30
 */
#include <QtGui>

#ifndef QCHANNELSWITCHTHRD_H
#define	QCHANNELSWITCHTHRD_H

class QChannelSwitchThrd : public QThread {
    Q_OBJECT
public:
    QChannelSwitchThrd(QWidget* parent);
    ~QChannelSwitchThrd();
private:
    void run();
};

#endif	/* QCHANNELSWITCHTHRD_H */

