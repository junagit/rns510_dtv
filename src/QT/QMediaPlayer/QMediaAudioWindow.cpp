        /* 
 * File:   QMediaAudioWindow.cpp
 * Author: michalm
 * 
 * Created on 28 marzec 2013, 07:17
 */
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iosfwd>
#include "QMediaAudioWindow.h"
#include "QProgressIndicator.h"
#include "AS_language.h"
#include "MP3ID3.h"
#include "MP3Info.h"
#include "QMediaPlayer.h"
#include "app_settings.h"

#include <id3tag.h>

using namespace std;
QMediaAudioWindow::QMediaAudioWindow(QWidget* parent) : QMainWindow(parent) 
{
    setWindowFlags(Qt::FramelessWindowHint);   
    setFocusPolicy(Qt::NoFocus);

    
    //setGeometry(0,0,1024,750);
    setGeometry(0,0,1024-150,750);
    setFont(QFont( "Arial", 30, QFont::Normal));
    
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0,180));
    setPalette(pal);
//=============================================
    //OKNO Z INFO
//    tag_track_name = NULL;
//    tag_track_album = NULL;
//    tag_track_artist = NULL;
//    tag_track_genre = NULL;
    current_view = 1;
    current_pos_in_dir = 0;
    current_item = 0;
    curr_win_idx = 0;
    win_idx_max = 0;
    selected_item = -1;
   
    
    track_name = new QLabel1(this);
    track_name->setGeometry(5,5,916,70);
    
    track_progress = new QSlider(Qt::Horizontal,this);
    track_progress->setGeometry(154,648-6,552,50);
    track_progress->setMinimum(0);
    //tak ,mozna wstawic jakis obraz np.  przesuwajaca sie kulke
    //setStyleSheet("QSlider::handle {image: url(:/resources/image.png);}");

    track_duration = new QLabel(this);
    track_duration->setObjectName("simple_label");
    track_duration->setGeometry(860-149,648-6,149,50);
    track_duration->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    
    track_current_time = new QLabel(this);
    track_current_time->setObjectName("simple_label");
    track_current_time->setGeometry(0,648-6,199-50,50);
    track_current_time->setTextFormat(Qt::LogText);
    track_current_time->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    
    track_duration->setText("--:--");
    track_current_time->setText("--:--");
    //============================================
    fb_img = new QLabel(this);
    fb_img->setObjectName("simple_label");
    fb_img->setGeometry(417-50-20,698-16,60,60);
    fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_normal.png")));
    
    play_img = new QLabel(this);
    play_img->setObjectName("simple_label");
    play_img->setGeometry(482-50-20,698-16,60,60);
    play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/play_normal.png")));
    
    ff_img = new QLabel(this);
    ff_img->setObjectName("simple_label");
    ff_img->setGeometry(544-50-20,698-16,60,60);
    ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_normal.png")));
    //============================================
//    QWidget *horizontalLineWidget = new QWidget(this);
//    horizontalLineWidget->setGeometry(5,150-70,868,3);
//    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    horizontalLineWidget->setStyleSheet(QString("background-color: #ff6600;"));
//    
//    QWidget *horizontalLineWidget2 = new QWidget(this);
//    horizontalLineWidget2->setGeometry(5,620,868,3);
//    horizontalLineWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    horizontalLineWidget2->setStyleSheet(QString("background-color: #ff6600;"));
    
    QWidget *horizontalLineWidget = new QWidget(this);
    horizontalLineWidget->setGeometry(870,0,5,768);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: grey;"));
    
    QWidget *horizontalLineWidget2 = new QWidget(this);
    horizontalLineWidget2->setGeometry(0,0,5,768);
    horizontalLineWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget2->setStyleSheet(QString("background-color: grey;"));
    
    show();
    
    folder =  new QPixmap(":/img/folder.xpm");
    file_audio =  new QPixmap(":/img/file.xpm");
    file_video =  new QPixmap(":/img/movie.xpm");
    highlight_image = new QPixmap(":/img/border.xpm");
    selected_image = new QPixmap(":/img/border2.xpm");
    
    img_sd = new QPixmap(":/img/Znak_SD1.png");
    img_usb = new QPixmap(":/img/Znak_USB1.png");
    
    int y = 160-2-70;
    
    for(int i = 0;i<6;i++) 
    {
        items[i] = NULL;
        items[i] = new QLabel1(this);
        items[i]->setGeometry(5,y,855-7-5,86);
        items[i]->show();
        items[i]->setFont(QFont( "Arial", 45, QFont::Normal));
        
        y+=86;
    }

    sb = new QScrollBar(this);
    sb->setObjectName("scrollbar1");
    sb->setOrientation(Qt::Vertical);
    sb->setGeometry(1024-150-20,8+80,10,520);
    sb->setSingleStep(1);
    sb->hide();
    
    track_name->setFont(QFont( "Arial", 40, QFont::Normal));
    track_duration->setFont(QFont( "Arial", 35, QFont::Normal));
    track_current_time->setFont(QFont( "Arial", 35, QFont::Normal));
 
}

