/* 
 * File:   most_tv_item.cpp
 * Author: juna
 * 
 * Created on 16 marzec 2013, 15:21
 */

#include <stdio.h>

#include "most_tv_item.h"

most_tv_item::most_tv_item(QObject* parent) : QObject(parent)  {
    type = NORMAL_ITEM;
    state = NO_SELECTED;
    item_id = -1;
}


most_tv_item::~most_tv_item() {
    
}


