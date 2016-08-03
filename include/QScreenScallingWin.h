/* 
 * File:   QScreenScallingWin.h
 * Author: michalm
 *
 * Created on 10 lipiec 2013, 07:24
 */
#include <QtGui>

#ifndef QSCREENSCALLINGWIN_H
#define	QSCREENSCALLINGWIN_H

class QScreenScallingWin : public QMainWindow{
    
    Q_OBJECT
public:
    QScreenScallingWin(QWidget* parent = 0);
    ~QScreenScallingWin();
private:

        int                             via_fd;
        QLabel                          *adj_buttons[5];
        uint8_t                         current_button;
        uint8_t                         sel_bit;
    
        uint16_t                        via_read(uint8_t reg);
        uint8_t                         via_write(uint8_t reg,uint16_t data);
        void                            via_disconnect(void);
        uint8_t                         via_connect(void);
        
        void                            right_b_cb ();
        void                            left_b_cb ();
        void                            sel_b_cb ();
        void                            down_b_cb ();
        void                            up_b_cb ();
private slots:
        void                            doSigMenuAction(int action);
        
signals:
        void                            sigMenuAction(int action);

};

#endif	/* QSCREENSCALLINGWIN_H */

