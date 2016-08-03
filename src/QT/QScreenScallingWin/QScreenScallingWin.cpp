/* 
 * File:   QScreenScallingWin.cpp
 * Author: michalm
 * 
 * Created on 10 lipiec 2013, 07:24
 */
#include <QtGui>

#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>

#include "rc.h"
#include "main_menu.h"
#include "QScreenScallingWin.h"
#include "i2c-dev.h"
#include "utils.h"
#include "app_settings.h"
#include "sslLcdcApi.h"
//#define DEBUG 1
#ifdef DEBUG
#define dbg(arg...) printf(arg)
#else
#define dbg(arg...) 
#endif

#define VIA_SCALLING_DATA_FILE  "/media/localdata/scalling.dat"
#define VIA_I2C_ADDR            0x20
#define I2C_DEVICE_FILE         "/dev/i2c-0"
extern ssl_panel_t lcdc_AT070TN93;


#define MAX_LCDC_H 576
#define MIN_LCDC_H 450

int h_diff = 0;
int panel_h_temp;

QScreenScallingWin::QScreenScallingWin(QWidget* parent) : QMainWindow(parent)
{
    app_settings settings;
    setWindowFlags(Qt::FramelessWindowHint);   
    setFocusPolicy(Qt::NoFocus);
    setObjectName("scalling_window");
    
    sel_bit = 1;
    //musimy ustawic okno jako nieprzeźroczyste - białe tło
    QPalette transparentPallete;
    transparentPallete.setColor(QPalette::Window, QColor(255, 255, 255, 255));
    //transparentPallete.setBrush(QPalette::Background,*(new QBrush(*(new QPixmap(QString::fromUtf8(":/img/scr_adj.png"))))));
    setPalette(transparentPallete);
    
    adj_buttons[1] = new QLabel(this);
    adj_buttons[1]->setGeometry(452,(332),120,80);
    adj_buttons[1]->setText("sdsdd");
    adj_buttons[1]->setObjectName("simple_label");
    adj_buttons[1]->setAlignment(Qt::AlignCenter);
    adj_buttons[1]->setPixmap(QPixmap(":/img/Przesuniecie.xpm"));
 //   adj_buttons[1]->image (*new xpmImage(Przesuniecie_xpm));

    adj_buttons[0] = new QLabel(this);
    adj_buttons[0]->setGeometry(472,(adj_buttons[1]->y()-160),80,80);
    adj_buttons[0]->setText("\u25b3");
    adj_buttons[0]->setObjectName("simple_label");
    adj_buttons[0]->setAlignment(Qt::AlignCenter);


    adj_buttons[2] = new QLabel(this);
    adj_buttons[2]->setGeometry(472,(adj_buttons[1]->y()+160),80,80);
    adj_buttons[2]->setText("\u25bd");
    adj_buttons[2]->setObjectName("simple_label");
    adj_buttons[2]->setAlignment(Qt::AlignCenter);
  //  adj_buttons[2]->image (*new xpmImage(mainmenu_arrowdown_xpm));



    adj_buttons[3] = new QLabel(this);
    adj_buttons[3]->setGeometry((adj_buttons[1]->x()-160),adj_buttons[1]->y(),80,80);
    adj_buttons[3]->setText("\u25c1");
    adj_buttons[3]->setObjectName("simple_label");
    adj_buttons[3]->setAlignment(Qt::AlignCenter);
 //   adj_buttons[3]->image (*new xpmImage(mainmenu_arrow_left_xpm));


    adj_buttons[4] = new QLabel(this);
    adj_buttons[4]->setGeometry((adj_buttons[1]->x()+180),adj_buttons[1]->y(),80,80);
    adj_buttons[4]->setText("\u25b7");
    adj_buttons[4]->setObjectName("simple_label");
    adj_buttons[4]->setAlignment(Qt::AlignCenter);
 //   adj_buttons[4]->image (*new xpmImage(mainmenu_arrow_right_xpm));
    
    show();
    
    adj_buttons[0]->setFont(QFont( "Arial", 50, QFont::Normal));
    adj_buttons[1]->setFont(QFont( "Arial", 50, QFont::Normal));
    adj_buttons[2]->setFont(QFont( "Arial", 50, QFont::Normal));
    adj_buttons[3]->setFont(QFont( "Arial", 50, QFont::Normal));
    adj_buttons[4]->setFont(QFont( "Arial", 50, QFont::Normal));
    
    setGeometry(0,0,1024,768);
    via_fd=-1;
    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    via_connect();
    
    volatile uint32_t via_data = settings.getViaSettings();
    
    if(via_data!=0)
    {
        h_diff = ((via_data & 0x0000ff00)>>8);
        panel_h_temp = 576 - h_diff;
     //   printf("panel_h_temp:%d\n",panel_h_temp);
    }
    else
    {
        h_diff = 0;
        lcdc_AT070TN93.panel_height = 576;
        panel_h_temp = 576;
    }
  //  printf("panel_h_temp:%d\n",panel_h_temp);
    
}



