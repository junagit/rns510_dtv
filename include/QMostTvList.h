/* 
 * File:   QMostTvList.h
 * Author: michalm
 *
 * Created on 24 wrzesień 2013, 10:06
 */

#include <QtGui>

#include "QMostInterface.h"
#include "QMediaItem.h"
#include "most_tv_item.h"
#include "struct_define.h"
#include "dvbtcontrol.h"

#ifndef QMOSTTVLIST_H
#define	QMOSTTVLIST_H

#define MEDIA_STOP              0
#define MEDIA_RESUME            1

#define QMOST_TV_LIST_DEBUG_T       1
#define QMOST_TV_LIST_ERROR_DEBUG_T 1

#if QMOST_TV_LIST_DEBUG_T
        #define QMOST_LIST_TV_DEBUG(fmt, args...)   printf("[QMOST_TV_LIST][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define QMOST_TV_LIST_DEBUG(fmt, args...)
#endif

#if QMOST_TV_LIST_ERROR_DEBUG_T
        #define QMOST_TV_LIST_ERROR_DEBUG(fmt, args...)   printf("[QMOST_TV_LIST ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define QMOST_TV_LIST_ERROR_DEBUG(fmt, args...)
#endif

class QCANrcTV : public QObject{
    Q_OBJECT
public:
    QCANrcTV(QObject* parent = NULL);
    ~QCANrcTV();
    
    
    static bool     menu_from_can;
    int32_t             getCurrentItemIdx(void);
    bool                sendServiceList()
    {
      if(can_interface)  return can_interface->sendServiceList();
      
      return false;
    };
    
    bool        set_pos_on_can(int pos, char * info)
    {
        if(can_interface)  return can_interface->set_pos_on_can(pos,info);
    };
//    most_tv_item        *getCurrentItem(void);
//    int32_t             getListSize();
//    void                setNoServicesMsg(void);
//    void                switchToMediaPlayer();
//    void                addTvServiceToList(dvb_service *service,int list_idx);
    void                disableCANsignals(bool f);
    
    void customEvent(QEvent* e);
    
signals:
    void                sigTvOnOff(bool state);
    void                epgFromMOST(bool on_off);
    void                ChSwitchFromCAN(int ch);
    void                epgFromCAN(bool on_off);
    void                scanFromCAN(bool on_off);
    void                ChSwitchFromCAN_next_prev(bool up_down);
    void                sendList(void);
    void                startMediaMplayer();
    void                sig_can_rc_code(char code);
    void                ChSwitchFromMOST(int ch);

    
private:
    QCanInterface       *can_interface;
//    QList<most_tv_item*>*tv_list;
    volatile int32_t    current_item;
    volatile int32_t    current_item_tmp;
    volatile bool       tv_media;
    volatile bool       epg_state;
    volatile bool       connected;
    volatile int        last_ch_idx;
    QTimer              sw_timer;
    void                sortList(void);
 private slots:
    void                onCanRecieve(QByteArray *data);
    void                DoChannelSwitch(void);

};

#endif	/* QMOSTTVLIST_H */

