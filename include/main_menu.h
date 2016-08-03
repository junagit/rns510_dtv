/** 
 * @file   main_menu.h
 * @author michalm
 *
 * @date   3 wrzesień 2012, 11:30
 */
#include <QtGui>
#include "QRNS_Buttons.h"

#ifndef MAIN_MENU_H
#define	MAIN_MENU_H

class main_menu  : public QMainWindow {
    Q_OBJECT
public:
        main_menu(QWidget* parent = 0,int to_select = -1);

        ~main_menu();
private:
//        QGraphicsOpacityEffect          *win_alpha;
        QLabel                          *mm_items[4];
        int                             current_item;
//        bool                            show_first_time;
        QRNS_Buttons                    *legned;
//        QLabel                          *legend1_wybor;
//        QLabel                          *legend2_up_down;
//        QLabel                          *legend3_exit;
//        QLabel                          *legend4_back;
        
        void                            highlightIdx(int idx,bool f);
public slots:
        void                            doSigMenuAction(int action);
        void                            doConnect(void)
        {
            connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
        };
        
signals:
        void                            sigMenuAction(int action);
        void startMediaMplayer();

};

#endif	/* MAIN_MENU_H */

