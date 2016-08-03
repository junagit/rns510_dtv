/* 
 * File:   QRNS_Buttons.h
 * Author: michalm
 *
 * Created on 2 kwietnia 2014, 07:12
 */
#include <QtGui>

#ifndef QRNS_BUTTONS_H
#define	QRNS_BUTTONS_H

class QRNS_Buttons : public QMainWindow{
    Q_OBJECT
public:
    QRNS_Buttons(QWidget* parent = 0,int menu_type = 0);
    QRNS_Buttons(const QRNS_Buttons& orig);
    virtual ~QRNS_Buttons();
private:

    QLabel *l1,*l2,*l3,*l4,*l5,*l6;
};

#endif	/* QRNS_BUTTONS_H */

