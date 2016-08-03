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

#include "Gst_config.h"
//#include <utils/Log.h>
#include <sys/prctl.h>
#include <sys/resource.h>
//#include <media/thread_init.h>
//#include <utils/threads.h>
//#include <utils/List.h>
#include "GstPlayer.h"
#include "GstPlayerPipeline.h"
#include <stdio.h>
#ifdef ANDROID 
namespace android {
#endif
// ----------------------------------------------------------------------------
// implement the Gstreamer player
// ----------------------------------------------------------------------------
GstPlayer::GstPlayer(int mediatype)
{
    GST_DEBUG_Player ("Enter\n");
    mDataSourcePath = NULL;
    mGstPlayerPipeline = new GstPlayerPipeline(this, mediatype);
    if (mGstPlayerPipeline != NULL)
    {
        if(mGstPlayerPipeline->getPipeInitState())
            mInit = GSTPlayer_OK;
        else
            mInit = GSTPlayer_NO_INIT;
    }
    else
    {
        mInit = GSTPlayer_NO_INIT;
    }
    mIsDataSourceSet = false;
    mDuration = -1;
    mPlayer_CB = NULL;
    mMediaPlayingState = -1;
#ifdef ANDROID 	
    mSurface = NULL;
#endif
    GST_DEBUG_Player ("Leave\n");
}

status_t GstPlayer::initCheck()
{
    GST_DEBUG_Player ("mInit = %s\n", (mInit==GSTPlayer_OK)?"GSTPlayer_OK":"ERROR");
    return mInit;
}

GstPlayer::~GstPlayer()
{
    GST_DEBUG_Player ("Enter\n");
    if (mGstPlayerPipeline )
    {
        delete mGstPlayerPipeline;
        mGstPlayerPipeline = NULL;
    }
    
#ifdef ANDROID 	
    GST_DEBUG_Player ("Clear Surface.\n");
    if (mSurface != NULL)
    {
        mSurface.clear();
    }
#endif	
//    if(mDataSourcePath)
//        free(mDataSourcePath);
    mPlayer_CB = NULL;
    GST_DEBUG_Player ("Leave\n");
}

status_t GstPlayer::setAudioStreamType(int type)
{
    mStreamType = type;
    GST_DEBUG_Player ("mStreamType = %d\n", mStreamType);
    return GSTPlayer_OK;
}

status_t GstPlayer::setDataSource(const char *url)
{
    status_t ret = GSTPlayer_UNKNOW_ERROR;

//    printf("setDataSource:%s\n",url);
    
    if (mGstPlayerPipeline == NULL || url == NULL)
        return GSTPlayer_UNKNOW_ERROR;



    if (mGstPlayerPipeline->setDataSource(url) != true)
    {
        printf("Cannot set data source:%s\n",url);
        goto EXIT;
    }


    if (mGstPlayerPipeline->setAudioSink( ) != true)
    {
        printf("Cannot set audio sink\n");
        goto EXIT;
    }

    ret = GSTPlayer_OK;
EXIT:    
    return ret;
}

status_t GstPlayer::setDataSource(int fd, int64_t offset, int64_t length)
{
    status_t ret = GSTPlayer_UNKNOW_ERROR;

    GST_DEBUG_Player("fd: %i, offset: %ld, len: %ld\n", fd, (long)offset, (long)length);

    if (mGstPlayerPipeline->setDataSource(fd, offset, length) != true)
    {
        GST_DEBUG_Player("Cannot set data source\n");
        goto EXIT;
    }
#ifdef ANDROID	
    if (mGstPlayerPipeline->setAudioSink(mAudioSink) != true)
#else
	 if (mGstPlayerPipeline->setAudioSink( ) != true)
#endif
    {
        GST_DEBUG_Player("Cannot set audio sink\n");
        goto EXIT;
    }

    ret = GSTPlayer_OK;
EXIT:    
    return ret;
}

#ifdef ANDROID 
status_t GstPlayer::setVideoSurface(const sp<ISurface>& surface)
{
    GST_DEBUG_Player("ISurface: %p\n", surface.get());
    mSurface = surface;
    return GSTPlayer_OK;
}
#endif
status_t GstPlayer::prepare()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->prepare() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

void GstPlayer::vol_set(double vol)
{
    mGstPlayerPipeline->vol_set(vol);
}

void GstPlayer::mute(bool f)
{
    mGstPlayerPipeline->mute(f);
}

status_t GstPlayer::prepareAsync()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->prepareAsync() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::start()
{

    
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;


    return mGstPlayerPipeline->start() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::stop()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->stop() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::pause()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->pause() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

bool GstPlayer::isPlaying()
{
    if(mGstPlayerPipeline == NULL)
        return false;
    else
        return mGstPlayerPipeline->isPlaying();
}

status_t GstPlayer::getVideoWidth(int *w)
{
    int h;
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->getVideoSize(w, &h) ?  
        GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::getVideoHeight(int *h)
{
    int w;
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;    

    return mGstPlayerPipeline->getVideoSize(&w, h) ?  
        GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::getCurrentPosition(int *msec)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;    

    return mGstPlayerPipeline->getCurrentPosition(msec) ?  
        GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::getDuration(int *msec)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;    

    return mGstPlayerPipeline->getDuration(msec) ?  
        GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::seekTo(int msec)
{
    GST_DEBUG_Player ("seekTo(%d)\n", msec);
    // can't always seek to end of streams - so we fudge a little
    if ((msec == mDuration) && (mDuration > 0)) {
        msec--;
        GST_DEBUG_Player ("Seek adjusted 1 msec from end\n");
    }
    if (!mGstPlayerPipeline->seekTo(msec))
    {
        GST_DEBUG_Player ("Failed to seekTo() in Gstplayer Pipeline.\n");
        return GSTPlayer_UNKNOW_ERROR;
    }      

    /*if (!mGstPlayerPipeline->seekTo(msec)) //tom 110310 
    {
        GST_DEBUG_Player ("Failed to seekTo() in Gstplayer Pipeline.\n");
        return GSTPlayer_UNKNOW_ERROR;
    }*/   
    return GSTPlayer_OK;
}

status_t GstPlayer::reset()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;    

    return mGstPlayerPipeline->reset() ?  GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer::setLooping(int loop)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;    

    return mGstPlayerPipeline->setLooping(loop) ?  GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

 status_t GstPlayer::get_vs_value(int64_t *vs_value)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->get_vs_value(vs_value) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

 status_t GstPlayer::eq_set_equalizer(int eq_value)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->eq_set_equalizer(eq_value) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

 status_t GstPlayer::FastForward(int front_state)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->FastForward(front_state) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}
 status_t GstPlayer::FastBack(int back_state)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->FastBack(back_state) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}
 status_t GstPlayer::SetAudioPlayStyle(int audio_style)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->SetAudioPlayStyle(audio_style) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;

}

