/* 
 * File:   QFastList.h
 * Author: michalm
 *
 * Created on 17 marzec 2014, 08:44
 */
#include <QtGui>
#include <stdint.h>

#include "QLabel1.h"

#ifndef QFASTLIST_H
#define	QFASTLIST_H



typedef struct fast_list_item
{
    QString item_name;
    int item_type;
    uint32_t size;
    uint32_t pos_on_list;
    
} fast_list_item_t;



class QFastList : public QWidget{
    Q_OBJECT
public:
    QFastList(QWidget* parent = NULL);
    QFastList(const QFastList& orig);
    virtual ~QFastList();
    int                getListCount(){
      return list.size();  
    };
    bool               showList(void);
    void               highlightIdx(int i);
    void              setSelectedIdx(int idx);
    void              addItem(QString item)
    {
        fast_list_item *temp = new fast_list_item();
        temp->item_name = item;
        list.append(temp);  
    };
    int getCurrentIdx(void)
    {
        return current_item;  
    };

    void clearList(void)
    {
        list.clear();
    };
    //void setVisibleItems(int n);todo
    
     
private:
    void hideEvent(QHideEvent *ev)
    {
        Q_UNUSED(ev);

    };

    void showEvent(QShowEvent *ev)
    {
        Q_UNUSED(ev);
      QWidget::show();
      (void)showList();  
      update();
    };
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    bool showListPortion();
    void highlightIdx(int idx,bool f);
    
    int                     current_pos_in_dir;
    int                     current_item;
    int                     selected_item;
    int                     items_to_show; 
    QLabel1                 **items;
    QPixmap                 *folder;
    QPixmap                 *file_audio;
    QPixmap                 *file_video;
    QPixmap                 *highlight_image;
    QPixmap                 *selected_image;
    QScrollBar              *sb;
    QList<fast_list_item*>   list;
    volatile uint32_t       curr_win_idx;
    volatile uint32_t       win_idx_max;
};

#endif	/* QFASTLIST_H */

