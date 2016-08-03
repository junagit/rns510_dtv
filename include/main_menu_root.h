/* 
 * File:   main_menu_root.h
 * Author: michalm
 *
 * Created on 7 kwietnia 2014, 08:07
 */
#include <QtGui>
#include "QRNS_Buttons.h"

#ifndef MAIN_MENU_ROOT_H
#define	MAIN_MENU_ROOT_H

class main_menu_root : public QMainWindow{
    Q_OBJECT
public:
    main_menu_root(QWidget* parent = 0);

    virtual ~main_menu_root();
private:
    QLabel                          *mm_items[2];
    QRNS_Buttons                    *legned;
    void                            highlightIdx(int idx,bool f);
    int                             current_item;
//    QLabel                          *legend1_wybor;
//    QLabel                          *legend2_up_down;
//    QLabel                          *legend3_exit;
//    QLabel                          *legend4_back;
public slots:
        void                        doSigMenuAction(int action);
        
signals:
        void sigMenuAction(int action);
        void startMediaMplayer();
};

#endif	/* MAIN_MENU_ROOT_H */

