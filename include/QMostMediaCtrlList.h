/* 
 * File:   QMostMediaCtrlList.h
 * Author: michalm
 *
 * Created on 24 wrzesień 2013, 12:29
 */

#include <QtGui>

#include "QMostInterface.h"
#include "QMediaItem.h"
#include "most_tv_item.h"

#ifndef QMOSTMEDIACTRLLIST_H
#define	QMOSTMEDIACTRLLIST_H

enum QMostMediaCtrlList_Evt{
    EVT_TV,
    EVT_USB,
    EVT_USB_C,
    EVT_SD,
    EVT_SD_C,
    EVT_PLAY,
    EVT_PAUSE,
    EVT_NEXT,
    EVT_PREV,
    EVT_VIEW,
    EVT_MEDIA_STOP,
    EVT_MEDIA_RESUME,
    EVT_SEND_LIST
};

class QMostMediaCtrlList  : public QObject{
    Q_OBJECT
public:
    QMostMediaCtrlList(QObject* parent);
    ~QMostMediaCtrlList();
    bool                        sendMediaCtrlList(void);
    void                        disableMOSTsignals(bool f);
    void customEvent(QEvent* e)
    {
        if(e->type() == (QEvent::Type)1002)
        {	
            onMOSTrecieve(QCanInterface::getData());
        }
    };
    Q_SIGNALS:
    void                        sigTV(void);
    void                        sigPlay(void);
    void                        sigPause(void);
    void                        sigNext(void);
    void                        sigPrev(void);
    void                        sigView(void);
    void                        sendList(void);
    void                        sigMediaCtrlList(int evt);
private:
    #define MEDIA_STOP              0
    #define MEDIA_RESUME            1
    QCanInterface              *can_interface;
    QList<QMediaItem*>          *media_ctrl_list;
    int                         current_media_source;
    bool                        media_player_state;
    bool                        connected;
    void                        createCtrlList(void);
    void                        selectMediaItem(int32_t idx,bool emit_signal);
    void                        addMediaItemCtrl(QString name, QMediaItem::media_item_type type, uint32_t id, const QObject *receiver, const char *member);
private slots:
    void                        onMOSTrecieve(QByteArray *data);
    void                        onMediaCtrlItemCB(QMediaItem *p);

};

#endif	/* QMOSTMEDIACTRLLIST_H */

