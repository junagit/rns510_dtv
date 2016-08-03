/* 
 * File:   QMostInterface.cpp
 * Author: michalm
 * 
 * Created on 20 wrzesień 2013, 09:58
 */
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "QMostInterface.h"
#include "QMediaItem.h"
#include "most_tv_item.h"
#include "service_management.h"

static          uint8_t AsciiToChar[104];
bool            QCanInterface::can_init_flag = false;
bool            QCanInterface::can_ready_flag = false;
QMutex          QCanInterface::op_mutex;
QMutex          QCanInterface::init_mutex;
SerialPort      QCanInterface::can_serial;
uint32_t        QCanInterface::usage_cnt = 0;
QByteArray      QCanInterface::data_temp;
QList<QObject*> QCanInterface::obj_list;

QCanInterface::QCanInterface(QObject* parent) : QObject(parent) 
{
    usage_cnt++;
}

QCanInterface::~QCanInterface() 
{

    usage_cnt--;
    int to_del =-1;
    for(int i = 0;i<obj_list.size();i++)
    {
        if(obj_list.at(i) == this->parent())
        {
            to_del = i;
        }
    }
    
    if(to_del>=0)
    {
        obj_list.removeAt(to_del);
    }

    if(usage_cnt==0) 
    {
        exitCAN();
        obj_list.clear();
    }
}


/**
 * 
 * @return 
 */
bool QCanInterface::initCAN()
{ 
    int retry_cnt = 0;
    
    if(init_mutex.tryLock(300)==false)
    {
        QMOST_DEBUG("mutex locked!!!!!!!!\n");
        return false;
    }
   
    if(can_init_flag==true) 
    {
        obj_list.append(this->parent());
//        printf("QMostInterface: add obj to list %s\n",this->parent()->objectName().toAscii().data());
        init_mutex.unlock();
        return false;
    }

    int i;
    for (i=0;i<10;i++)
            AsciiToChar[48+i]=i;
    for (i=10;i<16;i++)
            AsciiToChar[65+i-10]=i;
    for (i=10;i<16;i++)
            AsciiToChar[97+i-10]=i;
       
      
    if(init_can_uart()==false)
    {
        init_mutex.unlock();
        return false;
    }
    
//    for(i = 0;i<4;i++)
//    {
//      if(getDeviceStatus()==false)
//      {
//          retry_cnt++;
//      }
//      else
//      {
//          most_ready_flag = true;
//      }
//    }
    
    can_ready_flag = true;
    
      if(retry_cnt>=3)
      {
          can_ready_flag = false;
          printf("[MOST] Warning: MOST INTERFACE NOT PRESENT...\n");
      }
    
    init_mutex.unlock();
    return true;
}

/**
 * 
 * @return 
 */
bool QCanInterface::exitCAN()
{
  //  if(most_serial!=NULL) delete most_serial;
    can_serial.close();
    can_init_flag = false;
    data_temp.clear();
    return true;
}


/**
 * 
 * @return 
 */
bool QCanInterface::init_can_uart(void)
{
    if(can_serial.isOpen()==true) return false;

    //============== RS 232 =========================================
    //most_serial = new SerialPort(NULL);
    can_serial.setPort(MOST_UART_PORT);
    QIODevice::OpenMode mode = QIODevice::ReadWrite | QIODevice::Unbuffered ;

    if (can_serial.open(mode)) 
    {
        can_serial.setDataBits(SerialPort::Data8);
        can_serial.setStopBits(SerialPort::OneStop);
        can_serial.setFlowControl(SerialPort::NoFlowControl);
        can_serial.setRate(SerialPort::Rate115200);
        can_serial.setParity(SerialPort::NoParity); 
        can_serial.setReadBufferSize(4);
        
        connect(&can_serial, SIGNAL(readyRead(void)), this, SLOT(can_uart_read_ready(void)),Qt::DirectConnection);
        can_init_flag = true;
        //obj_list = new QList<QObject*>;
        obj_list.clear();
        obj_list.append(this->parent());
//        printf("QMostInterface: add obj to list %s\n",this->parent()->objectName().toAscii().data());

        return true;
    }
    else
    {
        can_init_flag = false;
        QMOST_ERROR_DEBUG("####### BLAD OTWIERANIA PORTU RS232!!!!!!! #######\n");
        return false;
    }
    
}

