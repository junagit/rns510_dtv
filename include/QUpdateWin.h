/* 
 * File:   QUpdateWin.h
 * Author: michalm
 *
 * Created on 8 marzec 2013, 14:07
 */

#include "wait_animation.h"


#include <QtGui>


#ifndef QUPDATEWIN_H
#define	QUPDATEWIN_H

class QUpdateWin : public QMainWindow{
    Q_OBJECT
    
public:
    QUpdateWin(QWidget* parent = 0);
//    QUpdateWin(const QUpdateWin& orig);
    ~QUpdateWin();
private:
    QProgressBar                *fileProgress;
    QLabel                      *update_info;
    QLabel                      *update_version;
    bool                        copy_active;
    qint64                      written;
    qint64                      file_size;
    bool                        file_copy(char* src, char* dst);
    bool                        sd1_in_flag,sd1_last_flag;
    void                        check_source_status();
    QTimer                      *srcCheck;
    bool                        card_checked;
    bool                        update_complete;
private slots:
        void             doSigMenuAction(int action);
        void             setValue(qint64 value);
        void            media_timerEvent(void);
        
signals:
        void             sigMenuAction(int action);
        void             sigSetValue(qint64 value);
};

#endif	/* QUPDATEWIN_H */

