/* 
 * File:   QMenuInfoWindow.h
 * Author: michalm
 *
 * Created on 22 styczeń 2014, 07:09
 */
#include <QtGui>

#ifndef QMENUINFOWINDOW_H
#define	QMENUINFOWINDOW_H

class QMenuInfoWindow : public QMainWindow {
    Q_OBJECT
public:
    QMenuInfoWindow(QWidget* parent = 0);
    ~QMenuInfoWindow();
    
private slots:
        void doSigMenuAction(int action);
private:
    QLabel info;

};

#endif	/* QMENUINFOWINDOW_H */