void QCanInterface::can_uart_read_ready(void)
{
   bool res = false;
   QByteArray data;

    QCoreApplication::flush();

    can_serial.blockSignals(true);
    
    if ((can_serial.bytesAvailable() > 0)) 
     {    
        data = can_serial.readAll();

        if(data.size()>0)
        {
            send_ack();

            res = check_cmd_from_can(&data);

            if(res!=false)
            {
                for(int i = 0;i<obj_list.size();i++)
                {
                    //dodac pomijanie wysylania evetntów do obiektów które mają zablokowane sygnaly - signalsBlocked==true
                 //   printf("send event to %s obj_list.size():%d\n",obj_list.at(i)->objectName().toAscii().data(),obj_list.size());
                    QCanEvent *ev = new QCanEvent();
                    
//                    bool send = obj_list.at(i)->property("block").toBool();
//                    if(send==false)
//                    {
//                        printf("nie wysyłaj do %s\n",obj_list.at(i)->objectName().toAscii().data());
//                    }
//                    ev->test = "hello!!!";
                    
                   //QApplication::postEvent(obj_list.at(i),new QEvent(QEvent::Type(MOST_DATA_CUSTOM_EVENT)),INT_MAX);
                   QApplication::postEvent(obj_list.at(i),ev,INT_MAX);

                }
            }

            can_serial.flush();
            
        }
    }

    can_serial.blockSignals(false);
}

/**
 * @brief Funkcja odczytuje i parsuje komende z MOSTU
 * @param data
 * @return Jezeli crc zgodne to zwraca wskaznika na prametry komendy, jesli zle crc to NULL
 */
bool QCanInterface::check_cmd_from_can(QByteArray *data)
{
    uint8_t SendCRC=0;
    char crc1;
    char crc2;
    int i = 1;
    char res_tab[5];
    
//    printf("data:");
//    if(data->size()==0) return NULL;
//    for(int j = 0;j<data->length();j++)
//    {
//        printf(" 0x%x",data->at(j));
//        
//    }
//    printf("\n");
    if(data->at(0)!=STX) 
    {
        data_temp.clear();
        return false;
    }
    
    while(data->at(i)!=ETB)
    {
        SendCRC=SendCRC+data->at(i);
        i++;
    }
    
    i++;
    SendCRC = (SendCRC ^ 0xFF) + 1;
    
    sprintf(&crc1,"%x",(SendCRC & 0xf0)>>4);
    
    if(crc1!=data->at(i)) 
    {
        data_temp.clear();
        return false;
    }
    
    i++;
    sprintf(&crc2,"%x",(SendCRC & 0x0f));
     
    if(crc2!=data->at(i)) 
    {
        data_temp.clear();
        return false;
    }
    
    res_tab[0] = (AsciiToChar[(int)data->at(1)]<<4) | AsciiToChar[(int)data->at(2)];
    res_tab[1] = (AsciiToChar[(int)data->at(3)]<<4) | AsciiToChar[(int)data->at(4)];
    res_tab[2] = (AsciiToChar[(int)data->at(5)]<<4) | AsciiToChar[(int)data->at(6)];
    res_tab[3] = (AsciiToChar[(int)data->at(7)]<<4) | AsciiToChar[(int)data->at(8)];
    res_tab[4] = (AsciiToChar[(int)data->at(9)]<<4) | AsciiToChar[(int)data->at(10)];
       

    if(res_tab[0]!=0x82 && res_tab[0]!=0x88 && res_tab[0]!=0x89) 
    {
        if(res_tab[1]>0) res_tab[1]--;
    }

    data_temp.clear();
    data_temp.append(QByteArray::fromRawData(res_tab,5));

    return true;
}


