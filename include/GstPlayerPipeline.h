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


 
#ifndef _GST_PLAYER_PIPELINE_H_
#define _GST_PLAYER_PIPELINE_H_
//#include <utils/Errors.h>
#ifdef ANDROID 
#include <media/mediaplayer.h>
#include <media/MediaPlayerInterface.h>
#endif
#include "GstPlayer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#ifdef ANDROID 
using namespace android;
#endif

#define FF_FB_NEW_MODE  //for ff fb new operation

enum PlayingState{
    mPlaying_No = 0,
    mPlaying_is = 1,
    mPlaying_stop = 2,
    mPlaying_complete = 3,
    mPlaying_pause = 4,
    mPlaying_seek =5,
    mPlaying_FF = 6,
    mPlaying_FB = 7,
};


//add spectband define for spectrum by lorrylv
static guint spect_bands = 40;
#define AUDIOFREQ 32000
static gdouble co_mag[40];
static gdouble co_phase[40];

//add equalizer type value by lorrylv  
static gdouble band_state[][10]={
{ 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},//default
{ 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -7.200000, -7.200000, -7.200000, -9.600000},//classical
{ 9.600000, 7.100000, 2.400000, 0.000000, 0.000000, -6.600000, -6.600000, -7.200000, -0.000000, -0.000000},//Dance
{ -1.700000, 4.800000, 7.100000, 8.000000, 6.600000, 0.000000, -2.600000, -2.600000, -1.700000, -1.600000},//pop
{ 8.000000, 4.800000, -6.600000, -8.000000, -5.500000, 4.000000, 8.800000, 11.100000, 11.100000, 11.200000},//rock
{ -8.000000, 9.600000, 9.600000, 6.600000, 1.600000, -4.000000, -8.000000,-10.400000, -11.200000, -11.200000},//full bass
};
/*
{ 0.000000, 0.000000, 8.000000, 6.600000, 6.600000, 6.600000, 3.200000, 0.000000, 0.000000, 0.000000},//club
{ 9.600000, 7.100000, 2.400000, 0.000000, 0.000000, -6.600000, -6.600000, -7.200000, -0.000000, -0.000000},//Dance
{ -8.000000, 9.600000, 9.600000, 6.600000, 1.600000, -4.000000, -8.000000,-10.400000, -11.200000, -11.200000},//full bass
{ 7.100000, 6.600000, 0.000000, -7.200000, -4.900000, 1.600000, 8.000000, 11.100000, 12.000000, 12.000000},//fullbass treble
{ -9.700000, -9.700000, -9.700000, -4.000000, 2.400000, 11.100000, 16.000000, 16.000000, 16.000000, 16.800000},//full treble
{ 4.800000, 11.100000, 6.600000, -5.500000, -2.600000, 1.600000, 4.800000, 9.600000, 12.800000, 14.400000},//headphones
{ 10.500000, 10.500000, 6.600000, 6.600000, 0.000000, -4.900000, -4.900000, -4.900000, 0.000000, -0.000000},//large hall
{ -4.900000, 0.000000, 4.000000, 6.600000, 6.600000, 6.600000, 4.000000, 2.400000, 2.400000, 2.400000},//live
{ 7.100000, 7.100000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 7.100000, 7.200000},//party
{ -1.700000, 4.800000, 7.100000, 8.000000, 6.600000, 0.000000, -2.600000, -2.600000, -1.700000, -1.600000},//pop
{ 0.000000, 0.000000, 0.000000, -6.600000, 0.000000, 6.400000, 6.400000, 0.000000, 0.000000, -0.000000},//reggae
{ 8.000000, 4.800000, -6.600000, -8.000000, -5.500000, 4.000000, 8.800000, 11.100000, 11.100000, 11.200000},//rock
{ -2.600000, -4.900000, -4.000000, 0.000000, 4.000000, 6.600000, 8.800000, 9.600000, 11.100000, 9.600000},//ska
{ 4.800000, 1.600000, 0.000000, -2.600000, 0.000000, 4.000000, 8.000000, 9.600000, 11.100000, 12.000000},//soft
{ 4.000000, 4.000000, 2.600000, 0.000000, -4.000000, -6.600000, -5.500000, 0.000000, 2.400000, 8.800000},//soft rock
{ 8.000000, 6.600000, 0.000000, -6.600000, -4.900000, 0.000000, 8.000000, 9.600000, 9.600000, 8.000000},//techno
};
*/
// The class to handle gst pipeline
class GstPlayerPipeline
{
public:
    GstPlayerPipeline(GstPlayer* gstPlayer, int mediatype);
    ~GstPlayerPipeline();

