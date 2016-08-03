/** 
 * @file   play_ctrl.h
 * @author michalm
 *
 * @date   8 sierpień 2012, 07:27
 */
#include <QtGui>
#include <QSettings>
#include "type.h"
#include "uart_io.h"
#include "dtv_core.h"
#include "tv_bar.h"
#include "volume_bar.h"
#include "tv_main_menu.h"
#include "rc.h"

#include "main_menu.h"
#include "QMessageWindow.h"
#include "serialport.h"
#include <QCopChannel>
#include "QUserServiceList.h"
#include "app_settings.h"
#include "QMoreInfoWindow.h"
#include "QChannelSwitchThrd.h"
#include "QInitThread.h"

#include "QMostTvList.h"

#include "dvbtcontrol.h"
#include "gpio_ctrl.h"
#include "QMostMediaPlayer.h"
#include "QTvList.h"

#define BAD_SIGNAL_TIMEOUT      14
#define MAX_BER_LEVEL           200000
#define TIMER_DELAY             250 //ms

#define PLAY_CTRL_DEBUG_T       1
#define PLAY_CTRL_ERROR_DEBUG_T 1

#if PLAY_CTRL_DEBUG_T
        #define PLAY_CTRL_DEBUG(fmt, args...)   printf("[PLAY_CTRL][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define DTV_CORE_DEBUG(fmt, args...)
#endif

#if PLAY_CTRL_ERROR_DEBUG_T
        #define PLAY_CTRL_ERROR_DEBUG(fmt, args...)   printf("[PLAY_CTRL ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define DTV_CORE_ERROR_DEBUG(fmt, args...)
#endif

#ifndef PLAY_CTRL_H
#define	PLAY_CTRL_H


enum wdt_ctrl{
    WDT_OPEN,
    WDT_CLOSE,
    WDT_REFRESH
    
};


class play_ctrl : public QMainWindow{
    friend class QOtherOptWindow;
    friend class QChannelSwitchThrd;
    friend class QInitThread;
    friend class QAutoScanWindow;
    friend class main_menu;
    friend class main_menu_root;
    friend class manual_scan;
    Q_OBJECT
public:
    play_ctrl(QWidget* parent = 0);
    ~play_ctrl();
    
    bool        getMainMenuFlag()
    {
        return main_menu_show_hide;
    };
    
    void        app_quit(void)
    {
                
                if(main_menu_show_hide==true || epg_win_show_hide==true || more_is_showed==true)
                {
                    parse_rc_cmd(MENU);
                }
                usleep(200000);
                parse_rc_cmd(_EXIT_);
    };
    
    dtv         *get_dtv_p(void){ return tv_player; };
  
    static int         get_current_ch_idx()
    {
      return current_ch_index;  
    };
    
    static int menu_root;
    
    char        *getCurrentServiceInfo();
    
    void        StartServiceSelect(int from_idx);
    void        UpdateChannelsNumber(bool need_change);
    
    void stop_sys_timer()
    {
        if(debug_timer!=NULL)debug_timer->stop();
    };
    
    void start_sys_timer()
    {
        int i = 30;
        if(debug_timer!=NULL)
        {
            while(debug_timer->isActive()==false)   
            {
                debug_timer->start(250);
                i--;
                if(!i) break;
            }
        }
    }

    bool isSelecting() const {
        return ch_sw->isRunning();
    };
    QByteArray getDigitsFromTime(int firstTime, int secondTime);
    QByteArray getTimeRangeAndStationTitle();
    volatile bool       most_scan;
    volatile bool       manual_scan;
    volatile bool       need_send_list;
signals:
    void sigMenuAction(int action);
    void SigSystemInfo(bool f,int type,QString msg);
    void ChSwitchFromMOST(int ch);
    void ChSwitchFromMOST_next_prev(bool up_down);
    void epgFromMOST(bool on_off);
    void scanFromMOST(bool on_off);
    void sigTvOnOff(bool state);
    void sendList(void);
    void winDestroy(QObject *p);
    void startMediaMplayer();
    void menuEnable(bool en);
public slots:
    void do_sig_can_rc_code(char rc_code);
    
private slots:
    void domenuEnable(bool en){
        if(en)
        {
                can_rc4tv->disableCANsignals(false);
                do_sig_can_rc_code(MENU);
        
        }else
        {
                can_rc4tv->disableCANsignals(true);
                do_sig_can_rc_code(MENU);
        }
    };
    void doWinDestroy(QObject *p);
    void doSigQuitFromMore(void);
    void procReadyRead();
    void DoChannelSwitch();
    void doSigTvOnOff(bool state);
    void doStartMediaMplayer(void);
    void doSigSystemInfo(bool f,int type,QString msg);
    void DoChannelSwitchFromMOST(int ch);
    void DoDEBUG();

