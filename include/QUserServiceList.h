/** 
 * @file   QUserServiceList.h
 * @author michalm
 *
 * @date   28 wrzesień 2012, 13:11
 */
#include <Qt>

#include "QServiceListItem.h"

#ifndef QUSERSERVICELIST_H
#define	QUSERSERVICELIST_H



class QUserServiceList  : public QWidget{
    Q_OBJECT
public:
    QUserServiceList(QWidget *parent = NULL,bool with_checkbox = false);
    ~QUserServiceList();
    void addItem(QString s);
    int getItemsCnt(void)
    {
       return gloabal_items.count();
    };
    
    void nextItem();
    void previousItem();
    void selectCurrentItem(void);
    void checkCurrentItem(bool f);
    void goToItem(int idx);
    
private:
#define VISIBLE_ITEMS 10
    
typedef struct list_item_internal{
    QString name;
    bool    state;
}_list_item_internal;

    QServiceListItem *visible_items[VISIBLE_ITEMS];
    //QStringList gloabal_items; 
    QList<list_item_internal> gloabal_items;
    int current_item;
    int visible_items_cnt;
    int visible_item_highlight;
//    QGraphicsOpacityEffect      *win_alpha;
   Q_SIGNALS:
   void sigItemSelected(QString s,int idx);
        
};

#endif	/* QUSERSERVICELIST_H */

