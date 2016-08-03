/** 
 * @file   QOtherOptWindow.cpp
 * @author michalm
 *
 * @date   10 wrzesień 2012, 11:45
 */
#include <QtGui>
#include "rc.h"
#include "QOtherOptWindow.h"
#include "QMenuInfoWindow.h"
#include "dtv_core.h"
#include "play_ctrl.h"
#include "QmenuTvItem.h"
#include "app_settings.h"
#include "AS_language.h"

volatile bool need_update_volume = false;

volatile bool selected_for_change = false;
QOtherOptWindow::QOtherOptWindow(QWidget* parent,int to_select) : QMainWindow(parent)
{
    AS_Language *language = new AS_Language("#MENU_OTHERS",5);
    char **Lang = NULL;
    Lang=language->GetContext();
    
    setWindowFlags(Qt::FramelessWindowHint);    
    setGeometry(100,96,720,576);
//    setObjectName("audio_win_grey");
    setFocusPolicy(Qt::NoFocus);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0,0));
    setPalette(pal);
    
    current_item = 0;
    tv_record_location = 0;
    antena_power = 0;
    global_alpha = 0;
    osd_time = 0;
    
    application_settings = new app_settings();
    osd_time = application_settings->get_osd_time();
 
    
    for(int i = 0;i<MAX_TV_OPT;i++) menu_items[i] = NULL;
    //===========================================================================
    //czas chowania się paska glosnosci i glownej belki tv
    menu_items[0] = new QmenuTvItem(this,QString::fromUtf8(Lang[0]));
    menu_items[0]->setPosition(20,20);
    menu_items[0]->addItem(QString::fromUtf8(Lang[1]));
    menu_items[0]->addItem("2 s");
    menu_items[0]->addItem("3 s");
    menu_items[0]->addItem("4 s");
    menu_items[0]->addItem("5 s");
    menu_items[0]->addItem("6 s");
    menu_items[0]->addItem("7 s");
    menu_items[0]->addItem("8 s");
    //===========================================================================
    //OPCJA GŁOŚNOŚCI SYSTEMOWEJ
    menu_items[1] = new QmenuTvItem(this,QString::fromUtf8(Lang[2]));
    menu_items[1]->setPosition(20,90+30);
    menu_items[1]->addItem("1");
    menu_items[1]->addItem("2");
    menu_items[1]->addItem("3");
    menu_items[1]->addItem("4");
    menu_items[1]->addItem("5");
    menu_items[1]->addItem("6");
    menu_items[1]->addItem("7");
    menu_items[1]->addItem("8");
    menu_items[1]->addItem("9");
    menu_items[1]->addItem("10");
    //ważne::switchItem powoduje wywołanie się callback'a uzytkownika dlatego w konstruktorze 
    //musimy wywołać metodę switchItem przed podaniem callbaca żeby nie został on wywołany

    
    if((application_settings->getSystemVolume()*14)>9)
    {
        menu_items[1]->switchItem(1);
        application_settings->saveSystemVolume(0.07);
    }
    else
    menu_items[1]->switchItem((application_settings->getSystemVolume()*14));
    
    menu_items[1]->setUserCB(&sys_vol_cb,this);
    
    
    menu_items[2] = new QmenuTvItem(this,QString::fromUtf8(Lang[3]));
    menu_items[2]->setPosition(20,160+60);
    //menu_items[2]->addItem("POLSKI");
    //menu_items[2]->addItem("ENGLISH");
    for (int i = 0; i < language->getLanguages().size(); i++)
    {
        QString temp = language->getLanguageFileWithoutExt(i);
        menu_items[2]->addItem(temp.toUpper());
    }
    //ważne::switchItem powoduje wywołanie się callback'a uzytkownika dlatego w konstruktorze 
    //musimy wywołać metodę switchItem przed podaniem callbaca żeby nie został on wywołany
    if(application_settings->getSystemLanguage() > language->getLanguages().size())
    {
        QString lang = language->getDefaultLanguage();
        lang += language->getDefaultLangExt();
        menu_items[2]->switchItem(language->getLanguageId(lang.toLower()));
        application_settings->saveSystemLanguage(language->getLanguageId(lang));
    }
    else
    {
        menu_items[2]->switchItem(application_settings->getSystemLanguage());
    }
    
    menu_items[2]->setUserCB(&language_cb,this);
    
    //ważne::switchItem powoduje wywołanie się callback'a uzytkownika dlatego w konstruktorze 
    //musimy wywołać metodę switchItem przed podaniem callbaca żeby nie został on wywołany

    menu_items[3] = new QmenuTvItem(this,QString::fromUtf8(Lang[4]));
    menu_items[3]->setPosition(20,(160+70+90));
    //menu_items[3]->addItem("\u25B6");
    menu_items[3]->addItem("   ");
    menu_items[3]->setUserCB(&sys_info_cb,this);
    
    if(osd_time==1)menu_items[0]->switchItem(0);
    else
    menu_items[0]->switchItem(osd_time-1);
    
    menu_items[0]->setUserCB(&czas_osd_cb,this);
    
    current_item = 0;
    

    connect(parentWidget(),SIGNAL(sigMenuAction(int)),this,SLOT(doSigMenuAction(int)));
    show();
    
    
    for(int i = 0;i<MAX_TV_OPT;i++) menu_items[i]->selectItem(false, false);
    
    if(to_select>=0)current_item = to_select;
    
    if(selected_for_change&&to_select==2)menu_items[current_item]->selectItem(true, true);
    else    
    menu_items[current_item]->selectItem(true, false);
    self_p = this;
    
    if(QCANrcTV::menu_from_can==true)
    {
        legned = new QRNS_Buttons(NULL);
        legned->show();
    }
    else
    {
        legned = NULL;
    }
}



