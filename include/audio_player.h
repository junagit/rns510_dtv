/** 
 * @file   audio_player.h
 * @author michalm
 *
 * @date   16 kwiecień 2012, 11:24
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappbuffer.h>

#ifndef AUDIO_PLAYER_H
#define	AUDIO_PLAYER_H

#define GET_STATE_DELAY  10 //define the get state wait timer

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

class audio_player {
public:
    audio_player();
    ~audio_player();
    bool play_file(char *file);
    bool stop();
    bool pause();
    bool seek_to(int msec);
    bool isPlaying();
    bool seekTo(int msec);
    bool getCurrentPosition(int & msec);
    bool getDuration(int & msec);
    bool reset();
    bool setLooping(int loop);

    bool get_vs_value(int64_t & value);
    bool eq_set_equalizer(int eqvalue);
private:
    GstElement          *pipeline;
    GMainLoop           *loop;
    GstStateChangeReturn state_return;
    GMainContext        *mMainCtx;	
    GThread             *mBusMessageThread;
    GSource             *mBusWatch;
    int                 mPlayingState;//tom 1231 added 
    bool                mSeeking;
    GstState            mSeekState;
    GMutex              *mActionMutex;
    bool                mAsynchPreparePending;
    bool                mIsLooping;
};

#endif	/* AUDIO_PLAYER_H */

