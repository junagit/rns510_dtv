/* 
 * File:   QMediaItem.h
 * Author: michalm
 *
 * Created on 21 marzec 2013, 07:25
 */
#include <QtGui>
#include <stdint.h>

#ifndef QMEDIAITEM_H
#define	QMEDIAITEM_H

class QMediaItem : public QObject{
    Q_OBJECT
public:
        enum media_item_type {
    CTRL_ITEM,
    NORMAL_ITEM,//nazwa stacji
    DIRECTORY_ITEM,
    EMPTY_ITEM,//pusta pozycja

};

enum media_item_state{
    SELECTED,
    NO_SELECTED
};
    QMediaItem(QObject* parent);
    ~QMediaItem();
    
    void selectItem(media_item_state f)
    {
        
        
        //jesli typ pusty to nie wysylamy żadnych sygnałów
        if(type==EMPTY_ITEM) return;
        state = f;
        emit sigMediaItemSelected(this);

    };
    
    QString             file_name;
    media_item_type     type;
    media_item_state    state;
    uint32_t            id;
    
signals:
   void sigMediaItemSelected(QMediaItem *p);
   
private:

};

#endif	/* QMEDIAITEM_H */

