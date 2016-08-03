/** 
 * @file   tv_main_menu.h
 * @author michalm
 *
 * @date   30 sierpień 2012, 10:28
 */
#include <QGraphicsEffect>
#include <pthread.h>
#include <QtGui>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QString>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>

#ifndef TV_MAIN_MENU_H
#define	TV_MAIN_MENU_H

class tv_main_menu : public QMainWindow 
{
    Q_OBJECT
    
public:
    tv_main_menu(QWidget* parent = 0);
    ~tv_main_menu();
    void rc_controll(char code);
    tv_main_menu        *current_menu_window;
 //   menu_root_win       *root;
private:
    qreal               win_alpha_value;
    QGraphicsOpacityEffect *win_alpha;  
    void show_main_menu(void);
    
};

#endif	/* TV_MAIN_MENU_H */

