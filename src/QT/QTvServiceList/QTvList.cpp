/* 
 * File:   QTvList.cpp
 * Author: michalm
 * 
 * Created on 4 luty 2014, 12:24
 */

#include "QTvList.h"
#include "rc.h"
#include "service_management.h"
#include "app_settings.h"
#include "play_ctrl.h"
#include "AS_language.h"

QTvList::QTvList(QWidget* parent) : QMainWindow(parent)   
{
     can_interface=NULL;
    setObjectName("QTvList");
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::NoFocus);
    setGeometry(0,0,1024,768);
    
    background = new QPixmap(":/img/tv_list.png");

    connected = false;
    saved_channels = 0;
    
    setMask(background->mask());
    
    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(*background));
    setPalette(palette);       
    
    AS_Language *language = new AS_Language("#WIN_INFO",8);
    char **Lang = NULL;
    Lang=language->GetContext();
        
    info = new QLabel(this);
    info->setObjectName("simple_label");
    info->setText("INFO");
    info->setGeometry(10,13,200,45);
    info->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    
    QLabel *media = new QLabel(this);
    media->setObjectName("simple_label");
    media->setText("USB/SD");
    media->setGeometry(10,768-58,200,45);
    media->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    
    QLabel *sound = new QLabel(this);
    sound->setObjectName("simple_label");
    //sound->setText("DŹWIĘK");
    sound->setText(Lang[7]);
    sound->setGeometry(1024-210,768-58,200,45);
    sound->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    
    QLabel *source = new QLabel(this);
    source->setObjectName("simple_label");
//    source->setText("ŹRÓDŁO");
    source->setText(Lang[6]);
    source->setGeometry(1024-210,13,200,45);
    source->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    
    QLabel *header = new QLabel(this);
    header->setStyleSheet("border: none; color:yellow; background: none;");
    header->setText("TV DVB-T");
    header->setGeometry((1024/2)-125,20,250,45);
    header->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    
    arrow_up = new QLabel(this);
    arrow_up->setObjectName("simple_label");
    arrow_up->setText("\u25B2");
    arrow_up->setGeometry(110,88,40,40);
    arrow_up->hide();
    
    arrow_down = new QLabel(this);
    arrow_down->setObjectName("simple_label");
    arrow_down->setText("\u25BC");
    arrow_down->setGeometry(110,768-128,40,40);
    arrow_down->hide();
    
    current_item = 0;
    curr_win_idx = 0;
    win_idx_max = 0;
    was_highlighted = false;
    
    hide_timer = new QTimer(this);
    hide_timer->setInterval(6000);
    hide_timer->setSingleShot(true);
    hide_timer->stop();
    connect(hide_timer, SIGNAL(timeout()), SLOT(doTimer(void)),Qt::QueuedConnection);
    
    image_highlight[0] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or1.xpm");
    image_highlight[1] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or2.xpm");
    image_highlight[2] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or3.xpm");
    image_highlight[3] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or4.xpm");
    image_highlight[4] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or5.xpm");
    image_highlight[5] = new QPixmap(":/img/Labelki_wyboru/just/Lista_5a_wyb_or6.xpm");
    
    image_curr[0] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b1.xpm");
    image_curr[1] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b2.xpm");
    image_curr[2] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b3.xpm");
    image_curr[3] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b4.xpm");
    image_curr[4] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b5.xpm");
    image_curr[5] = new QPixmap(":/img/Labelki_wyboru/Lista_5a_wyb_b6.xpm");
    
    show();
    
    int y = 132;
    for(int i = 0;i<6;i++) 
    {
        items[i] = NULL;
        items[i] = new QLabel1(this);
        items[i]->setObjectName("simple_label");
        items[i]->setGeometry(50,y,922,84);
        items[i]->do_alpha = true;
        items[i]->show();
        items[i]->setFont(QFont( "Arial", 55, QFont::Normal));
        y+=85;
    }
    
    arrow_up->setFont(QFont( "Arial", 50, QFont::Normal));
    arrow_down->setFont(QFont( "Arial", 50, QFont::Normal));
    
    info->setFont(QFont( "Arial", 45, QFont::Normal));
    media->setFont(QFont( "Arial", 45, QFont::Normal));
    source->setFont(QFont( "Arial", 45, QFont::Normal));
    sound->setFont(QFont( "Arial", 45, QFont::Normal));
    header->setFont(QFont( "Arial", 50, QFont::Normal));
    
