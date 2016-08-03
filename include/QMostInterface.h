/* 
 * File:   QMostInterface.h
 * Author: michalm
 *
 * Created on 20 wrzesień 2013, 09:58
 */
#include <QtGui>

#include "serialport.h"
#include "QMostItem.h"
#include "QMediaItem.h"
#include "most_tv_item.h"

#ifndef QMOSTINTERFACE_H
#define	QMOSTINTERFACE_H

#define STX                     (0x02)
#define ETB                     (0x17)
#define ETX                     (0x03)

#define MOST_UART_PORT          "/dev/ttyS1"

#define QMOST_DEBUG_T 1
#define QMOST_ERROR_DEBUG_T 1

#if QMOST_DEBUG_T
        #define QMOST_DEBUG(fmt, args...)   printf("[QMOST_INTERFACE][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define QMOST_DEBUG(fmt, args...)
#endif

#if QMOST_ERROR_DEBUG_T
        #define QMOST_ERROR_DEBUG(fmt, args...)   printf("[QMOST_INTERFACE ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define QMOST_ERROR_DEBUG(fmt, args...)
#endif





static const int MOST_DATA_CUSTOM_EVENT=1002; 

class QCanInterface : public QObject
{
    Q_OBJECT
    
public:
    QCanInterface(QObject* parent = NULL);
    ~QCanInterface();
    
    bool        sendServiceList();
    bool        sendCustomList(QStringList &list);
    bool        set_pos_on_can(int pos, char * info);
    bool        initCAN();
    bool        exitCAN();
    bool        isInitialized(void){ return can_init_flag;};
    uint32_t    getUsageCnt(void);
    static      QByteArray *getData(void)
    {
        QMutexLocker ml(&op_mutex);
        return &data_temp; 
    };
    
private:
    static SerialPort   can_serial;
    static bool         can_init_flag;
    static bool         can_ready_flag;
    static QMutex       op_mutex;
    static QMutex       init_mutex;
    uint8_t             SendCRC;
    static uint32_t     usage_cnt;
    static QByteArray   data_temp;
    static QList<QObject*>        obj_list;
    
    bool                getDeviceStatus();
    static bool                send_ack(void);
    static bool                check_cmd_from_can(QByteArray *data);
    bool                init_can_uart(void);
    bool                wait4ack(void);
    int                 send_to_can_internal(char *data,int size);

private slots:
    static void        can_uart_read_ready(void);
};

class QCanEvent : public QEvent
{
public:
    QCanEvent() : QEvent((QEvent::Type)MOST_DATA_CUSTOM_EVENT) {}
    QString test;
};




#endif	/* QMOSTINTERFACE_H */

