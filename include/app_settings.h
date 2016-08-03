/** 
 * @file   app_settings.h
 * @author Michał Motyl
 *
 * @date   11 październik 2012, 10:09
 */
#include <QtGui>
#include <QSettings>

#ifndef APP_SETTINGS_H
#define	APP_SETTINGS_H

#define POLSKI          0
#define ENGLISH         1
#define DEUTSCH         2

class app_settings {
    friend class QOtherOptWindow;
public:
    app_settings();
    ~app_settings();
    int get_channel_idx(void);
    int get_record_place(void);
    int get_volume(void);
    int get_osd_time(void);
    int get_audio_mode(void);//0 - analog 1 -digital
    int get_tv_state(void);//1-tv, 0-media player
    int getAudioView();
    int getSystemLanguage();
    float get_alpha_value();
    double getSystemVolume(void);
    uint32_t getViaSettings(void); 
    QString getLastFile();
    QString getLastDir();
    int getLastMediaSource();
    int getLastMediaTime();
    int getDirDepth();
    
    void save_channel_idx(int val);
    void save_record_place(int val);
    void save_volume(int val);
    void save_osd_time(int val);
    void save_audio_mode(int val);//0 - analog 1 -digital
    void save_tv_state(int val);//1-tv, 0-media player
    void saveAudioView(int state);
    void saveSystemLanguage(int lang);
    void saveSystemVolume(double val);
    void saveViaSettings(uint32_t val); 
    void saveLastFile(QString file);
    void saveLastDir(QString file);
    void saveLastMediaSource(int src);
    void saveLastMediaTime(int time);
    void saveDirDepth(int dd);
private:
//ustawienia aplikacji
    QSettings           *application_settings;
    bool app_settings_load(QString setting_name,QVariant *val);
    bool app_settings_save(QString setting_name,QVariant val);
    int  get_alpha_value_priv();
    void save_alpha_value_priv(int val);
};

#endif	/* APP_SETTINGS_H */