//    can_interface = new QCanInterface(this);
//    can_interface->initCAN();
    
    delete language;
}
    
       

QTvList::~QTvList() 
{
    for(int i = 0;i<6;i++) 
    {
        delete image_highlight[i];
        delete image_curr[i];
    }
    
    delete background;
}

bool QTvList::moveUp(void)
{
//    QTime myTimer;
//    myTimer.start();
    
    if(isVisible()==false || saved_channels==0) return false;

    
    hide_timer->stop();
    hide_timer->start();
    
    if(current_item==0) return false;

    highlightIdx(current_item%ITEMS_TO_SHOW,false);
    
    --current_item;

    if(((current_item+1)%ITEMS_TO_SHOW)==0)
    {
        if(curr_win_idx>0)
        {
            //items[selected_item%ITEMS_TO_SHOW]->setStyleSheet("border: none; color: white; background: none;");
            curr_win_idx--;
            showListPortion();
            was_highlighted = false;
        }
    }
   
    if(selected_item==current_item)
    {
        was_highlighted = false;
        items[current_item%ITEMS_TO_SHOW]->setPixmapHighlightState(image_highlight[current_item%ITEMS_TO_SHOW]);
    }
       
    highlightIdx(current_item%ITEMS_TO_SHOW,true);
    
    if(selected_item==current_item) was_highlighted = false;
    
    if(selected_item!=current_item)highlightCurrentService();
    
//    printf("@@@@@@@ %s %d %d ms\n",__FUNCTION__,__LINE__,myTimer.elapsed());
    return true;
 }


 bool QTvList::moveDown(void)
{
//     QTime myTimer;
//    myTimer.start();
    
     if(isVisible()==false || saved_channels==0) return false;
     
    hide_timer->stop();
    hide_timer->start();

    if(current_item<(list.size()-1)==false) return false;

    highlightIdx(current_item%ITEMS_TO_SHOW,false);

    current_item++;

    int temp = current_item%ITEMS_TO_SHOW;
    
    if(selected_item==current_item) was_highlighted = false;
    
    if(temp==0)
    {
        curr_win_idx++;
        showListPortion();
        was_highlighted = false;
        highlightCurrentService();
    }
    
    if(selected_item==current_item)
    {
        was_highlighted = false;
        items[temp]->setPixmapHighlightState(image_highlight[temp]);
    }
          
    highlightIdx(temp,true);
    
    if(selected_item!=current_item)highlightCurrentService();

//    printf("@@@@@@@ %s %d %d ms\n",__FUNCTION__,__LINE__,myTimer.elapsed());
    return true;
};
    
    
bool QTvList::showList(void)
{
//    QTime myTimer;
//    myTimer.start();
    int service_cnt = 0;
    int type; 
    list.clear();
    service_manager service_management;
    service_management.Init_services_data_for_user(false);//załaduj listę ze stacjami
    saved_channels = service_cnt = service_management.get_services_count();
    
    was_highlighted = false;
    connected = true;
    
    if(can_interface==NULL)
    {
        can_interface = new QCanInterface(this);
        can_interface->initCAN();
    }
    
    if(saved_channels==0)
    {
        AS_Language *language = new AS_Language("#WIN_INFO",8);
        char **Lang = NULL;
        Lang=language->GetContext();
            
        addItem(Lang[1],type);
        info->setText("SCAN");
        showListPortion();
        hide_timer->start();
        delete language;
        return true;
    }
    else
    {
        info->setText("INFO");
    }
    
//   app_settings app_set;
   
    
    if(service_cnt==0)return false;
    else
    {
        for(int i = 0;i<service_cnt;i++)
        {
            //addItem(QString::number(i+1) + "." + service_management.DTV_Index_To_Service(i)->getServiceName(),0);
            addItem(service_management.DTV_Index_To_Service(i)->getServiceName(),type);
        }
        
         //incjalizacja zmiennych win_idx_max,curr_win_idx
        if(list.size()>=ITEMS_TO_SHOW)
         {
             win_idx_max = (uint32_t)(list.size()/ITEMS_TO_SHOW);

             int tmp = (int)(list.size()%ITEMS_TO_SHOW);

             if(tmp>0) win_idx_max++;
             
             arrow_down->show();
         }
         else
         {
             win_idx_max = 0;
             curr_win_idx = 0;
         }
        
//        selected_item = current_item = app_set.get_channel_idx();
        selected_item = current_item = play_ctrl::get_current_ch_idx();
    }
    
    curr_win_idx = (uint32_t)(selected_item/ITEMS_TO_SHOW);
    showListPortion();
    hide_timer->start();
    
    highlightIdx((selected_item%ITEMS_TO_SHOW),true);
//    printf("@@@@@@@ %s %d %d ms\n",__FUNCTION__,__LINE__,myTimer.elapsed());
    return true;
};

    
void QTvList::doSigMenuAction(int action)
{
    if(isVisible()==false) return;
    
    hide_timer->stop();
    hide_timer->start();
    
//    printf("@@@@@@@ %s %d action :%d \n",__FUNCTION__,__LINE__,action);
    
                switch(action)
		  {
                    case OK:
                    {
                        if(saved_channels==0) return;
                        //wyczysc poprzednie zaznaczenie
                   //     items[selected_item%ITEMS_TO_SHOW]->setStyleSheet("border: none; color: white; background: none;");
                        items[selected_item%ITEMS_TO_SHOW]->setState(false);
                        selected_item = current_item;
                        
                        
                       hide_timer->stop();
                        doTimer();   
                        emit ChSwitchFromMOST(current_item); 
                    }
		    break;
                                     
                    case INFO:
                           connected = false;
                           delete can_interface;
                           can_interface = NULL;
                           emit epgFromMOST(true);
                           hide_timer->stop();
                        break;
                        
                    case 0x56:
                        printf("###### URUCHOM MEDIA PLAYER #####\n");
                        connected = false;
                        delete can_interface;
                        can_interface = NULL;
                        emit startMediaMplayer();
                        hide_timer->stop();
                        break;
               
                    case 0x55:
                        moveDown();
                        break;

                    case 0x54:
                        moveUp();
                        break;
                        
                    default:
                    break;
	}
    
}