QMediaAudioWindow::~QMediaAudioWindow() 
{
//        if(tag_track_name!=NULL) 
//        {
//            delete tag_track_name;
//            tag_track_name = NULL;
//
//            delete tag_track_name_txt;
//            tag_track_name_txt = NULL;
//        }
//
//        if(tag_track_artist!=NULL)
//        {
//            delete tag_track_artist;
//            tag_track_artist= NULL;
//            
//            delete tag_track_artist_txt;
//            tag_track_artist_txt= NULL;
//        }
//
//        if(tag_track_album!=NULL) 
//        {
//            delete tag_track_album;
//            tag_track_album = NULL;
//            
//            delete tag_track_album_txt;
//            tag_track_album_txt = NULL;
//        }
//
//        if(tag_track_genre!=NULL) 
//        {
//            delete tag_track_genre;
//            tag_track_genre = NULL;
//            
//            delete tag_track_genre_txt;
//            tag_track_genre_txt = NULL;
//        }

        
    delete folder;
    delete file_audio;
    delete file_video;
    delete highlight_image;
    delete selected_image;
    delete img_sd;
    delete img_usb;
}

void QMediaAudioWindow::setTrackTitle(const QString &title,int src_id)
{
    track_path = title;
   // printf("setTrackTitle:%s\n",track_path.toAscii().data());
    track_name->setText(title);
    
    if(src_id==SOURCE_USB1)
    {
        track_name->setIcon(img_usb);
    }
    else
    {
        track_name->setIcon(img_sd);
    }
    
    track_name->update();
    
}


 void QMediaAudioWindow::setTrackLen(int val)
 {
     track_progress->setMinimum(0);
     track_progress->setMaximum(val);
     totalTimeDisplay(val/1000);
 }

 void QMediaAudioWindow::setTrackPos(int val)
 {
     if(track_progress->value()!=val)
     {
        track_progress->setValue(val);
        currentTimeDisplay(val/1000);
     }
 }
  
 void QMediaAudioWindow::totalTimeDisplay(int v)
 {
     int hour = 0;
     int min = 0;
     int sec = 0;
     int temp = 0;
     QString t;
     
     hour = (int)(v/3600);
     
     if(hour) 
     {
         temp = v-(hour*3600);
         min = temp/60;
     }
     else
     min = (int)(v/60);
     
     if(min) 
     {
         temp = v-(hour*3600) - (min*60);
         sec = temp%60;
     }
     else
     {
         sec = v%60;
     }
     
       
     if(hour)
     {//wyswietlamy tylko liczbe godzin jesli są
        t.append(QString().sprintf("%02d",hour));
        t.append(":");
     }
     
    t.append(QString().sprintf("%02d",min));
    t.append(":");
    t.append(QString().sprintf("%02d",sec));

     track_duration->setText(t);
 }
 
  void QMediaAudioWindow::currentTimeDisplay(int v)
 {
     int hour = 0;
     int min = 0;
     int sec = 0;
     int temp = 0;
     QString t;
     
     hour = (int)(v/3600);
     
     if(hour) 
     {
         temp = v-(hour*3600);
         min = temp/60;
     }
     else
     min = (int)(v/60);
     
     if(min) 
     {
         temp = v-(hour*3600) - (min*60);
         sec = temp%60;
     }
     else
     sec = v%60;
     
     
     if(hour)
     {//wyswietlamy tylko liczbe godzin jesli są
        t.append(QString().sprintf("%02d",hour));
        t.append(":");
     }
    t.append(QString().sprintf("%02d",min));
    t.append(":");
    t.append(QString().sprintf("%02d",sec));
         
     track_current_time->setText(t);
 }
  
  
  
//void QMediaAudioWindow::switchToView(int i,const QString &current_file_path)
//{
//
//}


void QMediaAudioWindow::setTrackPath(const QString &s)
{
    track_path = s;   
}

//void QMediaAudioWindow::toggleView(const QString &current_dir,bool mode)
//{
//        current_view = (int)mode;
////        switchToView(current_view,current_dir);
//}

