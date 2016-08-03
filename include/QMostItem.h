/* 
 * File:   QMostItem.h
 * Author: michalm
 *
 * Created on 20 wrzesień 2013, 12:12
 */
#include <QtGui>

#include <stdio.h>
#include <stdint.h>

#ifndef QMOSTITEM_H
#define	QMOSTITEM_H

class QMostItem {
public:
    QMostItem();
    virtual ~QMostItem();
    QString    name;
    uint32_t   id;
    uint32_t   flags;
private:

};

#endif	/* QMOSTITEM_H */