    //void received_from_most ( const QString & message, const QByteArray & data );
    //############################################################################
    void doMainBarShowTimeOut(void)
    {
        doSigHideMainBar();
    };
    
    void  doSigHideMainBar(void);
    void  doSigHideVolumeWindow(void);
    //############################################################################
    void doVolumeShowTimeOut(void)
    {
        doSigHideVolumeWindow();
    };
    //############################################################################
    void doEpgFromMOST(bool on_off);
    void doScanFromMOST(bool on_off);
    void doChSwitchFromMOST_next_prev(bool up_down);
    
private:
    wait_animation      *anim;
    volatile bool       different_channel;
    QTvList             *tv_list;
    QSplashScreen       *splash;
    app_settings        *settings;
    QString             ch_idx_tmp;
    dtv                 *tv_player;
    tv_bar              *main_bar;
    volume_bar          *volume_window;
    QMessageWindow      *system_info;
    SerialPort          *rs_232;
    QTimer              *ch_sw_timeout;
    QTimer              *debug_timer;
    QTimer              *volume_timer;                    
    QTimer              *main_bar_timer; 

    QCANrcTV            *can_rc4tv;
    QMediaPlayerMain    *media_player;

    QInitThread         *init_thrd;
    QChannelSwitchThrd  *ch_sw;
    
    QLabel              *mute_icon;
    QMoreInfoWindow     *more;
    
    volatile int        ch_switch_timeout;
    volatile bool       more_is_showed;
    volatile bool       epg_is_actual;
    volatile bool       tv_init_ok;
    volatile bool       epg_win_show_hide;
    volatile bool       main_menu_show_hide;
    volatile bool       mute;
    volatile bool       wait4constructor;
    volatile bool       quick_list_show;
    volatile bool       no_signal;
    volatile bool       system_info_show_flag;//flaga oznaczajaca wyswietlenie okna informacyjnego
    volatile bool       freq_was_locked;
    volatile bool       selecting_from_most;
    volatile uint32_t   signal_level;
    volatile uint32_t   signal_ber;
    volatile bool       need_change;
    static int          current_ch_index;
    volatile int        current_ch_index_tmp;
#ifdef VOLUME_CTRL
    volatile int        current_volume;
#else
    double              current_volume;
#endif
    volatile int        total_channles_num;
    volatile int        bad_signal_cnt;
    volatile bool       was_crashed;
    volatile bool       stop_tv_from_most;
    volatile bool       info_was_found;
    volatile bool       quick_list_show_flag;
    int                 watchdog_fd;
    volatile bool       opt_win_flag;
    //####################################################################
    bool                parse_rc_cmd(char code);
    void                update_bar_info(void);
    void                doSigShowMainBar(void);
    void                doSigShowVolumeWindow(int val);
    
    bool         signal_is_bad()
    { 
        QMutexLocker ml(&mMutex);
        return no_signal; 
    };
    void MOST_TV_STOP_PLAY();
    void MOST_TV_RESUME_PLAY();
    
    void doSigCreateMainMenu(void);    
    
    void HandleTvSignal(void);
    void showMute(bool f);
    char* rcCode2digit(char rc_code);
    void watchdog_ctrl(wdt_ctrl cmd);
    void show_more_info(bool f);
    
    QMutex mMutex;
    
    protected:
        void paintEvent(QPaintEvent *event);
    
};


#endif	/* PLAY_CTRL_H */