void QMediaAudioWindow::highlightCurrentTrack(int i, bool font_color_change)
{
//    printf("highlightCurrentTrack:%d\n",i);
    
    if(i<0)
    {
        for(int j = 0;j<6;j++)
        {
            highlightIdx(j,false);
        }
        return;
    }
    
    int idx = current_item%ITEMS_TO_SHOW2;
    
    highlightIdx(idx,false);

    current_item = i;
    int win_id_temp  = (uint32_t)(current_item/ITEMS_TO_SHOW2);
    
    if(win_id_temp!=curr_win_idx)
    {
        idx = current_item%ITEMS_TO_SHOW2;
        curr_win_idx = (uint32_t)(current_item/ITEMS_TO_SHOW2);
        showListPortion();

        highlightIdx(idx,true);
    }
    else
    {
        idx = current_item%ITEMS_TO_SHOW2;
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

void QMediaAudioWindow::setSelectedItem(int idx)
{
    for(int i = 0;i<6;i++)
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


 void QMediaAudioWindow::setPlayState(bool f)
 {
     if(f==true)
     play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/play_selected.png")));
     else
     play_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/pause_selected.png")));
 }



 void QMediaAudioWindow::setFfState(bool f)
 {
     if(f==true)
     ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_selected.png")));
     else
     ff_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/ff_normal.png")));
 }

 void QMediaAudioWindow::setFbState(bool f)
 {
     if(f==true)
     fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_selected.png")));
     else
     fb_img->setPixmap(QPixmap(QString::fromUtf8(":/img/player/fb_normal.png")));
     
 }
 
 void QMediaAudioWindow::addItemToList(browser_item *item_temp,bool do_show_list)
 {
     if(do_show_list==false)
     {
        list.append(item_temp);
     }
     else
     {
         if(list.size()) showList();
     }
 }
 
 bool QMediaAudioWindow::showListPortion()
{
    int temp_add = 0;
    
    for(int i = 0;i<ITEMS_TO_SHOW2;i++) 
    {
        items[i]->clear();
//        items[i]->setHighlightMode(true);
    }
    
    if(list.size()==0) return false;
    
    if(curr_win_idx==0)
    {
        if(win_idx_max>1)
        {//jesli mamy wiecej niz jedno "okienko"
                if(list.size()>ITEMS_TO_SHOW2)temp_add = ITEMS_TO_SHOW2;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    browser_item *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)
                    {
                        if(item_temp->item_type)
                        {
                            if(item_temp->item_type==1)
                            {
                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());

                                if(av) items[j]->setIcon(file_audio);
                                else
                                items[j]->setIcon(file_video);
                            } 
                         }
                        else
                        items[j]->setIcon(folder);
                        
                        items[j]->setText(item_temp->item_name);
//                        items[j]->setPixmapHighlightState(highlight_image);
                        items[j]->setPixmapSelectedState(selected_image);
                    }
                }
        }
        else
        {//jesli miescimy sie w okienku, tzn. plikow jest<=6
            if(list.size()>0)
            {
                if(list.size()>=ITEMS_TO_SHOW2)temp_add = ITEMS_TO_SHOW2;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    browser_item *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)
                    {
                        if(item_temp->item_type)
                        {
                            if(item_temp->item_type==1)
                            {
                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());

                                if(av) items[j]->setIcon(file_audio);
                                else
                                items[j]->setIcon(file_video);
                            }
                         }
                        else
                        items[j]->setIcon(folder);
                        
                        items[j]->setText(item_temp->item_name);
//                        items[j]->setPixmapHighlightState(highlight_image);
                        items[j]->setPixmapSelectedState(selected_image);
                    }
                }
            }
            else
            {
                printf("BRAK PLIKOW W KATALOGU???\n");   
            }
        }
    }
    else
    {
        if(curr_win_idx==(win_idx_max-1))
        {//czy ostatnie okienko

            int c = ((ITEMS_TO_SHOW2*curr_win_idx));
            
            if(list.size()%ITEMS_TO_SHOW2==0)
            {
                for(int j = c,i = 0;j<(c+ITEMS_TO_SHOW2);j++,i++)
                {
                    browser_item *item_temp = list.at(j);
                        
                    if(item_temp!=NULL)
                    {
                        if(item_temp->item_type)
                        {
                            if(item_temp->item_type==1)
                            {
                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());

                                if(av) items[i]->setIcon(file_audio);
                                else
                                items[i]->setIcon(file_video);
                            }
                         }
                        else
                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
                        items[i]->setPixmapSelectedState(selected_image);
                    }
                }
            }
            else
            {
           
                for(int j = c,i = 0;j<(c+((list.size()%ITEMS_TO_SHOW2)));j++,i++)
                {
                    browser_item *item_temp = list.at(j);

                    if(item_temp!=NULL)
                    {
                        if(item_temp->item_type)
                        {
                            if(item_temp->item_type==1)
                            {
                                bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());

                                if(av) items[i]->setIcon(file_audio);
                                else
                                items[i]->setIcon(file_video);
                            }
                         }
                        else
                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
                        items[i]->setPixmapSelectedState(selected_image);
                    }
                }
            }
        }
        else
        {
            int c = (ITEMS_TO_SHOW2*curr_win_idx);
            
            for(int j = c,i = 0;j<(c+ITEMS_TO_SHOW2);j++,i++)
            {
                browser_item *item_temp = list.at(j);
                
                    if(item_temp!=NULL)
                    {
                        if(item_temp->item_type)
                        {
                            if(item_temp->item_type==1)
                            {
                            bool av = QMediaPlayer::isAudioOrVideo((char*)item_temp->item_name.toAscii().data());

                                if(av) items[i]->setIcon(file_audio);
                                else
                                items[i]->setIcon(file_video);
                            }
                         }
                        else
                        items[i]->setIcon(folder);
                        
                        items[i]->setText(item_temp->item_name);
//                        items[i]->setPixmapHighlightState(highlight_image);
                        items[i]->setPixmapSelectedState(selected_image);
                    }
            }
        }
    }

    for(int i = 0;i<ITEMS_TO_SHOW2;i++) items[i]->update();
    
 return true;
   
}
 
