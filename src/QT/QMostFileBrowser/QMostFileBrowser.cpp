/* 
 * File:   QMostFileBrowser.cpp
 * Author: michalm
 * 
 * Created on 20 wrzesień 2013, 12:52
 */

#include "QMostFileBrowser.h"
#include "AS_language.h"
#include "QMediaPlayer.h"
#include "QMostMediaCtrlList.h"

#include <QPushButton>

QFileBrowser::QFileBrowser(QObject* parent,QString base_path) : QObject(parent)  
{
    src_notify_connected = false;  
    browser = NULL;
    browser = new file_browser(this);

//    //printf("base_path:%s\n",base_path.toAscii().data());
//    if(base_path.contains("/media/sd1/",Qt::CaseSensitive)==true)
//    {
//        current_source = SOURCE_SD1;
//        browser->select_source(SOURCE_SD1);
//    }
//    else
//    {
//        if(base_path.contains("/media/usb1/",Qt::CaseSensitive)==true)
//        {
//            current_source = SOURCE_USB1;
//            browser->select_source(SOURCE_USB1);
//        }
//    }
//    
//
//    if(base_path.isEmpty()==false)
//    {
//
//        browser->set_base_path(base_path.toAscii().data());
//    }
//    else
//    {
//
//        browser->select_source(SOURCE_USB1);
//        current_source = SOURCE_USB1;
//    }
    

    setObjectName("QMostFileBrowser");
    
    browser->enable_src_check(true);
  
}



QFileBrowser::~QFileBrowser()
{
    delete browser;
}

void QFileBrowser::setBasePath(const QString &path)
{
    if(browser!=NULL)browser->set_base_path(path);
}
    

bool QFileBrowser::jumpToDir(const QString &dir)
{
    int res = browser->refresh_file_list(dir);
  
    
    if(browser->get_all_items_cnt()>0)
    {

    }
    else
    {

    }
    
    if(res!=0 || browser->get_all_items_cnt()==0) return false;
    else
        return true;
}


/**
 * @brief Funkcja sprawdza czy istnieje plik o podanej nazwie w aktualnym katalogu
 * @param file
 * @return 
 */
int QFileBrowser::checkFile(const QString &file)
{
    QString file_tmp;
    if(browser)
    {   
        int fcnt = browser->get_files_cnt();
        
        if(fcnt)
        {
            for(int i = 0;i<fcnt;i++)
            {
                file_tmp = browser->get_file_name_by_index(i);
                
                if(file_tmp==file) return i;
            }
        }
    }
    return -1;
}

/**
 * @brief Funkcja wywolywana przy wyjeciu nosnika USB
 * @param src_id - id nośnika
 */
void QFileBrowser::onMediaRemove(uint8_t src_id)
{
    emit sigMediaRemove(src_id);
}

/**
 * @brief Funkcja wywolywana przy włożeniu nośnika 
 * @param src_id - identyfikator wlozonego nośnika(źródła)
 */
void QFileBrowser::onMediaInsert(uint8_t src_id)
{
    emit sigMediaInsert(src_id);
}


/**
 * 
 * @param src
 * @return 
 */
bool QFileBrowser::setSource(uint8_t src)
{
    current_source = src;
    browser->select_source(src);
    
    if(browser->get_all_items_cnt()>0)
    {
        
    }
    else
    {

    }
    
//    //jesli przelaczymy sie na dane źródło i nie ma żadnych plików to wyslij "BRAK NOŚNIKA"
//    if(browser->get_all_items_cnt()==0)
//    {
//        AS_Language *language = new AS_Language("#WIN_MEDIA_PLAYER",7);
//        char **Lang = NULL;
//        Lang=language->GetContext();
////        can_interface->set_pos_on_most(1,Lang[0]);
//
//        delete language;
//    }
    
    return true;
}

void QFileBrowser::enableSourceNotify(bool enable)
{
    if(enable==true)
    {
        if(src_notify_connected==false)
        {
            connect(browser,SIGNAL(sigMediaInsert(uint8_t)),this,SLOT(onMediaInsert(uint8_t)));
            connect(browser,SIGNAL(sigMediaRemove(uint8_t)),this,SLOT(onMediaRemove(uint8_t)));
            src_notify_connected = true;
        }
    }
    else
    {
        if(src_notify_connected ==true)
        {
            disconnect(browser,SIGNAL(sigMediaInsert(uint8_t)),this,SLOT(onMediaInsert(uint8_t)));
            disconnect(browser,SIGNAL(sigMediaRemove(uint8_t)),this,SLOT(onMediaRemove(uint8_t)));
            src_notify_connected = false;
        }
    }
}

int QFileBrowser::getCurrentSource(void)
{
    return browser->getCurrentSource();
}

bool QFileBrowser::selectItem(int idx)
{
    browser_item *p = NULL;

    if(browser)
    {
        p = browser->get_item(idx);
        
    }
    else
    {
        return false;
    }
    if(p==NULL) return false;
    
  
    if(p->item_type==0)
    {  
            if(p->item_name=="/..")
            {
                browser->select_item(p->pos_on_list);
                
                if(browser->get_all_items_cnt()>0)
                {

                }
                else
                {

                }
            }
            else
            {
                browser->select_item(p->pos_on_list);

                
                //po każdym wejsciu w katalog nalezy odswiezyc wyliczyc na nowo zmienną win_idx_max!!!
                if(browser->get_all_items_cnt()>0)
                {

                }
                else
                {

                }

            }
            
            emit dir_selected(browser->get_current_path());

         }
        else
        {

        if(p->item_type==1)
        {
          //  printf("#### %s %d  %s   %s#####\n",__FUNCTION__,__LINE__,p->file_name.toAscii().data(),browser->get_current_path().c_str());
            emit file_selected(p->item_name,browser->get_current_path());
        }
    }
    return true;
}