QOtherOptWindow::~QOtherOptWindow() 
{
    for(int i = 0;i<MAX_TV_OPT;i++)
    {
        delete menu_items[i];
        menu_items[i] = NULL;
    }
    
    delete application_settings;
    
    if(legned)delete legned;
}

void QOtherOptWindow::doSigMenuAction(int action)
{

                switch(action)
		  {

                    case LEFT:
                    {
                        //zablokuj wejscie w ostatnia pozycję za pomocą klawisza LEFT na pilocie
                        if(current_item!=3) 
                            menu_items[current_item]->switchToPrevItem();
                    }
                        break;
                        
                        
                       case RIGHT:
                        {
                            if(current_item!=3) 
                            menu_items[current_item]->switchToNextItem();
                        }
                        break;
                        
                    case OK:
                    {
                        if(current_item==3) menu_items[current_item]->switchToNextItem();
                        else
                        {
                            selected_for_change = !selected_for_change;
                            if(selected_for_change)
                            {
                                menu_items[current_item]->selectItem(true,true);
                            }
                            else
                            {
                                menu_items[current_item]->selectItem(true, false);
                            }
                        }
                    }
                        break;

                  case 0x55:          
                    case DOWN:   
                        if(selected_for_change==false)
                        {
                            if(current_item<(MAX_TV_OPT-1))
                            {
                               menu_items[current_item]->selectItem(false, false);
                               current_item++;
                               menu_items[current_item]->selectItem(true, false);
                            }
                            else
                            {
                                menu_items[current_item]->selectItem(false, false);
                               current_item = 0;
                               menu_items[current_item]->selectItem(true, false);
                            }
                        }
                        else
                        {
                            doSigMenuAction(LEFT);
                        }
                     break;
                     
                  case 0x54:    
                   case UP:     
                    if(selected_for_change==false)
                    {
                      if(current_item>0)
                        {
                           menu_items[current_item]->selectItem(false, false);
                           current_item--;
                           menu_items[current_item]->selectItem(true, false);
                        }                 
                       else
                       {
                            menu_items[current_item]->selectItem(false, false);
                            current_item = MAX_TV_OPT-1;
                            menu_items[current_item]->selectItem(true, false);
                       }
                    }
                    else
                    {
                        doSigMenuAction(RIGHT);
                    }
		   break;
                   
                   
		  case _EXIT_:
                  {
                    main_menu *m = new main_menu(parentWidget(),1);   
                    selected_for_change = false;
                    delete this;
                  }
		  break;
                          

                    
                case MENU:
                    selected_for_change = false;
                    delete this;
                break;
                
		default:
		break;
	}

            
}



void QOtherOptWindow::cb1(void *p)
{
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
   v->application_settings->save_record_place(v->menu_items[v->current_item]->currentIdx()); 
}


void QOtherOptWindow::czas_osd_cb(void *p)
{
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
    v->application_settings->save_osd_time((v->menu_items[v->current_item]->currentIdx()+1));
}

void QOtherOptWindow::win_alpha_cb(void *p)
{
QOtherOptWindow *v = ((QOtherOptWindow*)p);
v->application_settings->save_alpha_value_priv(v->menu_items[v->current_item]->currentIdx());

    float temp = 1.0;
    temp = v->application_settings->get_alpha_value();

    v->win_alpha->setOpacity(temp);
}

void QOtherOptWindow::audio_type_cb(void *p)
{
 
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
    v->application_settings->save_audio_mode(v->menu_items[v->current_item]->currentIdx()); 
    
    if(v->application_settings->get_audio_mode()==0)
    {
        system("amixer cset numid=23 49152");
    }
    else
    {
        system("amixer cset numid=23 49664");
    }
    
    ((play_ctrl*)v->parentWidget())->need_change = true;
}

void QOtherOptWindow::sys_vol_cb(void *p)
{
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
    v->application_settings->saveSystemVolume((double)((double)((v->menu_items[v->current_item]->currentIdx())+1)/14));

    need_update_volume = true;
 }

void QOtherOptWindow::language_cb(void *p)
{
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
    v->application_settings->saveSystemLanguage(v->menu_items[v->current_item]->currentIdx());
    
    QEvent::registerEventType();
    foreach (QWidget *widget, QApplication::allWidgets())
    {
         //wyslij event do klasy mostu o zmianie jezyka
         if(widget->objectName()==QString("most"))
         {
         //    printf("QApplication::allWidgets() :%s\n",widget->objectName().toAscii().data());
             QApplication::sendEvent(widget,new QEvent(QEvent::Type(LANGUAGE_CHANGE_CUSTOM_EVENT)));
         }
    }
    
    QOtherOptWindow *o_opt = new QOtherOptWindow(v->parentWidget(),2);
    delete v;
}

void QOtherOptWindow::sys_info_cb(void *p)
{
    QOtherOptWindow *v = ((QOtherOptWindow*)p);
    
    QMenuInfoWindow *info = new QMenuInfoWindow(v->parentWidget());
    delete v;
}