/** 
 * @file   wait_animation.h
 * @author michalm
 *
 * @date   5 wrzesień 2012, 12:50
 */
#include <QThread>
#include <QMainWindow>
#include "QProgressIndicator.h"

#ifndef WAIT_ANIMATION_H
#define	WAIT_ANIMATION_H

class wait_animation : public QMainWindow{
    Q_OBJECT
public:
     wait_animation(QWidget* parent = 0);
    ~wait_animation();
    void ShowAnim(bool f);

private:
    QProgressIndicator *pi;
    
    
public slots:
    void doShowAnimation(bool f);
    
signals:
    void sigShowAnimation(bool f);
        
};

#endif	/* WAIT_ANIMATION_H */

