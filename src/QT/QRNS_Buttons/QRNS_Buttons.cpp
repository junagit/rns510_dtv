/* 
 * File:   QRNS_Buttons.cpp
 * Author: michalm
 * 
 * Created on 2 kwietnia 2014, 07:12
 */

#include "QRNS_Buttons.h"

QRNS_Buttons::QRNS_Buttons(QWidget* parent,int menu_type) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);   
    setGeometry((1024-150),0,150,768);
 
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
                
    switch(menu_type)
    {
        case 0:
        {
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("UP");
            
            l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("DOWN");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("----");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("RETURN");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("OK");
        }
            break;
            
            
        case 1:
            {
                
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("UP");
            
           l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("DOWN");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("---");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("USB/SD");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("OK\nPAUSE");
        }
            break;
            
            
        case 2:
                    {
//            setObjectName("audio_win");
            
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("UP");
            
           l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("DOWN");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("----");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("RETURN");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("OK/SEL");
        }
            break;
            
            case 3:
            {
//            setObjectName("audio_win");
            
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("UP");
            
           l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("DOWN");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("----");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("RETURN");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("SCAN\nADD");
        }
        break;
        
           case 4:
            {
//            setObjectName("audio_win");
            
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("UP");
            
           l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("DOWN");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("----");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("RETURN");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("OK");
        }
        break;
        
        case 5:
            {
//            setObjectName("audio_win");
            
            l1 = new QLabel(this);
            l1->setObjectName("simple_label");
            l1->setGeometry(0,0,150,126);
            l1->setAlignment(Qt::AlignCenter);
            l1->setText("TV");
            
            l2 = new QLabel(this);
            l2->setObjectName("simple_label");
            l2->setGeometry(0,126,150,126);
            l2->setAlignment(Qt::AlignCenter);
            l2->setText("FF");
            
           l3 = new QLabel(this);
            l3->setObjectName("simple_label");
            l3->setGeometry(0,(252),150,126);
            l3->setAlignment(Qt::AlignCenter);
            l3->setText("FB");
            
            l4 = new QLabel(this);
            l4->setObjectName("simple_label");
            l4->setGeometry(0,(378),150,126);
            l4->setAlignment(Qt::AlignCenter);
            l4->setText("----");
            
            l5 = new QLabel(this);
            l5->setObjectName("simple_label");
            l5->setGeometry(0,(504),150,126);
            l5->setAlignment(Qt::AlignCenter);
            l5->setText("USB/SD\nFF/FB");
            
            l6 = new QLabel(this);
            l6->setObjectName("simple_label");
            l6->setGeometry(0,(630),150,126);
            l6->setAlignment(Qt::AlignCenter);
            l6->setText("OK");
        }
        break;
    }
    
    QWidget *horizontalLineWidget2 = new QWidget(this);
    horizontalLineWidget2->setGeometry(0,0,7,768);
    horizontalLineWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget2->setStyleSheet(QString("background-color: grey;"));
    
    show();
    l1->setFont(QFont( "Arial", 30, QFont::Normal));
    l2->setFont(QFont( "Arial", 30, QFont::Normal));
    l3->setFont(QFont( "Arial", 30, QFont::Normal));
    l4->setFont(QFont( "Arial", 30, QFont::Normal));
    l5->setFont(QFont( "Arial", 30, QFont::Normal));
    l6->setFont(QFont( "Arial", 30, QFont::Normal));
    
    
    
}

QRNS_Buttons::QRNS_Buttons(const QRNS_Buttons& orig) {
}

QRNS_Buttons::~QRNS_Buttons() {
}