    bool setDataSource(const char *url);
    bool setDataSource(int fd, int64_t offset, int64_t length);
#ifdef ANDROID 	
    bool setAudioSink(sp<MediaPlayerInterface::AudioSink> audiosink);
    bool setVideoSurface(const sp<ISurface>& surface);
#else
    bool setAudioSink(/* maybe need to add param*/);
    bool setVideoSurface(/* maybe need to add param*/);	
#endif
    bool prepare();
    bool prepareAsync();
    bool start();
    bool stop();
    bool pause();
    bool isPlaying();
    bool getVideoSize(int *w, int *h);
    bool seekTo(int msec);
    bool getCurrentPosition(int *msec);
    bool getDuration(int *msec);
    bool reset();
    bool setLooping(int loop);
    void vol_set(double vol);
    void mute(bool f);
    bool get_vs_value(int64_t *value);
    bool eq_set_equalizer(int eqvalue);
    bool FastForward(int front_state);
    bool FastBack(int back_state);
    bool SetAudioPlayStyle(int audio_style);
    bool SetVideoInfo(int x, int y, int w, int h, int stride);
    int getPipeInitState(void);
    //subtitle api
    bool subtitle_open();
    bool subtitle_closed();
    bool subtitle_setPos(int posx, int poxy); 
    bool subtitle_setSize( int width, int height );
    bool subtitle_setAlpha(int alpha_type, int value);
    bool subtitle_setFont( const gchar* font_desc );
    bool subtitle_setFile( const gchar* sub_url );
    int subtitle_Status(void );
    // for multi subtitle track
    int subtitle_get_track(void);
    bool subtitle_set_track(int track);
    // for multi audio track
    int audio_get_track(void);
    bool audio_set_track(int track);
    
private:
    // static apis
    static GstBusSyncReply bus_callback (GstBus *bus, GstMessage *msg, gpointer data);
    static void playbin2_found_source(GObject * object, GObject * orig, 
            GParamSpec * pspec, gpointer player_pipeline);
    static void appsrc_need_data(GstAppSrc *src, guint length, 
            gpointer user_data);
    static void appsrc_enough_data(GstAppSrc *src, gpointer user_data);
    static gboolean appsrc_seek_data(GstAppSrc *src, guint64 offset,
            gpointer user_data);
    static void taglist_foreach(const GstTagList *list, const gchar *tag, 
            gpointer user_data);
    //added for subtitle
    static void subtitle_signal_func(GObject * object, GObject * orig, 
            GParamSpec * pspec, gpointer player_pipeline);
#ifdef FF_FB_NEW_MODE
     static gboolean  FF_FB_Timer_callback (gpointer user_data);
#endif
    
    // private apis
    bool create_pipeline();
    void delete_pipeline();  

    void handleEos(GstMessage* p_msg);
    void handleError(GstMessage* p_msg);
    void handleTag(GstMessage* p_msg);
    void handleBuffering(GstMessage* p_msg);
    void handleStateChanged(GstMessage* p_msg);
    void handleAsyncDone(GstMessage* p_msg);
    void handleSegmentDone(GstMessage* p_msg);
    void handleDuration(GstMessage* p_msg);
    void handleElement(GstMessage* p_msg);
    void handleApplication(GstMessage* p_msg);

	void handleWarning(GstMessage* p_msg);
    GstPlayer*  mGstPlayer;

    // gst elements
    GstElement* mPlayBin;
    GstElement* mAudioSink;
    GstElement* mVideoSink;
#ifdef SSL_GST_VPPINSINK
    GstElement* mTextSink;
#endif  //SSL_GST_VPPINSINK
    GstAppSrc* mAppSource;

    GstElement* mEqualizer;	
    GstElement* mSpectrum;
    GstElement* mConvert;
    GstElement* mVol;
    GstElement* audio_bin;

    GstPad        *pad1;

	gint mDruation;
    // app source 
    int mFd;
    guint64  mLength;
    guint64  mOffset;
    guint8*  mMemBase;
    // seek
    bool     mSeeking;
    GstState mSeekState;
    // prepare
    bool mAsynchPreparePending;
    // loop
    bool mIsLooping;
    gdouble m_speed;//tom adder 1224
    int mPlayingState;//tom 1231 added 
    int mMediaType;// MediaTyp_video or MediaTyp_Audio
    bool mCompletedFlag;
    int mPipe_init_state;// 1;ok,0:error
    int mHttpStreamFlag;
#ifdef FF_FB_NEW_MODE    
volatile int mFFFBLoopFlag;// 1 :run, 0:stop, clean the flag, sys will exit the ff/fb timer call back
    int mFrameCount;
    gint64  mPreFrameTime;
volatile	int mFFFBTRUE;
	GThread*  mFFFBThread;
	static gpointer fffb_loop (GstPlayerPipeline *ed);
#endif

    int mTrackFlag;
            
    //subtitle control
    int mSubXpos;
    int mSubYpos;
    int mSubAlpha;
    int mSubDisplay;
    int mSubWidth;
    int mSubHeight;
    gchar* mSubFont;
    gchar* mSubFile;

    //video position info
    int mVideoPosX;
    int mVideoPosY;
    int mVideoWidth;
    int mVideoHeigh;
    int mVideoStride;
    // internal audio sink
#ifdef ANDROID 	    
    sp<MediaPlayerInterface::AudioSink> mAudioOut;
#endif

    // main loop
    GMainContext*   mMainCtx;	
    GSource*        mBusWatch;
    static gpointer do_loop (GstPlayerPipeline *ed);
  
    GMainLoop*  mMainLoop;
    GThread*      mBusMessageThread;
#ifdef ANDROID    
    pthread_mutex_t  mActionMutex;
#else
    GMutex *mActionMutex;
#endif
}; 

#endif   /*_GST_PLAYER_PIPELINE_H_*/
