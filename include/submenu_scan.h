/** 
 * @file   submenu_scan.h
 * @author michalm
 *
 * @date   3 wrzesień 2012, 13:51
 */
#include <QtGui>
#include "QRNS_Buttons.h"

#ifndef SUBMENU_SCAN_H
#define	SUBMENU_SCAN_H

class submenu_scan : public QMainWindow {
    Q_OBJECT
public:
    submenu_scan(QWidget* parent = 0,int to_select = 0);

    ~submenu_scan();
private:
//        QGraphicsOpacityEffect          *win_alpha;
        QLabel                          *mm_items[4];
        QRNS_Buttons                    *legned;
        int                             current_item;
//        QLabel                          *legend1_wybor;
//        QLabel                          *legend2_up_down;
//        QLabel                          *legend3_exit;
//        QLabel                          *legend4_back;
        bool                            block_once;
        void highlightIdx(int idx,bool f);
private slots:
        void doSigMenuAction(int action);
        
signals:
        void sigMenuAction(int action);

};

#endif	/* SUBMENU_SCAN_H */