int QCanInterface::send_to_can_internal(char *data,int size)
{

    char d;
    QByteArray msg;
    char *data2 = data;
    
    SendCRC=0;
    msg.clear();
    
    if(op_mutex.tryLock(100)==false)
    {
        QMOST_DEBUG("mutex locked!!!!!!!!\n");
        return 1;
    }
    
    if(can_serial.isOpen()==false|| size==0) 
    {
        op_mutex.unlock();
        return 2;
    }

    msg.append((char)STX);

    for (int i=0;i<size;i++)
    {
        if((char)data2[i]<(char)16)
        {
            sprintf((char*)&d,"%x",data2[i]);
            msg.append((char)d);    
            SendCRC= SendCRC+ d;
        }
        else
        {
            msg.append((char)data2[i]);
            SendCRC= SendCRC+ data2[i];
        }
        
    }
    
    msg.append((char)ETB);
    
    SendCRC = ((SendCRC ^ ((uint8_t)0xFF)) + 1);

    sprintf((char*)&d,"%x",((SendCRC >> 4) & 0x0f));
    msg.append(d);


    sprintf((char*)&d,"%x",(SendCRC & 0x0f));

    msg.append(d);
    msg.append((char)ETX);

//    printf("\n");
//    printf("data:");
//    for(int k = 0;k<msg.size();k++)
//    {
//        printf("0x%x ",msg.at(k));
//    }
//    printf("\n");
    can_serial.write(msg);
    op_mutex.unlock();
   
    return 0;
}

/**
 * @brief      Funkcja odbierajaca ack lub nack Z MOST
 * @return 
 */
bool QCanInterface::wait4ack(void)
{
    int i = 20;

    if(can_serial.isOpen()==false) return false;
    
    while(i--)
    {

        if(can_serial.bytesAvailable())
        {
            QByteArray data = can_serial.read(can_serial.bytesAvailable());

            if(data.count()==6 && data.at(0)==STX && data.at(1)==0x06 && data.at(2)==ETB && data.at(3)==0x66 && data.at(4)==0x61 && data.at(5)==ETX) 
            {
                can_serial.flush();
    
                return true;
            }
            else
            {
                can_serial.flush();
                return false;
            }
        }
        usleep(10000);
    }

    return false;
}


bool QCanInterface::send_ack(void)
{
    QByteArray msg;
    int cnt = 20;
    if(can_serial.isOpen()==false || can_ready_flag==false) return false;
    
    msg.append(STX);
    msg.append(0x06);
    msg.append(ETB);
    msg.append(0x46);
    msg.append(0x41);
    msg.append(ETX);
        
    while(1)
    {
        if(op_mutex.tryLock(10)==false)
        {
            cnt--;
            
            if(cnt==0) break;
            
            continue;
        }

//        most_serial->blockSignals(true);
        can_serial.write(msg);
//        most_serial->blockSignals(false);
        
        op_mutex.unlock();
        break;
    }
    
    return true;
}

bool QCanInterface::getDeviceStatus(void)
{
     QByteArray msg;
    
    if(can_serial.isOpen()==false) return false;
    
    msg.clear();
    
    msg.append((char)0);
    msg.append((char)9);

    can_serial.blockSignals(true);
    can_serial.flush();

    send_to_can_internal(msg.data(),msg.size());

   if(wait4ack()==false) 
   {
       QMOST_ERROR_DEBUG("WARNING!!! NO ACK!!!\n"); 
       can_serial.blockSignals(false);
       return false;
   }
   else
   {
       can_serial.blockSignals(false);
       return true;
   }
    
}



uint32_t QCanInterface::getUsageCnt(void)
{
    return usage_cnt;
    
}

bool QCanInterface::sendServiceList()
{
    service_manager    *user_service_list;

    QByteArray          msg;
    QByteArray          bytes;
    int service_cnt;
    char* item_name;
    int cnt = 100;
    int size = 0;
    bool sent = false;
    int res = -1;
    QString tmp;
    
    user_service_list = new service_manager;
    user_service_list->Init_services_data_for_user(false);
    size = service_cnt = user_service_list->get_services_count();
    
    if(service_cnt==0)
    {
        delete user_service_list;
        return false;
    }
    
    service_cnt;
    
    can_serial.blockSignals(true);
    can_serial.flush();
     
    for(int i = 0 ;i<service_cnt;i++)
    {
        
//        if(i<3)
//        {
//            if(i==0)tmp = "97 FF";
//            if(i==1)tmp = "98 --";
//            if(i==2)tmp = "99 FB";
//        }else
//        {
                tmp = user_service_list->DTV_Index_To_Service(i)->getServiceName();
//                printf("sending %s\n",tmp.toAscii().data());
//        }
                msg.append((char)0);
                msg.append((char)1);
                msg.append((char)((i+1)>>4)&0x0F);
                msg.append((char)(i+1)&0x0F);
                msg.append((char)(size>>4)&0x0F);
                msg.append((char)size&0x0F);

                msg.append((char)0);
                msg.append((char)4);
                msg.append((char)5);
                msg.append((char)6);
                msg.append((char)8);
                msg.append((char)9);   

                bytes = tmp.toAscii(); 
                item_name = bytes.data();

                int len = strlen(item_name);

                if(len>24)
                {
                    msg.append(item_name,24);
                }
                else
                {
                    msg.append(item_name);
                }

                while(1)
                {
                    if(sent==false)
                    {
                        res = send_to_can_internal(msg.data(),msg.size());
                        sent = true;
                    }

                    if(res==0)
                    {
                       if(wait4ack()==false) {QMOST_ERROR_DEBUG("WARNING!!! NO ACK!!!\n"); }

                       sent = false;
                       res = -1;
                       break;
                    }

                    if(!cnt--)
                    {
                        QMOST_ERROR_DEBUG("blad wysyslania!!!\n");
                        sent = false;
                        res = false;
                        break;
                    }
                }


                msg.clear();
    }
    
    can_serial.blockSignals(false);
    
    user_service_list->Clean_Loaded_User_Service_Data();
    delete user_service_list;
    return true;
}