bool QMediaAudioWindow::moveUp(void)
{
    if(current_item==0) return false;

    highlightIdx(current_item%ITEMS_TO_SHOW2,false);
    
    --current_item;

    if(((current_item+1)%ITEMS_TO_SHOW2)==0)
    {
        if(curr_win_idx>0)
        {
           // items[selected_item%ITEMS_TO_SHOW2]->setStyleSheet("border: none; color: white; background: none;");
            curr_win_idx--;
            showListPortion();
        }
    }
   
    highlightIdx(current_item%ITEMS_TO_SHOW2,true);
    return true;
 }


 bool QMediaAudioWindow::moveDown(void)
{  
    if((current_item<(list.size()-1))==false) return false;
    
    highlightIdx(current_item%ITEMS_TO_SHOW2,false);
    
    current_item++;
    
    int temp = current_item%ITEMS_TO_SHOW2;
    
    highlightIdx(temp,true);
    
    if(temp==0)
    {
        curr_win_idx++;
        showListPortion();
    }
    
    return true;
};
    
    
bool QMediaAudioWindow::showList(void)
{
    //incjalizacja zmiennych win_idx_max,curr_win_idx
    setUpdatesEnabled(false);
    current_item = 0;
    
    if(list.size()>=ITEMS_TO_SHOW2)
     {
         win_idx_max = (uint32_t)(list.size()/ITEMS_TO_SHOW2);

         int tmp = (int)(list.size()%ITEMS_TO_SHOW2);

         if(tmp>0) win_idx_max++;
     }
     else
     {
         win_idx_max = 0;
         curr_win_idx = 0;
     }
    
//    curr_win_idx = (uint32_t)(current_item/ITEMS_TO_SHOW2);
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


void QMediaAudioWindow::highlightIdx(int idx,bool f)
{
//    printf("highlightIdx:%d selected_item:%d\n",idx,selected_item);
    int win_id_temp  = (uint32_t)(current_item/ITEMS_TO_SHOW2);
    
    if(selected_item>=0)
    {
        //jezeli jestesmy w okienku w którym ma byc podsiwietlony wybrany element
        if(win_id_temp==(selected_item/ITEMS_TO_SHOW2))
        {
            if(idx==selected_item%ITEMS_TO_SHOW2)
            {//podświetl element normalnie nawet jesli to element wybrany
                if(f)items[idx]->setState(1);
                else
                items[idx]->setState(0);
            }
            else
            {//podświetl element zaznczony prze użytkownika

                if(items[selected_item%ITEMS_TO_SHOW2]->getState()!=2)items[selected_item%ITEMS_TO_SHOW2]->setState(2);

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

void QMediaAudioWindow::setSelectedIdx(int idx,bool f)
{
    int win_id_temp  = (uint32_t)(current_item/ITEMS_TO_SHOW2);
    
    //jezeli jestesmy w okienku w którym ma byc podsiwietlony wybrany element
    if(win_id_temp==(selected_item/ITEMS_TO_SHOW2))
    {
        int idx2 = idx%ITEMS_TO_SHOW2;
        
        if(f && items[idx2]->getState()!=2) items[idx2]->setState(2);
        else
        items[idx2]->setState(0);
        
        items[idx2]->update();
    }
}