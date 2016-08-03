/* 
 * File:   QMediaItem.cpp
 * Author: michalm
 * 
 * Created on 21 marzec 2013, 07:25
 */

#include "QMediaItem.h"

QMediaItem::QMediaItem(QObject* parent) : QObject(parent) {
    type = NORMAL_ITEM;
    state = NO_SELECTED;

}


QMediaItem::~QMediaItem() {
}

