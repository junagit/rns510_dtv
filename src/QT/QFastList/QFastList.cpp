/* 
 * File:   QFastList.cpp
 * Author: michalm
 * 
 * Created on 17 marzec 2014, 08:44
 */

#include "QFastList.h"

//int temp = 0;
QFastList::QFastList(QWidget* parent) : QWidget(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);   
    setFocusPolicy(Qt::NoFocus);
    
    current_item = 0;
    curr_win_idx = 0;
    win_idx_max = 0;
    selected_item = -1;
    
    items_to_show = 5;
    
    items = new QLabel1*[items_to_show];
    
    int y = 5;
    for(int i = 0;i<items_to_show;i++) 
    {
        items[i] = NULL;
        items[i] = new QLabel1(this);
        items[i]->setGeometry(0,y,(width()-20),((height()-10)/items_to_show));
        items[i]->show();
        items[i]->setFont(QFont( "Arial", 50, QFont::Normal));
        
        y+=((height()-10)/items_to_show);
    }
    
    sb =  new QScrollBar(this);
    sb->setObjectName("scrollbar1");
    sb->setOrientation(Qt::Vertical);
    sb->setGeometry(width()-15,5,12,height()-10);
    sb->setSingleStep(1);
    sb->hide();
}

QFastList::QFastList(const QFastList& orig) 
{
    
}

QFastList::~QFastList() 
{
//  delete items[];
    for(int i = 0;i<items_to_show;i++){
        delete items[i];
    }
    
    delete[] items;
}

bool QFastList::showListPortion()
{
    int temp_add = 0;
    
    for(int i = 0;i<items_to_show;i++) items[i]->clear();
    
    if(list.size()==0) return false;
    
    if(curr_win_idx==0)
    {
        if(win_idx_max>1)
        {//jesli mamy wiecej niz jedno "okienko"
                if(list.size()>items_to_show)temp_add = items_to_show;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    fast_list_item *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)
                    {
//                        if(item_temp->item_type)
//                        {
//                            if(item_temp->item_type==1)
//                            {
//                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());
//
//                                if(av) items[j]->setIcon(file_audio);
//                                else
//                                items[j]->setIcon(file_video);
//                            } 
//                         }
//                        else
//                        items[j]->setIcon(folder);
                        
                        items[j]->setText(item_temp->item_name);
//                        items[j]->setPixmapHighlightState(highlight_image);
//                        items[j]->setPixmapSelectedState(selected_image);
                    }
                }
        }
        else
        {//jesli miescimy sie w okienku, tzn. plikow jest<=6
            if(list.size()>0)
            {
                if(list.size()>=items_to_show)temp_add = items_to_show;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    fast_list_item *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)
                    {
//                        if(item_temp->item_type)
//                        {
//                            if(item_temp->item_type==1)
//                            {
//                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());
//
//                                if(av) items[j]->setIcon(file_audio);
//                                else
//                                items[j]->setIcon(file_video);
//                            }
//                         }
//                        else
//                        items[j]->setIcon(folder);
                        
                        items[j]->setText(item_temp->item_name);
//                        items[j]->setPixmapHighlightState(highlight_image);
//                        items[j]->setPixmapSelectedState(selected_image);
                    }
                }
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        if(curr_win_idx==(win_idx_max-1))
        {//czy ostatnie okienko

            int c = ((items_to_show*curr_win_idx));
            
            if(list.size()%items_to_show==0)
            {
                for(int j = c,i = 0;j<(c+items_to_show);j++,i++)
                {
                    fast_list_item *item_temp = list.at(j);
                        
                    if(item_temp!=NULL)
                    {
//                        if(item_temp->item_type)
//                        {
//                            if(item_temp->item_type==1)
//                            {
//                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());
//
//                                if(av) items[i]->setIcon(file_audio);
//                                else
//                                items[i]->setIcon(file_video);
//                            }
//                         }
//                        else
//                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
//                        items[i]->setPixmapSelectedState(selected_image);
                    }
                }
            }
            else
            {
           
                for(int j = c,i = 0;j<(c+((list.size()%items_to_show)));j++,i++)
                {
                    fast_list_item *item_temp = list.at(j);

                    if(item_temp!=NULL)
                    {
//                        if(item_temp->item_type)
//                        {
//                            if(item_temp->item_type==1)
//                            {
//                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());
//
//                                if(av) items[i]->setIcon(file_audio);
//                                else
//                                items[i]->setIcon(file_video);
//                            }
//                         }
//                        else
//                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
//                        items[i]->setPixmapSelectedState(selected_image);
                    }
                }
            }
        }
        else
        {
            int c = (items_to_show*curr_win_idx);
            
            for(int j = c,i = 0;j<(c+items_to_show);j++,i++)
            {
                fast_list_item *item_temp = list.at(j);
                
                    if(item_temp!=NULL)
                    {
//                        if(item_temp->item_type)
//                        {
//                            if(item_temp->item_type==1)
//                            {
//                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());
//
//                                if(av) items[i]->setIcon(file_audio);
//                                else
//                                items[i]->setIcon(file_video);
//                            }
//                         }
//                        else
//                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
//                        items[i]->setPixmapSelectedState(selected_image);
                    }
            }
        }
    }

    for(int i = 0;i<items_to_show;i++) items[i]->update();
    
 return true;
   
}
bool QFastList::showList(void)
{
    //incjalizacja zmiennych win_idx_max,curr_win_idx
    setUpdatesEnabled(false);
    current_item = 0;
    
    if(list.size()==0) return false;
    
    if(list.size()>=items_to_show)
     {
         win_idx_max = (uint32_t)(list.size()/items_to_show);

         int tmp = (int)(list.size()%items_to_show);

         if(tmp>0) win_idx_max++;
     }
     else
     {
         win_idx_max = 0;
         curr_win_idx = 0;
     }
    curr_win_idx = 0;

    showListPortion();
    
    sb->setRange(0,win_idx_max);

    //pokaż suwak gdy mamy więcej niż jedno okienko
    if(win_idx_max>1)sb->show();
    else
    sb->hide();
   
    setUpdatesEnabled(true);
    
    return true;
};

