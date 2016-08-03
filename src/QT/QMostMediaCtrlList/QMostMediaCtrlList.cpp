/* 
 * File:   QMostMediaCtrlList.cpp
 * Author: michalm
 * 
 * Created on 24 wrzesień 2013, 12:29
 */

#include "QMostMediaCtrlList.h"
#include "QMediaPlayer.h"
#include "AS_language.h"

QMostMediaCtrlList::QMostMediaCtrlList(QObject* parent) : QObject(parent) 
{
    media_ctrl_list = new QList<QMediaItem*>;
    
    //tworzymy listę sterującą początkową
    createCtrlList();
    setObjectName("QMostMediaCtrlList");
    can_interface = new QCanInterface(this);
    
    can_interface->initCAN();
    
   // connect(most_interface, SIGNAL(mostDataRecieved(QByteArray *)), this, SLOT(onMOSTrecieve(QByteArray *)),Qt::DirectConnection);
    connected = true;
}




QMostMediaCtrlList::~QMostMediaCtrlList() 
{
    media_ctrl_list->clear();
    
    delete media_ctrl_list;
    delete can_interface;
}

void QMostMediaCtrlList::onMOSTrecieve(QByteArray *data)
{
        if(data==NULL) return;
        
        if(connected==false) return;
    
    switch(data->at(0))
    {
        case 0x81:
            selectMediaItem(data->at(1),true);
            break;
            
            
        case 0x86:
//           emit sendList();
            emit sigMediaCtrlList(EVT_SEND_LIST);
           break;
           
        case 0x82:
            if(data->at(1)==MEDIA_STOP)
            {//otrzymany komunikat otrzymywany przy zmianie źródła mowiący nam że mamy przestać odtwarzać 

                  emit sigMediaCtrlList(EVT_MEDIA_STOP);
            }
            else
            {//wznów odtważanie
                emit sigMediaCtrlList(EVT_MEDIA_RESUME);
            }
            break;
            
            default:
            printf("### onMOSTrecieve:NIEZNANA KOMENDA...\n");
            break;
    }
}

bool QMostMediaCtrlList::sendMediaCtrlList(void)
{
    if(connected==false) 
    {
        return false;
    }

    createCtrlList();
//    can_interface->send_to_most(media_ctrl_list);   
//    can_interface->set_pos_on_most(4,QMediaPlayer::getCurrentFileName().toAscii().data());
    return true;
}


void QMostMediaCtrlList::addMediaItemCtrl(QString name, QMediaItem::media_item_type type, uint32_t id, const QObject *receiver, const char *member)
{
    QMediaItem *new_item = new QMediaItem(this);
    
    if(new_item==NULL) 
    {
        printf("BLAD ALOKACJI PAMIECI!!!\n");
        return;
    }
    
    new_item->file_name = name;
    new_item->id = id;
    new_item->type = type;
    

  //  printf("dodajemy do listy: %s  id:%d  media_list.size:%d\n",name.toUtf8().data(),new_item->id,media_list.size());
    //podlaczamy sygnal tylko dla typow kontrolnych i stacji
    if(!(receiver==NULL || member==NULL))
    {
        if(type!=QMediaItem::EMPTY_ITEM) connect(new_item,SIGNAL(sigMediaItemSelected(QMediaItem *)),receiver,member,Qt::DirectConnection);
    }
    media_ctrl_list->append(new_item);
}

