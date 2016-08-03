/** 
 * @file   epg_win.h
 * @author michalm
 *
 * @date   20 sierpień 2012, 09:00
 */

#include <QtGui>

#include <epg_events_list.h>

#include "struct_define.h"

#define EPG_WIN_DEBUG_T       0
#define EPG_WIN_ERROR_DEBUG_T 1

#if EPG_WIN_DEBUG_T
        #define EPG_WIN_DEBUG(fmt, args...)   printf("[EPG_WIN][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define EPG_WIN_DEBUG(fmt, args...)
#endif

#if EPG_WIN_ERROR_DEBUG_T
        #define EPG_WIN_ERROR_DEBUG(fmt, args...)   printf("[EPG_WIN ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define EPG_WIN_ERROR_DEBUG(fmt, args...)
#endif

#ifndef EPG_WIN_H
#define	EPG_WIN_H

class epg_win : public QMainWindow
{
    Q_OBJECT
public:
    epg_win(QWidget* parent = 0);
    ~epg_win();
    
private:
    QListWidget                 *channel_list;
    //QListWidget                 *tab_events_list[7];
    QListWidget                 *tab_events_list;
    QPlainTextEdit              *event_description;
    
    
    QPushButton                 *day_tab[7];
    QGraphicsOpacityEffect      *win_alpha;
    dvb_epg_events_list         *epg_events_tmp;
    int                         epg_current_channel;
    dvb_service                 *current_service;
    int                         epg_current_day;
    int                         epg_current_event;
    int                         epg_current_channel_day_events_cnt;
    int                         total_channles_num;
    bool                        epg_ready;
    void fill_epg_win(void);
    void show_epg_first_time();
    
        void highlightIdx(int idx,bool f);
    
    signals:
        void sigMenuAction(int action);
    
    private:
//        void doShowWin(void);
//        void doHideWin(void);
        
        void doSigChannelHighlight(int ch);
//        void doSigDayChange(int day);
        
        void doSigHighlightEvent(int idx, int day /* = 0 */);
        void doSigAddChannelEventToList(QString ch_name);
        
        void doSigScrollUpDesc(void);
        void doSigScrollDownDesc(void);
        void doSigSetCurrentDesc(QString txt);
        void doSigClearTVChannelList();
        void doSigClearChannelEventsList(/*int day*/);
        void  doSigAddTVChannelToList(QString ch_name);
        
    private slots:
        void doSigMenuAction(int action);
        
};

#endif	/* EPG_WIN_H */

