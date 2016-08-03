/******************************************************************************
*                                    Solomon-systech Confidential Proprietary
*                           (c) Copyright Solomon-systech Company Limited
*                                        All Rights Reserved
* Revision History:
*                                                                            Modification              Description of
*   Author                       email                                      Date                         Changes
*
*   tomchen     modify it 20101208
*
 *****************************************************************************/

  
#ifndef ANDROID_GSTPLAYER_H
#define ANDROID_GSTPLAYER_H

#include <gst/gst.h>
#include <glib.h>

#ifdef ANDROID
    #include <utils/Errors.h>
    #include <media/MediaPlayerInterface.h>
#else
    typedef int         status_t;
    typedef int         player_type;
    #define     GST_PLAYER      0x10000  

#endif

#define GST_CONFIG_FILE "/usr/gstreamer/bin/gst.conf"
//define gstplayer error type
#define GSTPlayer_OK                          0x00     //OK
#define GSTPlayer_UNKNOW_ERROR    0x01   //instead android::unknow_error
#define GSTPlayer_NO_INIT                 0x02   //no_init 

enum GST_media_event_type 
{
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,
    MEDIA_FF_COMPLETE = 6,
    MEDIA_FB_COMPLETE = 7,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
};
enum FF_State
{
    FF_normal = 0,    //normal playing speed
    FF_Speed1, // X2 speed
    FF_Speed2, // X4 speed
    FF_Speed3, // X8 speed
    FF_Speed4, // X16 speed
    FF_Speed5, // X32 speed
      
};
enum FB_State
{
    FB_normal = 0, //normal playing back speed
    FB_Speed1,// X2 speed
    FB_Speed2,// X4 speed
    FB_Speed3,// X8 speed
    FB_Speed4,// X16 speed
    FB_Speed5,// X32 speed
      
};
enum GstPlayer_MediaType
{
    MediaType_Video=0,
    MediaType_Audio,
    MediaType_Photo,
    MediaType_Unknow,
};

class GstPlayerPipeline;

typedef int (*GstPlayer_CB)  (int event);


#ifdef ANDROID
    namespace android {
#endif
// GstPlayer
// This is the wrapper layer to integrate gst player into android media 
// player service. To do that, we change
//  .../frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp 
// to load gstplayer or pvplayer by using a config file (gst_conf).
//

#ifdef ANDROID
class GstPlayer : public MediaPlayerInterface
#else
class GstPlayer
#endif
{
public:
    GstPlayer(int mediatype = MediaType_Video);
    virtual             ~GstPlayer();

    virtual status_t    initCheck();
    virtual status_t    setAudioStreamType(int type);
    virtual status_t    setDataSource(const char *url);
    virtual status_t    setDataSource(int fd, int64_t offset, int64_t length);
#ifdef ANDROID	
    virtual status_t    setVideoSurface(const sp<ISurface>& surface);
#endif
    virtual void        vol_set(double vol);
    virtual void        mute(bool f);
    virtual status_t    prepare();
    virtual status_t    prepareAsync();
    virtual status_t    start();
    virtual status_t    stop();
    virtual status_t    pause();
    virtual bool        isPlaying();
    virtual status_t    getVideoWidth(int *w);
    virtual status_t    getVideoHeight(int *h);
    virtual status_t    seekTo(int msec);
    virtual status_t    getCurrentPosition(int *msec);
    virtual status_t    getDuration(int *msec);
    virtual status_t    reset();
    virtual status_t    setLooping(int loop);
    virtual player_type playerType() { return GST_PLAYER; }

    virtual status_t    get_vs_value(int64_t *value);
    virtual status_t    eq_set_equalizer(int eqvalue);
    virtual status_t    FastForward(int front_state);
    virtual status_t    FastBack(int back_state);
    virtual status_t    SetAudioPlayStyle(int audio_style);
    virtual status_t    SetVideoInfo(int x, int y, int w, int h, int stride);
    //added the other function here later
   //added subtitle api here
    virtual status_t subtitle_open();
    virtual status_t subtitle_closed();
    virtual status_t subtitle_setPos(int posx, int poxy);
    virtual status_t subtitle_setSize(int width, int height);
    virtual status_t subtitle_setAlpha(int alpha_type, int value);
    virtual status_t subtitle_setFont( const char *font_desc );
    virtual status_t subtitle_setFile( const char* sub_url );
    virtual int subtitle_Status(void);
    // for multi subtitle track
    virtual int subtitle_get_track(void);
    virtual status_t subtitle_set_track(int track);
    // for multi audio track
    virtual int audio_get_track(void);
    virtual status_t audio_set_track(int track);
    // make available to GstPlayerPipeline
#ifdef ANDROID    
    void sendEvent(int msg, int ext1=0, int ext2=0) { MediaPlayerBase::sendEvent(msg, ext1, ext2); }
#else
     void sendEvent(int msg, int ext1=0, int ext2=0) ;
#endif

    int getGstPlayerState(void);
    int setGstPlayerCB(GstPlayer_CB func);

private:
    GstPlayerPipeline*   mGstPlayerPipeline;
    char *               mDataSourcePath;
    bool                 mIsDataSourceSet;
#ifdef ANDROID 
    sp<ISurface>         mSurface;
#endif
    status_t             mInit;
    int                  mDuration;
    int                  mStreamType;
    int                  mMediaPlayingState;
    GstPlayer_CB  mPlayer_CB;
};

#ifdef ANDROID
}; // namespace android
#endif
//file end

#endif   
