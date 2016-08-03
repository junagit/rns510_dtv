/* 
 * File:   QMediaMplayer.h
 * Author: michalm
 *
 * Created on 26 wrzesień 2013, 11:55
 */
#include <QtGui>

#ifndef QMEDIAMPLAYER_H
#define	QMEDIAMPLAYER_H

class QMediaMplayer  : public QObject{
    Q_OBJECT
public:
    QMediaMplayer(QObject* parent);
    ~QMediaMplayer();
private:

};

#endif	/* QMEDIAMPLAYER_H */

