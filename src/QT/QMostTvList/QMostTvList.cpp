/* 
 * File:   QMostTvList.cpp
 * Author: michalm
 * 
 * Created on 24 wrzesień 2013, 10:06
 */

#include <iostream>
#include <set>
#include <algorithm>

#include "QMostTvList.h"
#include "AS_language.h"
#include "app_settings.h"
#include "play_ctrl.h"

bool QCANrcTV::menu_from_can = false;
volatile bool menu_flag = false;
//int dir_cnt = 0;
extern void via_set_scalling(bool flag);

//ogolne sterowanie tv - przelaczanie kanalow
QCANrcTV::QCANrcTV(QObject* parent) : QObject(parent) 
{
    current_item_tmp = -1;
    current_item = -1;
    last_ch_idx = -1;

    setObjectName("QMostTvList");

    can_interface = new QCanInterface(this);
    can_interface->initCAN();
    connected = false;
    
    sw_timer.setInterval(1200);
    sw_timer.setSingleShot(true);
    connect(&sw_timer, SIGNAL(timeout()), SLOT(DoChannelSwitch()));
    sw_timer.stop();
    
    setProperty("block",false);
    
//    sortList();
}



QCANrcTV::~QCANrcTV() 
{
    delete can_interface;
}

void QCANrcTV::customEvent(QEvent* e)
{
    if(e==NULL) return;

    if(e->type() == (QEvent::Type)1002)
    {
//        QCanEvent *myEvent = static_cast<QCanEvent *>(e);
//      printf("test:%s\n",myEvent->test.toAscii().data());
        onCanRecieve(QCanInterface::getData());
    }
};

void QCANrcTV::onCanRecieve(QByteArray *data)
{
    if(data==NULL) return;
    
    if(data->size()==0) return;
    
    if(connected==false) return;
  
    
    switch(data->at(0))
    {
        case 0x81://zmiana kanalu po czasie 1.2s
         if(menu_flag==false)
         {
//            printf("QMediaPlayerMain::onCanRecieve 0x%x  %d\n",data->at(0),data->at(1));
            sw_timer.stop();
            sw_timer.start();
            last_ch_idx = data->at(1);
         }
        break;
        
        case 0x82:
        {
         emit sigTvOnOff((bool)data->at(1));

        }
            break;
        
        case 0x8b:
            emit sigTvOnOff(!bool(data->at(1)));
            break;
            
        case 0x88:
            if(data->at(1)==0x02)emit epgFromMOST(true);
            break;
        
        case 0x89:
      
            if(data->at(1)==0x01 && menu_flag==false)
            {
                via_set_scalling(true);
                menu_flag = true;
                menu_from_can = true;
                emit sig_can_rc_code(MENU);
            }
            else
            {
                if(data->at(1)==0x00 && menu_flag==true) 
                {
                    via_set_scalling(false);
                    menu_flag = false;
                    menu_from_can = false;
                    emit sig_can_rc_code(MENU);
                    break;
                }
                
                if(data->at(1)==0x03) emit sig_can_rc_code(UP);
                
                if(data->at(1)==0x04) emit sig_can_rc_code(DOWN);
                
                if(data->at(1)==0x06) emit sig_can_rc_code(OK);
                
                if(data->at(1)==0x05) 
                {
                    if(play_ctrl::menu_root==1)return;
                    
                    emit sig_can_rc_code(_EXIT_);
                }
            }
            break;
        default:

            break;
    }
    

}



/**
 * @brief Funkcja zwraca indeks aktualnego elementu(zanaczonego) na liscie
 * @return Zwraca -1 jesli nie zaznaczony żaden lub >=0 jesli cos jest zaznaczone
 */
int32_t QCANrcTV::getCurrentItemIdx()
{
  return current_item;  
};

void print(const std::string& item)
{
    std::cout << item << std::endl;
}


//int strcicmp(char const *a, char const *b)
//{
//    for (;; a++, b++) {
//        int d = tolower(*a) - tolower(*b);
//        if (d != 0 || !*a)
//            return d;
//    }
//}

void QCANrcTV::sortList(void)
{
//    std::set<std::string> sortedItems;
//    service_manager service_management;
//    service_management.Init_services_data_for_user(false);//załaduj listę ze stacjami
//    int service_cnt_tmp = service_management.get_services_count();
//    int j = 0,k = 0,n = 0;
//    
//    std::vector<QString> OptList(service_cnt_tmp);
//    QString temp;
//    
//    for(int i = 0;i<service_cnt_tmp;i++) OptList[i] = service_management.DTV_Index_To_Service(i)->getServiceName() ;
//        
//    for(int i = 0;i<service_cnt_tmp;i++)OptList[i] = OptList[i].toLower();
//
//    	for (j=1;j<service_cnt_tmp;j++)
//	{
//            for(k=0;k<j;k++)
//            {
//                n=j-k;
//                
//                //if(strcicmp(OptList[n].toAscii().data(),OptList[n-1].toAscii().data())<0)
//                    if(OptList[n].toAscii().data()[0]<OptList[n-1].toAscii().data()[0])
////                if(OptList[n].compare(OptList[n-1],Qt::CaseInsensitive)<0)
//                {
//                        temp = OptList[n];
//                        OptList[n] = OptList[n-1];
//                        OptList[n-1] = temp;
//                }
//                else
//                {
//                        break;
//                }
//            }
//	}
//    
//    
//    for(int i = 0;i<service_cnt_tmp;i++)
//    {
//        printf("sorted:%s\n",OptList[i].toAscii().data());
//    }
//  //  for(int i = 0;i<service_cnt_tmp;i++)sortedItems.insert(service_management.DTV_Index_To_Service(i)->getServiceName());
//    
////    std::for_each(OptList.begin(), OptList.end(), &print);
    
    
}

/**
 * @brief Funkcja blokuje otrymywanie sygnalów od klasy QMostInterface żeby klasa nie reagowała na wybór elementu z listy
 * @param f
 */
void QCANrcTV::disableCANsignals(bool f)
{
    if(f==true)
    {
          connected = false;
    }
    else
    {

          connected = true;

    }
    
//    printf("##### QMostTvList::disableMOSTsignals:  connected:%d\n",connected);s
}

void QCANrcTV::DoChannelSwitch(void)
{
    emit ChSwitchFromMOST(last_ch_idx);
}