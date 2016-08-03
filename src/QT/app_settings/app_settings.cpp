/** 
 * @file   app_settings.cpp
 * @author Michał Motyl
 *
 * @date   11 październik 2012, 10:09
 */

#include "app_settings.h"
#include "AS_language.h"

app_settings::app_settings() 
{
    application_settings = NULL; 
    application_settings = new QSettings("/media/localdata/DTV.ini",QSettings::IniFormat);
}

app_settings::~app_settings() 
{
    delete application_settings;
}


bool app_settings::app_settings_load(QString setting_name,QVariant *val)
{
    if(application_settings==NULL) return false;
    
//    if(setting_name=="system_volume")
//    {
//        *val = application_settings->value(setting_name,0.07);
//        return true;
//    }
    

    

        *val = application_settings->value(setting_name);
    

    return true;
}



bool app_settings::app_settings_save(QString setting_name,QVariant val)
{
    if(application_settings==NULL) return false;
    
    application_settings->setValue(setting_name,val);
    application_settings->sync();
    
    
    return true;
}

int app_settings::get_channel_idx(void)
{
    int ret = -1;
    QVariant temp;
    app_settings_load("current_ch_index",&temp);
    
    
    ret = temp.toInt();
    return ret;
}

int app_settings::get_record_place(void)
{
    int ret = -1;
    QVariant temp;
    app_settings_load("tv_record_location",&temp);
    ret = temp.toInt();
    return ret;
    
}

int app_settings::get_volume(void)
{
    int ret = -1;
    QVariant temp;
    app_settings_load("current_volume",&temp);
    ret = temp.toInt();
    return ret;
}

void app_settings::save_channel_idx(int val)
{
    QString n = "current_ch_index";
    app_settings_save(n,(QVariant)val);

}

void app_settings::save_record_place(int val)
{
    QString n = "tv_record_location";
    app_settings_save(n,(QVariant)val);
}

void app_settings::save_volume(int val)
{
    QString n = "current_volume";
    app_settings_save(n,(QVariant)val);
}

int app_settings::get_osd_time()
{
    int ret = -1;
    QVariant temp;
//    app_settings_load("osd_time",&temp);
    
    if(application_settings==NULL) return false;
    
    temp = application_settings->value("osd_time",0);
    
    ret = temp.toInt();
    
    if(ret==1) return 0;
    
    return ret;
}

void app_settings::save_osd_time(int val)
{
    QString n = "osd_time";
    app_settings_save(n,(QVariant)val);
}


int app_settings::get_alpha_value_priv()
{
    int ret = -1;
    QVariant temp;
    app_settings_load("alpha_value",&temp);
    ret = temp.toInt();
    return ret;
}

void app_settings::save_alpha_value_priv(int val)
{
    QString n = "alpha_value";
    app_settings_save(n,(QVariant)val);
}

float app_settings::get_alpha_value()
{
    int ret = -1;
    ret = get_alpha_value_priv();
    
    if(ret==0)return (float)1.00;
        
        if(ret==1)return (float)0.90;
        
        if(ret==2)return (float)0.80;

        if(ret==3)return (float)0.70;
        
        if(ret==4) return (float) 0.60;
    
    return (float)0.70;
}

int app_settings::get_audio_mode(void)
{
    int ret = -1;
    QVariant temp;
    app_settings_load("audio_mode",&temp);
    ret = temp.toInt();
    return ret;
}

void app_settings::save_audio_mode(int val)
{
    QString n = "audio_mode";
    app_settings_save(n,(QVariant)val);
    
}

int app_settings::get_tv_state(void)
{
    int ret = -1;
    QVariant temp;
    

    temp = application_settings->value("tv_state",1);

    ret = temp.toInt();
    return ret;
}

void app_settings::save_tv_state(int val)
{
    QString n = "tv_state";
    app_settings_save(n,(QVariant)val);
}

int app_settings::getAudioView()
{
    int ret = -1;
    QVariant temp;
    app_settings_load("audio_view",&temp);
    ret = temp.toInt();
    return ret;
}

void app_settings::saveAudioView(int state)
{
     QString n = "audio_view";
     app_settings_save(n,(QVariant)state);
}


double app_settings::getSystemVolume(void)
{
    double ret = -1;
    QVariant temp;
    
    if(application_settings==NULL)
    {
        return false;
    }
    temp = application_settings->value("system_volume",0.357142857142857);

//    app_settings_load("system_volume",&temp);
    ret = temp.toDouble();
//    printf("getSystemVolume:%f\n",ret);
    return ret;
}

void app_settings::saveSystemVolume(double val)
{
    QString n = "system_volume";
//    printf("saveSystemVolume:%f\n",val);
     app_settings_save(n,(QVariant)val);
}

int app_settings::getSystemLanguage()
{
    int ret = -1;
    QVariant temp;
//    app_settings_load("language",&temp);
    if(application_settings==NULL)
    {
        return false;
    }
    temp = application_settings->value("language",0);
    
    ret = temp.toInt();
    return ret;
}

void app_settings::saveSystemLanguage(int lang)
{
    QString n = "language";
    app_settings_save(n,(QVariant)lang);
}

void app_settings::saveViaSettings(uint32_t val)
{
    QString n = "via";
    app_settings_save(n,(QVariant)val);
}

uint32_t app_settings::getViaSettings(void)
{
    QVariant temp;
    app_settings_load("via",&temp);
    return temp.toInt();
}

void app_settings::saveLastFile(QString file)
{
    QString n = "last_file";
    app_settings_save(n,(QVariant)file);
}

QString app_settings::getLastFile()
{
    QVariant temp;
    app_settings_load("last_file",&temp);
    return temp.toString();
}

void app_settings::saveLastDir(QString file)
{
    QString n = "last_dir";
    app_settings_save(n,(QVariant)file);
}

QString app_settings::getLastDir()
{
    QVariant temp;
    app_settings_load("last_dir",&temp);
    return temp.toString();
}

void app_settings::saveLastMediaSource(int src)
{
    QString n = "media_source";
    app_settings_save(n,(QVariant)src);
}

int app_settings::getLastMediaSource()
{
    QVariant temp;
    app_settings_load("media_source",&temp);
    return temp.toInt();
}

void app_settings::saveLastMediaTime(int time)
{
        QString n = "last_time";
    app_settings_save(n,(QVariant)time);
}

int app_settings::getLastMediaTime()
{
    QVariant temp;
    app_settings_load("last_time",&temp);
    return temp.toInt();
}

int app_settings::getDirDepth()
{
    QVariant temp;
    app_settings_load("dir_depth",&temp);
    return temp.toInt();
}

void app_settings::saveDirDepth(int dd)
{
    QString n = "dir_depth";
    app_settings_save(n,(QVariant)dd);
}