void QTvList::doTimer(void)
{
    connected = false;
    delete can_interface;
    can_interface = NULL;
    
    emit winDestroy(this);
}

void QTvList::addItem(const QString &item_name,int &type)
{
    QTvItem *temp = new QTvItem();
    temp->name = item_name;
    list.append(temp);
}


bool QTvList::showListPortion()
{
    int temp_add = 0;
    
    for(int i = 0;i<ITEMS_TO_SHOW;i++) 
    {
        items[i]->clear();
        items[i]->setPixmapHighlightState(image_highlight[i]);
    }
    
    if(curr_win_idx==0)
    {
        if(win_idx_max>1)
        {//jesli mamy wiecej niz jedno "okienko"
 
                if(list.size()>ITEMS_TO_SHOW)temp_add = ITEMS_TO_SHOW;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    QTvItem *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)
                    {
                        items[j]->setText("     " + item_temp->name);
                    }
                }
        }
        else
        {//jesli miescimy sie w okienku, tzn. plikow jest<=6
            if(list.size()>0)
            {
                if(list.size()>=ITEMS_TO_SHOW)temp_add = ITEMS_TO_SHOW;
                else
                temp_add = list.size();
                        
                for(int j = 0;j<temp_add;j++)
                {
                    QTvItem *item_temp = list.at(j);
                    
                    if(item_temp!=NULL)items[j]->setText("     " + item_temp->name);
                    
                }
            }
//            else
//            {
//                printf("BRAK PLIKOW W KATALOGU???\n");   
//            }
        }
        
    }
    else
    {
        if(curr_win_idx==(win_idx_max-1))
        {//czy ostatnie okienko

            int c = ((ITEMS_TO_SHOW*curr_win_idx));
            
            if(list.size()%ITEMS_TO_SHOW==0)
            {
                for(int j = c,i = 0;j<(c+ITEMS_TO_SHOW);j++,i++)
                {
                    QTvItem *item_temp = list.at(j);
                        
                    if(item_temp!=NULL)items[i]->setText("     " + item_temp->name);   
                }
            }
            else
            {
                for(int j = c,i = 0;j<(c+((list.size()%ITEMS_TO_SHOW)));j++,i++)
                {
                    QTvItem *item_temp = list.at(j);

                    if(item_temp!=NULL) items[i]->setText("     " + item_temp->name);     
                }
            }
        }
        else
        {
            int c = (ITEMS_TO_SHOW*curr_win_idx);
            
            for(int j = c,i = 0;j<(c+ITEMS_TO_SHOW);j++,i++)
            {
                QTvItem *item_temp = list.at(j);
                    
                if(item_temp!=NULL)items[i]->setText("     " + item_temp->name);
            }
        }
    }

    //sterowanie wyswietlaniem strzałek