void QMostMediaCtrlList::createCtrlList(void)
{
    media_ctrl_list->clear();
    file_browser *browser = new file_browser(this);

    
    AS_Language *language = new AS_Language("#WIN_MEDIA_PLAYER",7);
    char **Lang = NULL;
    Lang=language->GetContext();
    
    addMediaItemCtrl("TV", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    addMediaItemCtrl("   ", QMediaItem::EMPTY_ITEM,0, NULL, NULL);
    
    QString tmp = QMediaPlayer::getCurrentFileDir();
    
    if(tmp.contains("sd1",Qt::CaseInsensitive)==true) current_media_source = 0;
    else
        current_media_source = 2;
    
    
//    if(current_media_source==2)//pobierane ze zmiennej statycznej z Qmediaplayer
//    {
//        addMediaItemCtrl("[USB]", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//        addMediaItemCtrl("SD", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    }
//    else
//    {
//        addMediaItemCtrl("USB", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//        addMediaItemCtrl("[SD]", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    }
    
    if(current_media_source==SOURCE_SD1)
        {
            if(browser->get_usb1_status()==true)
            addMediaItemCtrl("USB", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
            else
            addMediaItemCtrl("No USB", QMediaItem::CTRL_ITEM,0,NULL,NULL);
            
            if(browser->get_sd1_status()==true)
            addMediaItemCtrl("[SD]", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
            else
            addMediaItemCtrl("No SD", QMediaItem::CTRL_ITEM,0,NULL,NULL);
        }
        else
        {
            if(browser->get_usb1_status()==true)
            addMediaItemCtrl("[USB]", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
            else
            addMediaItemCtrl("No USB", QMediaItem::CTRL_ITEM,0,NULL,NULL);
            
            if(browser->get_sd1_status()==true)
            addMediaItemCtrl("SD", QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
            else
            addMediaItemCtrl("No SD", QMediaItem::CTRL_ITEM,0,NULL,NULL);
        }
    
    
//    addMediaItemCtrl("   ", QMediaItem::EMPTY_ITEM,0, NULL, NULL);
    
    //CZY PAUZA
  
//    if(QMediaPlayer::isPaused()==true)//pobierane ze zmiennej statycznej QMediaPlayer
//    {
//        addMediaItemCtrl(QString::fromUtf8(Lang[6]), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    }//CZY PLAY
//    else
//    {
//        addMediaItemCtrl(QString::fromUtf8(Lang[2]), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    }
    

    addMediaItemCtrl(QString::fromUtf8("<<--"), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    addMediaItemCtrl("   ", QMediaItem::EMPTY_ITEM,0, NULL, NULL);
    if(QMediaPlayer::isPaused()==true)//pobierane ze zmiennej statycznej QMediaPlayer
    {
        addMediaItemCtrl(QString::fromUtf8(Lang[6]), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
    }//CZY PLAY
    else
    {
        addMediaItemCtrl(QString::fromUtf8(Lang[2]), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
    }
    addMediaItemCtrl(QString::fromUtf8("-->>"), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    addMediaItemCtrl("   ", QMediaItem::EMPTY_ITEM,0, NULL, NULL);
    addMediaItemCtrl(QString::fromUtf8(Lang[1]), QMediaItem::CTRL_ITEM,0,this, SLOT(onMediaCtrlItemCB(QMediaItem *)));
//    addMediaItemCtrl("   ", QMediaItem::EMPTY_ITEM,0, NULL, NULL);
    
    delete language;
    delete browser;
}

/**
 * @brief Fukcja wywoływana przy wyborze elemntu z lity MOST
 * @param p - wksaźnik na wybrany element z listy
 */
void QMostMediaCtrlList::onMediaCtrlItemCB(QMediaItem *p)
{
    if(p==NULL) return;

    if(p->file_name=="TV")
    {
//        can_interface->set_pos_on_most(0," ");
        can_interface->mostFlush();
        
        emit sigMediaCtrlList(EVT_TV);
        return;
    }
 
    AS_Language *language = new AS_Language("#WIN_MEDIA_PLAYER",7);
    char **Lang = NULL;
    Lang=language->GetContext();

    
if(p->file_name == QString::fromUtf8(Lang[6]))
    {
        emit sigMediaCtrlList(EVT_PLAY);
        printf("#### WYBRALEM PLAY ####\n");
        
//        can_interface->set_pos_on_most(4,QMediaPlayer::getCurrentFileName().toAscii().data());
        delete language;
        return;
    }
    
    if(p->file_name == QString::fromUtf8(Lang[2]))
    {
        emit sigMediaCtrlList(EVT_PAUSE);
//        can_interface->set_pos_on_most(4,QMediaPlayer::getCurrentFileName().toAscii().data());
        delete language;
        return;
    }
    
    
   
    
    //if(p->file_name == QString::fromUtf8(Lang[4]))
    if(p->file_name == QString::fromUtf8("-->>"))
    {
        emit sigMediaCtrlList(EVT_NEXT);
//        can_interface->set_pos_on_most(4,QMediaPlayer::getCurrentFileName().toAscii().data());
        delete language;
        return;
    }
    
    //if(p->file_name == QString::fromUtf8(Lang[5]))
     if(p->file_name == QString::fromUtf8("<<--"))
    {
        emit sigMediaCtrlList(EVT_PREV);
//        can_interface->set_pos_on_most(4,QMediaPlayer::getCurrentFileName().toAscii().data());
        delete language;
        return;
    }
    
    if(p->file_name == "USB")
    {
        emit sigMediaCtrlList(EVT_USB);
        delete language;
        return;
    }
    
    if(p->file_name == QString::fromUtf8(Lang[1]))
    {
        emit sigMediaCtrlList(EVT_VIEW);
        delete language;
        return;
    }
    
    if(p->file_name == "[USB]") 
    {
        emit sigMediaCtrlList(EVT_USB_C);
        delete language;
        return;
    }
 
    if(p->file_name == "SD") 
    {
        emit sigMediaCtrlList(EVT_SD);
        delete language;
        return;
    }
    
    if(p->file_name == "[SD]") 
    {
        emit sigMediaCtrlList(EVT_SD_C);
        delete language;
        return;
    }
//    
    delete language;
    
}

/**
 * @brief Funkcja zaznacza obiekt na liście jednoczesnie wysyla sygnal do Obiektu ktory zainstalowal slot na ten sygnal 
 * @param idx - pozycja na liscie do zaznaczenia
 * @param emit_signal - jesli true to zostanie wyemitowany sygnal ze wybralismy element z listy,jesli false to tylko go podswietli
 */
void QMostMediaCtrlList::selectMediaItem(int32_t idx,bool emit_signal)
{
    QMediaItem *item = NULL;

    if(idx>=media_ctrl_list->size()) return;
    
    item = media_ctrl_list->at(idx);
            
    if(item!=NULL)
    {
        QMediaItem::media_item_type type = item->type;
        //nie podswietlamy pustych pozycji
        if(type==QMediaItem::EMPTY_ITEM) return;
            
        if(emit_signal) item->selectItem(QMediaItem::SELECTED);
    }
}

/**
 * @brief Funkcja blokuje otrymywanie sygnalów od klasy QMostInterface żeby klasa nie reagowała na wybór elementu z listy
 * @param f
 */
void QMostMediaCtrlList::disableMOSTsignals(bool f)
{
    if(f==false)
    {
        if(connected==true)
        {
        connected = false;
    //    disconnect(most_interface,SIGNAL(mostDataRecieved(QByteArray *)), this, SLOT(onMOSTrecieve(QByteArray *)));
        }
    }
    else
    {
        if(connected==false)
        {
                connected = true;
      //          connect(most_interface, SIGNAL(mostDataRecieved(QByteArray *)), this, SLOT(onMOSTrecieve(QByteArray *)),Qt::QueuedConnection);
        }
    }

}