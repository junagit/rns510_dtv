/** 
 * @file   QDateTimeWindow.h
 * @author michalm
 *
 * @date   11 wrzesień 2012, 07:48
 */
#include <QtGui>

#ifndef QDATETIMEWINDOW_H
#define	QDATETIMEWINDOW_H

class QDateTimeWindow  : public QMainWindow{
    Q_OBJECT
public:
    QDateTimeWindow(QWidget* parent = 0);
    ~QDateTimeWindow();
private:
        QGraphicsOpacityEffect      *win_alpha;

        
private slots:
        void doSigMenuAction(int action);
        
signals:
        void sigMenuAction(int action);
};

#endif	/* QDATETIMEWINDOW_H */