//    printf("curr_win_idx:%d  win_idx_max%d\n",curr_win_idx,win_idx_max);
    if(curr_win_idx>0 && curr_win_idx<(win_idx_max-1))
    {
        if(!arrow_up->isVisible())arrow_up->show();
        if(!arrow_down->isVisible())arrow_down->show();
    }
    else
    {
        if(curr_win_idx==0)
        {
                arrow_up->hide();
                if(!arrow_down->isVisible())arrow_down->show();
        }
        else
        {
            if(curr_win_idx==(win_idx_max-1))
            {
                if(!arrow_up->isVisible())arrow_up->show();
                arrow_down->hide();
            }
        }
    }
    
    for(int i = 0;i<ITEMS_TO_SHOW;i++) items[i]->update();
    
 return true;
   
}

void QTvList::highlightIdx(const int &idx,bool f)
{
    items[idx]->setState(f);
    return;
}

void QTvList::highlightCurrentService(void)
{
    if(was_highlighted) return;
    
    int win_id_temp  = (uint32_t)(selected_item/ITEMS_TO_SHOW);

    if(curr_win_idx==(uint32_t)win_id_temp)
    {
        int idx = selected_item%ITEMS_TO_SHOW;
        
        if(idx!=current_item)
        {
            items[idx]->setPixmapHighlightState(image_curr[idx]);
            items[idx]->setState(true);
        }
        
        was_highlighted = true;
    }
    
}

void QTvList::onCanRecieve(QByteArray *data)
{
    if(connected==false) return;
     
//    printf("@@@ %s %d connected:%d@@@\n",__FUNCTION__,__LINE__,connected);
    printf("QTvList::onCanRecieve:0x%x\n",data->at(0));
     switch(data->at(0))
    {
        
       case 0x85:
       case 0x88:
           //return
           doTimer();
            break;
            
        case 0x86:
            //NAST UTWOR
            //NASTEPNY UTWOR Z LISTY jesli jest wyslana do most'u lista kontrolujaca odtwarzacz
           break;
           
        case 0x87:
            //POPRZEDNI UTWOR
           break;
           
            break;
            
       case 0x83:
            if(saved_channels>0)doSigMenuAction(INFO);
            else
            {
                hide_timer->stop();
                doTimer();
                emit scanFromMOST(true);
            }
            break;
            
        case 0x84:
            doSigMenuAction(0x56);
        break;

        case 0x82:
        doSigMenuAction(OK);
        break;
//###################################################################
        case 0x80://galka w lewo
        moveDown();
        break;

        case 0x81:

        moveUp();
        break;
//###################################################################            
        default:

            break;
     }
}