/** 
 * @file   QMessageWindow.h
 * @author michalm
 *
 * @date   7 wrzesień 2012, 07:43
 */
#include <stdio.h>
#include <string.h>
#include <QtGui>

#ifndef QMESSAGEWINDOW_H
#define	QMESSAGEWINDOW_H

class QMessageWindow  : public QMainWindow{
    Q_OBJECT
    
public:
    QMessageWindow(QWidget* parent = 0,int type = 0,const QString &msg = " ");
    ~QMessageWindow();
private:
    QLabel *info;
    QLabel *img;
};

#endif	/* QMESSAGEWINDOW_H */

