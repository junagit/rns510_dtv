/*
 * File:   main.cpp
 * Author: Michał Motyl
 *
 * Created on 1 sierpień 2012, 14:35
 */
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <exception_defines.h>
#include <sys/prctl.h>
#include <QtGui/QApplication>
#include <QFile>
#include <QWSServer>
#include <sys/types.h>
#include <sys/wait.h>

#include "overlay.h"
#include "uart_io.h"
#include "play_ctrl.h"
#include <QTextCodec>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
//#include "MemTrack.h"
#include "i2c-dev.h"
#include "sslLcdcApi.h"

extern ssl_panel_t lcdc_AT070TN93;

play_ctrl *main_app = NULL;
QApplication *app = NULL;
int DoServer(int argc, char *argv[]);

void myMessageOutput(QtMsgType type, const char *msg)
 {
     //in this function, you can write the message to any stream!
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }

//skalowanie stałe dla RNS 510
void via_set_scalling(bool flag)
{
//    app_settings settings;
    volatile uint32_t via_data;
    int via_fd = 0;
    char wr_buff[2];
    int h_diff = 0;

    via_fd = open("/dev/vt1622a",O_RDWR);
    
    if (via_fd > 0)
      {
            if(flag)
            {
                //parametry skalowania do wejscia w epg/txt
                //via_data = 1764622336;
                via_data = 1764625408;
            }
            else
//            via_data = settings.getViaSettings();
            via_data=1779080192;
            
            if(via_data!=0)
            {
//                printf("getViasettings:0x%x\n",via_data);

                wr_buff[0] = 0x08;
                wr_buff[1] = ((via_data & 0xff000000)>>24);
                write(via_fd,wr_buff,2);
                
                wr_buff[0] = 0x09;
                wr_buff[1] = ((via_data & 0x00ff0000)>>16);
                write(via_fd,wr_buff,2);
                
                h_diff = ((via_data & 0x0000ff00)>>8);
                lcdc_AT070TN93.panel_height= 576 - h_diff;
                //lcdc_AT070TN93.panel_width = 725;
               
                sslLcdcDisplayOutputSet(&lcdc_AT070TN93);
                
                wr_buff[0] = 0x5f;
                wr_buff[1] = (via_data & 0x000000ff);
                write(via_fd,wr_buff,2);
                
            }
            close(via_fd);
      }
    
}

//using namespace MemTrack;

//void spinner(int spin_seconds) {
//    static char const spin_chars[] = "/-\\|";
//    unsigned long i, num_iterations = (spin_seconds * 10);
//    for (i=0; i<num_iterations; ++i) {
//        putchar(spin_chars[i % sizeof(spin_chars)]);
//        fflush(stdout);
//        usleep(100000);
//        putchar('\b');
//    }
//}

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    
    int result = fork();
    
    if(result == 0)DoServer(argc,argv);
    
    if(result < 0)exit(1);
    
    for(;;)
    {
        int status = 0;
        waitpid(-1, &status, 0);
        if(!WIFEXITED(status))
        {
            result = fork();
                   
            if(result == 0)
                DoServer(argc,argv);
            if(result < 0)
            {
                puts("uh... crashed and cannot restart");
                exit(1);
            }
        }
        else exit(0);
    }
    return 0;
}

int DoServer(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Images);
    int res;

    char name [17];	/* Name must be <= 16 characters + a null */
    strcpy (name,"main_app_thrd");
    prctl (PR_SET_NAME, (unsigned long)&name);

    printf("###### URUCHOMIENIE PROGRAMU ######\n");
    
    via_set_scalling(false);
    
//    map_fw_regs();
//    fw1_enable();
    
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    qInstallMsgHandler(myMessageOutput);
    
    app = new QApplication(argc, argv,QApplication::GuiServer);
//    app->setQuitOnLastWindowClosed(false);

    
    //linie poniżej ustawiają przeźroczyste okno
    QPalette transparentPallete;
    transparentPallete.setColor(QPalette::Window, QColor(0, 0, 0, 0));
    app->setPalette(transparentPallete);

    QWSServer::setBackground(QBrush(QColor(0, 0, 0, 0)));
    QWSServer::setCursorVisible( false );
    
    QFile qss(":/img/styl.css");
    qss.open(QFile::ReadOnly);
    app->setStyleSheet(qss.readAll());
    qss.close();

    
    main_app = new play_ctrl(NULL);

    main_app->show();

    
    res = app->exec();

    delete main_app;
    delete app;
//    fw1_disable();
//    unmap_lcdc_reg();

//    TrackListMemoryUsage();
//    TrackFree();
    return res;
}