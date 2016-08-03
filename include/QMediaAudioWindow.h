/* 
 * File:   QMediaAudioWindow.h
 * Author: michalm
 *
 * Created on 28 marzec 2013, 07:17
 */

#include <QtGui>
#include "file_browser.h"
//#include "scrolltext.h"
#include "scroll.h"
#include "scrolltext.h"
#include "NewList.h"
#include "QLabel1.h"

#ifndef QMEDIAAUDIOWINDOW_H
#define	QMEDIAAUDIOWINDOW_H


#define ITEMS_TO_SHOW2 6
//class MyItemDelegate : public QAbstractItemDelegate
//{
//public:
//     MyItemDelegate(QObject* parent = 0) : QAbstractItemDelegate(parent)
//     {
//     };
//     
//     void paint(QPainter *painter, const QStyleOptionViewItem &option,
//                            const QModelIndex &index) const
//        {
//         painter->setFont(QFont( "Arial", 39, QFont::Normal));
//         painter->setPen(Qt::black);
//         
//         if (option.state & QStyle::State_Selected)
//         {
//             painter->fillRect(option.rect, option.palette.highlight());
//         }
//         else
//         {
//             painter->fillRect(option.rect,Qt::white);
//         }
//      
//         painter->drawText(option.rect,index.data().toString());
//         
//     } ;
// 
//     QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
//     {
//         return QSize(30, 50);
//     };
//};

class QMediaAudioWindow : public QMainWindow{
    friend class QMediaPlayer;
    Q_OBJECT
public:
    QMediaAudioWindow(QWidget* parent = 0);
     ~QMediaAudioWindow();
     void               setTrackTitle(const QString &title,int src_id);
     void               setTrackPath(const QString &s);
     void               setTrackLen(int val);                       
     void               setTrackPos(int val);
//     void               toggleView(const QString &current_dir,bool mode);
     void               highlightCurrentTrack(int i, bool font_color_change);
     void               setSelectedItem(int idx);
     int               getCurrentIdx(void)
     {
       return current_item;  
     };

//     void               switchToView(int i,const QString &current_file_path);
     void               addItemToList(browser_item *item_temp,bool do_show_list);
    
     void               clearFileList(void)
     {
//         if(file_list!=NULL) file_list->clear();
         list.clear();
     };
private:
    QSlider             *track_progress;
    QLabel1             *track_name;
    QLabel              *track_duration;
    QLabel              *track_current_time;
    QScrollBar          *sb;

     void               totalTimeDisplay( int v);
     void               currentTimeDisplay(int v);
     
     void               setPlayState(bool f);
     void               setFfState(bool f);
     void               setFbState(bool f);
     
     //===============================================
    bool showListPortion();
    bool showList(void);
    bool moveDown(void);
    bool moveUp(void);
    void highlightIdx(int idx,bool f);
    void setSelectedIdx(int idx,bool f);
    
//     QLabel             *tag_track_name;
//     QLabel             *tag_track_name_txt;
//     
//     QLabel             *tag_track_album;
//     QLabel             *tag_track_album_txt;
//     
//     QLabel             *tag_track_artist;
//     QLabel             *tag_track_artist_txt;
//     
//     QLabel             *tag_track_genre;
//     QLabel             *tag_track_genre_txt;
     
     QLabel             *source_img;
     QLabel             *source_img2;
     
     QLabel             *play_img;
     QLabel             *ff_img;
     QLabel             *fb_img;
     
     QString            track_path;
     int                current_view;
     int                current_pos_in_dir;
     //================================================
     QTimer             *hide_timer;
    int                 current_item;
    int                 selected_item;
    QLabel1             *items[6];
    QLabel              *arrow_up;
    QLabel              *arrow_down;
    QPixmap             *folder;
    QPixmap             *file_audio;
    QPixmap             *file_video;
    QPixmap             *highlight_image;
    QPixmap             *selected_image;
    QPixmap             *img_sd;
    QPixmap             *img_usb;
    QList<browser_item*>     list;
    volatile uint32_t   curr_win_idx;
    volatile uint32_t   win_idx_max;
    
       
};

#endif	/* QMEDIAAUDIOWINDOW_H */