QScreenScallingWin::~QScreenScallingWin() 
{
    via_disconnect();
}


void QScreenScallingWin::doSigMenuAction(int action)
{
    volatile FILE *fd = NULL;
    app_settings *settings;
    unsigned char tmp;
                switch(action)
		  {

		  case OK:
                      

                      sel_b_cb();
		    break;


                    case 0x56:
		 case DOWN:
                     
                     down_b_cb ();
                     break;
                     
                 case INFO: 
                 case UP:
                     up_b_cb ();
                                          
		   break;
                   
                    case LEFT:
                        left_b_cb ();
                        break;
                    
                        case RIGHT:
                        right_b_cb ();
                        break;

                case MENU:
                {
                    settings = new app_settings();
                    uint32_t toSave = 0;
      
                    sync();
                   
                          tmp = via_read (0x08);
                          toSave |= (tmp<<24); 

                          tmp = via_read (0x09);
                          toSave |= (tmp<<16);
                                  
                       //   tmp = via_read (0x5e);
                          h_diff = 576 - panel_h_temp;
                          toSave |= (h_diff<<8);

                          tmp = via_read (0x5f);
                          toSave |= tmp;
                     //     printf("toSave = 0x%x\n",toSave);
                          settings->saveViaSettings(toSave);
                      
                           
                    delete settings;
                    sync();
                    delete this;
                }
                break;
                
                case _EXIT_:
                {
                    disconnect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
                    settings = new app_settings();
                    uint32_t toSave = 0;
                 //   printf("getViasettings:0x%x\n",settings->getViaSettings());
                    //usuń stary plik           
                    sync();
                    

                          tmp = via_read (0x08);
                          toSave |= (tmp<<24); 

                          tmp = via_read (0x09);
                          toSave |= (tmp<<16);
                                  
                       //   tmp = via_read (0x5e);
                          h_diff = 576 - panel_h_temp;
                          toSave |= (h_diff<<8);

                          tmp = via_read (0x5f);
                          toSave |= tmp;
                     //     printf("toSave = 0x%x\n",toSave);
                          settings->saveViaSettings(toSave);
                      
                           
                    delete settings;
                    sync();
                    main_menu *m = new main_menu(parentWidget(),2);   
                    delete this;
                }
                break;
                
		default:
		break;
	}
}


uint8_t QScreenScallingWin::via_connect(void)
{
    //via_fd = open(I2C_DEVICE_FILE,O_RDWR);
    via_fd = open("/dev/vt1622a",O_RDWR);
    

   if (via_fd > 0)
      {
//	    if (ioctl(via_fd, I2C_SLAVE,VIA_I2C_ADDR) < 0)
//	    {
//		printf("error:can't set slave address\n");
//		return 1;
//	    }
      }

  return 0;//ok
}