/**
 * @brief Fukcja wysyla do MOST-u zaznaczenie aktualnego kanału
 * @param pos
 * @return 
 */
bool QCanInterface::set_pos_on_can(int pos, char * info)
{
    QByteArray msg;
    int res = false;
    bool sent = false;
    int cnt = 10;
    
    if(can_serial.isOpen()==false || can_ready_flag==false) return false;
    
    msg.clear();
//    printf("QMostInterface::set_pos_on_most:%d %s\n",pos,info);
    
     can_serial.blockSignals(true);
     can_serial.flush();
     
    msg.append((char)0);
    msg.append((char)2);
    msg.append((char)((pos+1)>>4)&0x0F);
    msg.append((char)(pos+1)&0x0F);
    
    if(info!=NULL)
    {
        int len = strlen(info);

        if(len>32) msg.append(info,32);
        else
            msg.append(info);
    }
    
    while(1)
    {
        if(sent==false)
        {
            res = send_to_can_internal(msg.data(),msg.size());
            sent = true;
        }
        
        if(res==0)
        {
           if(wait4ack()==false) QMOST_ERROR_DEBUG("WARNING!!! NO ACK!!!\n"); 
           
           sent = false;
           res = -1;
           break;
        }
        
        if(!cnt--)
        {
            QMOST_ERROR_DEBUG("blad wysyslania!!!\n");
            sent = false;
            res = -1;
            break;
        }
    }
    
    can_serial.blockSignals(false);
    
    return res;
}

bool QCanInterface::sendCustomList(QStringList &list)
{
    QByteArray          msg;
    QByteArray          bytes;
    int service_cnt = 0;
    char* item_name;
    int cnt = 100;
    int size = 0;
    bool sent = false;
    int res = -1;
    QString tmp;
    
    service_cnt = size = list.size();
    
    
    can_serial.blockSignals(true);
    can_serial.flush();
     
    for(int i = 0 ;i<service_cnt;i++)
    {
                tmp = list.at(i);
//                printf("sending %s\n",tmp.toAscii().data());
                msg.append((char)0);
                msg.append((char)1);
                msg.append((char)((i+1)>>4)&0x0F);
                msg.append((char)(i+1)&0x0F);
                msg.append((char)(size>>4)&0x0F);
                msg.append((char)size&0x0F);

                msg.append((char)0);
                msg.append((char)4);
                msg.append((char)5);
                msg.append((char)6);
                msg.append((char)8);
                msg.append((char)9);   

                bytes = tmp.toAscii(); 
                item_name = bytes.data();

                int len = strlen(item_name);

                if(len>10)msg.append(item_name,10);
                else
                    msg.append(item_name);


                while(1)
                {
                    if(sent==false)
                    {
                        res = send_to_can_internal(msg.data(),msg.size());
                        sent = true;
                    }

                    if(res==0)
                    {
                       if(wait4ack()==false) {QMOST_ERROR_DEBUG("WARNING!!! NO ACK!!!\n"); }

                       sent = false;
                       res = -1;
                       break;
                    }

                    if(!cnt--)
                    {
                        QMOST_ERROR_DEBUG("blad wysyslania!!!\n");
                        sent = false;
                        res = false;
                        break;
                    }
                }


                msg.clear();
    }
    
    can_serial.blockSignals(false);
    

    return true;
}