void QFastList::highlightIdx(int idx,bool f)
{
//    printf("highlightIdx:%d selected_item:%d\n",idx,selected_item);
    int win_id_temp  = (uint32_t)(current_item/items_to_show);
    
    if(selected_item>=0)
    {
        //jezeli jestesmy w okienku w którym ma byc podsiwietlony wybrany element
        if(win_id_temp==(selected_item/items_to_show))
        {
            if(idx==selected_item%items_to_show)
            {//podświetl element normalnie nawet jesli to element wybrany
                if(f)items[idx]->setState(1);
                else
                items[idx]->setState(0);
            }
            else
            {//podświetl element zaznczony prze użytkownika
                if(items[selected_item%items_to_show]->getState()!=2)items[selected_item%items_to_show]->setState(2);

                if(f)items[idx]->setState(1);
                else
                items[idx]->setState(0);
            }
        }
        else
        {
            if(f)items[idx]->setState(1);
            else
            items[idx]->setState(0);
        }
    }
    else
    {
        if(f)items[idx]->setState(1);
        else
        items[idx]->setState(0);
    }
}

//void QFastList::setSelectedIdx(int idx,bool f)
//{
//    int win_id_temp  = (uint32_t)(current_item/ITEMS_TO_SHOW2);
//    
//    //jezeli jestesmy w okienku w którym ma byc podsiwietlony wybrany element
//    if(win_id_temp==(selected_item/ITEMS_TO_SHOW2))
//    {
//        int idx2 = idx%ITEMS_TO_SHOW2;
//        
//        if(f && items[idx2]->getState()!=2) items[idx2]->setState(2);
//        else
//        items[idx2]->setState(0);
//        
//        items[idx2]->update();
//    }
//}

void QFastList::highlightIdx(int i)
{
//    printf("highlightCurrentTrack:%d\n",i);
    
 
    if(i<0) return;
    
    if(i>list.size()-1) return;
    
    if(i<0)
    {
        for(int j = 0;j<items_to_show;j++)
        {
            highlightIdx(j,false);
        }
        return;
    }
    
    int idx = current_item%items_to_show;
    
    highlightIdx(idx,false);

    current_item = i;
    int win_id_temp  = (uint32_t)(current_item/items_to_show);
    
    if(win_id_temp!=curr_win_idx)
    {
        idx = current_item%items_to_show;
        curr_win_idx = (uint32_t)(current_item/items_to_show);
        showListPortion();

        highlightIdx(idx,true);
    }
    else
    {
        idx = current_item%items_to_show;
        highlightIdx(idx,true);
    }

    
    if(sb->isVisible())
    {
        if(curr_win_idx==(win_idx_max-1))
        sb->setValue(win_idx_max);
        else
        sb->setValue(curr_win_idx);
    }
    
}

void QFastList::setSelectedIdx(int idx)
{
    for(int i = 0;i<items_to_show;i++)
    {
        if(items[i]->getState()==2)
        {
            items[i]->setState(0);
            break;
        }
    }
    selected_item = idx;
    update();
}

void QFastList::wheelEvent(QWheelEvent* event)
{
//     int numDegrees = event->delta() / 8;
//     int numSteps = numDegrees / 15;
//
//     if (numSteps<0)
//     {
//         printf("numSteps1:%d\n",numSteps);
//         
//         temp--;
//         highlightIdx(temp);
//         
//     } else {
//         printf("numSteps:%d\n",numSteps);
//         temp++;
//         highlightIdx(temp);
//     }
//     event->accept();
}

void QFastList::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   printf("QFastList::resizeEvent!!!\n");
   
   int y = 5;
    for(int i = 0;i<items_to_show;i++) 
    {
        items[i]->setGeometry(0,y,(width()-20),((height()-10)/items_to_show));
        y+=((height()-10)/items_to_show);
    }
   
   sb->setGeometry(width()-15,5,12,height()-10);
   
   showListPortion();
//   update();
}