void QScreenScallingWin::via_disconnect(void)
{
    ::close(via_fd);
}


uint8_t QScreenScallingWin::via_write(uint8_t reg,uint16_t data)
{
    char buff[2];
    
  if(!via_fd) return 0;
  
    buff[0] = reg;
    buff[1] = (char)data;
    
    
    return write(via_fd,buff,2);

}

uint16_t QScreenScallingWin::via_read(uint8_t reg)
{
    char tx[10];
    if(!via_fd) return 0;

    tx[0] = reg;
    
    read(via_fd,tx,1);
    return tx[0];
    //return i2c_smbus_read_byte_data (via_fd,reg);
}

void QScreenScallingWin::up_b_cb ()
{
  QScreenScallingWin *p = this;

    if(p->sel_bit==1)
	    {
		unsigned short svp = via_read (0x09);

		dbg("SVP:%d\n",svp);

		if(svp<80){
                    svp++;
                    via_write (0x09,(svp & 0x00ff));

                }
	
	    }
	  else
	    {
                if(panel_h_temp>400)
                {
                    panel_h_temp-=2;
                    lcdc_AT070TN93.panel_height=panel_h_temp;
                    sslLcdcDisplayOutputSet(&lcdc_AT070TN93);
                }
	    }

}



void QScreenScallingWin::down_b_cb ()
{
  QScreenScallingWin *p = this;

    if(p->sel_bit==1)
	    {
		unsigned short svp = via_read (0x09);

		//dbg("SVP:%d\n",svp);

		if(svp>10){
                    svp--;

		via_write (0x09,(svp & 0x00ff));
                }
	    }
	  else
	    {
                if(panel_h_temp<576)
                {
                    panel_h_temp+=2;
                    lcdc_AT070TN93.panel_height=panel_h_temp;
                    sslLcdcDisplayOutputSet(&lcdc_AT070TN93);
                }
            }
}

void QScreenScallingWin::sel_b_cb ()
{
  QScreenScallingWin *p = (this);
  
    TOGGLE_BIT(p->sel_bit,0);
    dbg("sel_bit:%d\n",p->sel_bit);

    if(p->sel_bit==0)
      {

        adj_buttons[1]->setPixmap(QPixmap(":/img/Skala.xpm"));
      }
    else
      {
        adj_buttons[1]->setPixmap(QPixmap(":/img/Przesuniecie.xpm"));
      }

}

void QScreenScallingWin::left_b_cb ()
{
  QScreenScallingWin *p = this;
  //dbg("##### LEFT\n");
  
      if(p->sel_bit)
	    {
		unsigned short shp = via_read (0x08);

		//dbg("SHP:%d\n",shp);

                if(shp>0)shp--;

		if(shp>35){
		//printf("SHP:%d\n",shp);

		via_write (0x08,(shp & 0x00ff));

                }

	    }
	  else
	    {
		unsigned char hsp = via_read (0x5f);

		//dbg("HSP:%d\n",hsp);
                
                
                if(hsp<160){
                    hsp++;
		//printf("HSP:%d\n",hsp);
		via_write (0x5f,(hsp & 0x00ff));
                }

	    }
show();
//  update();

}

void QScreenScallingWin::right_b_cb ()
{
  QScreenScallingWin *p = this;
  //dbg("##### RIGHT\n");

      if(p->sel_bit)
	    {
		unsigned short shp = via_read (0x08);

		//printf("SHP:%d\n",shp);

                if(shp<180)
                {
                        shp++;
                        via_write (0x08,(shp & 0x00ff));
                }

	    }
	  else
	    {
		unsigned char hsp = via_read (0x5f);

		//dbg("HSP:%d\n",hsp);

                
                        
		if(hsp>0){
		//printf("HSP:%d\n",hsp);
                hsp--;
		via_write (0x5f,(hsp & 0x00ff));

                }
	
	    }
  
show();
}
