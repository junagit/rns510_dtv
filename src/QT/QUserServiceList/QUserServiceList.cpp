/** 
 * @file   QUserServiceList.cpp
 * @author michalm
 *
 * @date   28 wrzesień 2012, 13:11
 */
#include <Qt>
#include <QtCore/qstring.h>
#include "QUserServiceList.h"

QUserServiceList::QUserServiceList(QWidget *parent,bool with_checkbox) : QWidget(parent)
{
    int temp = 0;
    
//    setWindowFlags(Qt::FramelessWindowHint);    
//    setObjectName("mm");
//    win_alpha = new QGraphicsOpacityEffect(NULL);
//    win_alpha->setOpacity(0.75);
//    setGraphicsEffect(win_alpha);
    
    for(int i = 0;i<VISIBLE_ITEMS;i++) 
    {
        visible_items[i] = NULL;
        visible_items[i] = new QServiceListItem(this,"",with_checkbox);
        visible_items[i]->setGeometry(0,temp,-1,-1);
        temp+=visible_items[i]->height()+1;
    }
    
    setGeometry(20,20,visible_items[0]->width(),((visible_items[0]->width()*10)+10));
    
    current_item = 0;
    visible_items_cnt = 0;
    visible_item_highlight = 0;
    gloabal_items.clear();
    show();
}



QUserServiceList::~QUserServiceList() 
{
    gloabal_items.clear();
    hide();
    parentWidget()->update();
}

void QUserServiceList::addItem(QString s)
{
    list_item_internal p;
    
    p.name = s;
    p.state = false;
    
    
    gloabal_items.append(p);
    if(visible_items_cnt<VISIBLE_ITEMS) 
    {
        visible_items[visible_items_cnt]->setText(gloabal_items.at(visible_items_cnt).name);
        if(visible_items_cnt==0) {
            visible_items[visible_items_cnt]->selectItem(true);
            visible_items[visible_items_cnt]->checkItem(true);
           
        }
        
        if(visible_items_cnt==4) visible_items[visible_items_cnt]->checkItem(true); 
        visible_items_cnt++;
    }
    parentWidget()->update();
}


void QUserServiceList::nextItem()
{
    int j = 0;
    int temp;
    
    if(!(current_item<gloabal_items.count()-1)) return ;

    current_item++;

    if(visible_item_highlight>=(VISIBLE_ITEMS-1))
    { 
        temp = (current_item-(VISIBLE_ITEMS-1));
        if(temp<0) temp = 0;

            j = temp;
            for(int i = 0;i<VISIBLE_ITEMS;i++)
            {
                visible_items[i]->checkItem(false);
                visible_items[i]->setText(gloabal_items.at(j).name);
                
                if(gloabal_items.at(j).state==true) visible_items[i]->checkItem(true);
                
                j++;
            }
    }
    else
    {
        //jesli podswietlona pozycja jest < od 10 to podświtlamy tylko
        int j = current_item;
        j--;
        visible_items[j]->selectItem(false);
        visible_items[current_item]->selectItem(true);
        visible_item_highlight = current_item;
    } 

}

void QUserServiceList::previousItem()
{
    int j = 0;

    if(current_item==0) return;
    
    current_item--;

    if(visible_item_highlight>0)
    {
        visible_items[visible_item_highlight]->selectItem(false);
        visible_item_highlight--;
        visible_items[visible_item_highlight]->selectItem(true);
    }
    else
    {
        j = current_item;
        for(int i = 0;i<VISIBLE_ITEMS;i++)
        {
            visible_items[i]->checkItem(false);
            visible_items[i]->setText(gloabal_items.at(j).name);
            
            if(gloabal_items.at(j).state==true) visible_items[i]->checkItem(true);
                
            j++;
        }
    }

}

/**
 * @brief Funkcja emituje sygnal dla użytkownika z podswietlonym elementem
 */
void QUserServiceList::selectCurrentItem(void)
{
    emit sigItemSelected(gloabal_items.at(current_item).name,current_item);
}

void QUserServiceList::checkCurrentItem(bool f)
{
    list_item_internal p;
    int j,temp;
    p = gloabal_items.at(current_item);
    p.state = f;
    gloabal_items.removeAt(current_item);
    gloabal_items.insert(current_item,p);
    
    
    if(visible_item_highlight>=0 && visible_item_highlight<=VISIBLE_ITEMS-1)
    {
        j = current_item - visible_item_highlight;
    }
    else
    {
        temp = (current_item-(VISIBLE_ITEMS-1));
        if(temp<0) temp = 0;

        j = temp;
    }
    
    
    for(int i = 0;i<VISIBLE_ITEMS;i++)
    {
        visible_items[i]->checkItem(false);
        visible_items[i]->setText(gloabal_items.at(j).name);

        if(gloabal_items.at(j).state==true) visible_items[i]->checkItem(true);

        j++;
    }
    
    parentWidget()->update();
}

void QUserServiceList::goToItem(int idx)
{
    if(idx>9)
    {
        
    }
    else
    {
        
    }
}

