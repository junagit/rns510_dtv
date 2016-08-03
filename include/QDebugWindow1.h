/** 
 * @file   QDebugWindow1.h
 * @author michalm
 *
 * @date   7 listopad 2012, 07:13
 */
#include <QtGui>

#ifndef QDEBUGWINDOW1_H
#define	QDEBUGWINDOW1_H

class QDebugWindow1 : public QMainWindow
{
    Q_OBJECT
public:
    QDebugWindow1(QWidget* parent = 0);
    ~QDebugWindow1();
    
private slots:
        void doSigMenuAction(int action);
        void doUpdate(void);
        
signals:
        void sigMenuAction(int action);
        void update_dbg(void);
        
private:
    QTableWidget *log;
    static void         *update_thrd(void *p);
    pthread_t                  debug;
    bool quit,run;
};

#endif	/* QDEBUGWINDOW1_H */

