/* 
 * File:   most_tv_item.h
 * Author: juna
 *
 * Created on 16 marzec 2013, 15:21
 */
#include <QtGui>
#include <stdint.h>

#ifndef MOST_TV_ITEM_H
#define	MOST_TV_ITEM_H



class most_tv_item : public QObject {
    Q_OBJECT
public:
    
    enum item_type {
    CTRL_ITEM,//EPG,SCAN,INFO itp
    NORMAL_ITEM,//nazwa stacji
    EMPTY_ITEM,//pusta pozycja
    CTRL_NEXT_LIST,//dla MOST G2 z krótkimi listami do 20 pozycji
    CTRL_PREV_LIST//dla MOST G2 z krótkimi listami do 20 pozycji
};

enum item_state{
    SELECTED,
    NO_SELECTED
};

    most_tv_item(QObject* parent = NULL);
    ~most_tv_item();
    
    void setItemName(QString s){name = s;};
    
    QString getItemName(){return name;};
    
    void setItemState(item_state f)
    {
        state = f;
        
        //jesli typ pusty to nie wysylamy żadnych sygnałów
        if(type==EMPTY_ITEM) return;
        
        if(f==SELECTED) emit sigTvItemSelected(this);
        else
            emit sigTvItemDeSelected(this);
    };
    
    item_state getItemState(){return state;};
    
    void setItemType(item_type f){type = f;};
    
    item_type getItemType(){return type;};
    
    void setItemId(int32_t id){item_id = id;};
    
    int32_t getItemId(){return item_id;};
    uint32_t    freq;
    uint16_t    service_id;
    uint16_t    ts_service_id;
signals:
   void sigTvItemSelected(most_tv_item *p);
   void sigTvItemDeSelected(most_tv_item *p);
   
private:
    QString     name;
    item_type   type;
    item_state  state;
    int32_t     item_id;
    
};

#endif	/* MOST_TV_ITEM_H */

