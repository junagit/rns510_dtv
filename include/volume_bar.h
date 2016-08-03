/** 
 * @file   volume_bar.h
 * @author MIchał Motyl
 *
 * @date   10 sierpień 2012, 07:10
 */
#include <QGraphicsEffect>
#include <pthread.h>
#include <QtGui>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QString>
#include <sstream>
#include <stdint.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
using namespace std;

#include "overlay.h"

#define DEFAULT_VOLUME_DELAY   5*1000000

#ifndef VOLUME_BAR_H
#define	VOLUME_BAR_H

class volume_bar : public QMainWindow 
{
    Q_OBJECT
public:
    volume_bar(QWidget* parent = 0);
    ~volume_bar();
    

    void        set_volume(int val)
    {
        ostringstream ss;
        //if(b->value()!=val)//odświerzmay tylko gdy wartosc jest zmieniona
        {
            ss << val;
//            setUpdatesEnabled(false);
            b->setValue(val);
            val_txt->setText((QString)ss.str().c_str());
//                setUpdatesEnabled(true);
        }
    };
   // qreal       get_win_alpha() { return win_alpha_value;};
//    void        set_win_alpha(qreal v);

private:

    QProgressBar        *b;
    QLabel              *val_txt;
//    qreal               win_alpha_value;
};

#endif	/* NEWCLASS_H */