 status_t    GstPlayer::SetVideoInfo(int x, int y, int w, int h, int stride)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->SetVideoInfo(x,y,w,h,stride) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
   
}
 /***** added subtitle api here *****/
 //added subtitle api here
status_t GstPlayer:: subtitle_open()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_open() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}
status_t GstPlayer::subtitle_closed()
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_closed() ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;

}
status_t GstPlayer:: subtitle_setPos(int posx, int poxy)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_setPos( posx,  poxy)? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;

}
status_t GstPlayer:: subtitle_setSize(int width, int height)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_setSize( width,  height)? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;

}
status_t GstPlayer:: subtitle_setAlpha(int alpha_type, int value)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_setAlpha( alpha_type,  value) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;

}

status_t GstPlayer:: subtitle_setFont( const char* font_desc )
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;
    return mGstPlayerPipeline->subtitle_setFont( font_desc ) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

status_t GstPlayer:: subtitle_setFile( const char* sub_url )
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;
    return mGstPlayerPipeline->subtitle_setFile( sub_url ) ? GSTPlayer_OK : GSTPlayer_UNKNOW_ERROR;
}

int GstPlayer:: subtitle_Status(void)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_Status();

}
// for multi subtitle track
int GstPlayer::subtitle_get_track(void)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_get_track();	
}
status_t GstPlayer::subtitle_set_track(int track)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->subtitle_set_track(track);
}
// for multi audio track
int GstPlayer::audio_get_track(void)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->audio_get_track();	
}
status_t GstPlayer::audio_set_track(int track)
{
    if(mGstPlayerPipeline == NULL)
        return GSTPlayer_UNKNOW_ERROR;

    return mGstPlayerPipeline->audio_set_track(track);	
}
    
#ifndef ANDROID
void GstPlayer::sendEvent(int msg, int ext1, int ext2)
{
//	int i = 0;
       mMediaPlayingState = msg;
       
       if(mPlayer_CB)
        {
            mPlayer_CB(mMediaPlayingState);
       }
       GST_DEBUG_Player("**** mMediaPlayingState is %d****\n", mMediaPlayingState);
}
#endif
int GstPlayer::getGstPlayerState(void)
{
    return mMediaPlayingState;
}

int GstPlayer::setGstPlayerCB(GstPlayer_CB func)
{
    if(func)
    {
        mPlayer_CB = func;
    }
    return 0;
 }
 
#ifdef ANDROID
}; // namespace android
#endif
