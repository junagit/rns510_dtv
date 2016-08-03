/* 
 * File:   QMostFileBrowser.h
 * Author: michalm
 *
 * Created on 20 wrzesień 2013, 12:52
 */
#include <QtGui>

#include "file_browser.h"
#include "QMostInterface.h"
#include "QMediaItem.h"
#include "rc.h"

#ifndef QMOSTFILEBROWSER_H
#define	QMOSTFILEBROWSER_H

class QFileBrowser : public QObject{
    Q_OBJECT
public:
    QFileBrowser(QObject* parent = NULL,QString base_path = "");
    ~QFileBrowser();
    void                setBasePath(const QString &path);    
    browser_item *      get_item(int idx)
    {
        if(browser)
        {
            return browser->get_item(idx);
        }
        
        return NULL;
    };
    
    bool isRoot()
    {
       if(browser)return            browser->isRoot(); 
    };
    
    int                 getAllItemsCnt(void)
    {   
      if(browser)return            browser->get_all_items_cnt();
      
      return 0;
    };
    
    int                 getDirCnt(void)
    {
        if(browser)return            browser->get_dir_cnt();
        
        return 0;
    };
    bool                jumpToDir(const QString &dir);
    int                 checkFile(const QString &file);
    bool                setSource(uint8_t src);
    void                enableSourceNotify(bool enable);
    int                 getCurrentSource(void);
    QString             getCurrentPath(void)
    {
        if(browser!=NULL)
        {
            return browser->get_current_path();
        }
    };
    void                disableCANsignals(bool f);

    bool                selectItem(int idx);
    QString             getCurrentDir(void)
    {
        if(browser!=NULL) return browser->get_current_dir();
    };

    bool                getSourceStatus(uint8_t src_id)
    {
        if(browser!=NULL)
        {
            if(src_id==SOURCE_SD1) return browser->get_sd1_status();
            if(src_id==SOURCE_SD2) return browser->get_sd2_status();
            if(src_id==SOURCE_USB1) return browser->get_usb1_status();
            if(src_id==SOURCE_USB2) return browser->get_usb2_status();
        }
    };
//    void customEvent(QEvent* e)
//    {
//        if(e->type() == (QEvent::Type)1002)
//        {	
//            onCANrecieve(QCanInterface::getData());
//        }
//    };
signals:
    void                file_selected(const QString &file,const QString &dir);
    void                dir_selected(const QString &dir);
    void                sigMediaRemove(uint8_t src_id);
    void                sigMediaInsert(uint8_t src_id);
    void                sigMediaCtrlList(int evt);
    void                return2ctrl(void);
    void                sig_can_rc_code(char code);
    
private:
    
//    bool                connected;
    file_browser        *browser;
//    QCanInterface      *can_interface;
//    QList<QMediaItem*>  *media_list;
//    QStack<int>         window_memory;
//    volatile uint32_t   curr_win_idx;
//    volatile uint32_t   win_idx_max;
    volatile uint32_t   current_source;
    bool                src_notify_connected;
private slots:
//    void                onCANrecieve(QByteArray *data);
    //void                onMediaNormalItemCB(QMediaItem *p);
    void                onMediaRemove(uint8_t src_id);
    void                onMediaInsert(uint8_t src_id);
//    void                OnReturn_timer(void);
};

#endif	/* QMOSTFILEBROWSER_H */

