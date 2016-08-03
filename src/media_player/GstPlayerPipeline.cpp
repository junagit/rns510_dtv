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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "GstPlayerPipeline.h"
#include "math.h"
#include <sys/mman.h>
#include <stdio.h>
#ifdef SSL_GST_PORT_VQ
#warning SSL_GST_PORT_VQ
#include "sslMap36MB_0x40000000.h"
#include "mapm.h"
static int sslUpdateAVqueue(GstElement* mPlayBin);
#endif  //SSL_GST_PORT_VQ

#ifdef ANDROID
using namespace android;
#endif
// always enable gst log
#define ENABLE_GST_LOG

#define MSG_QUIT_MAINLOOP   "application/x-quit-mainloop"
#define MSG_PAUSE           "application/x-pause"

#ifdef ANDROID		//tom 1214 changed it
#define INIT_LOCK(pMutex)   pthread_mutex_init(pMutex, NULL)
#define LOCK(pMutex)        pthread_mutex_lock(pMutex)
#define UNLOCK(pMutex)      pthread_mutex_unlock(pMutex)
#define DELETE_LOCK(pMutex) pthread_mutex_destroy(pMutex)
#else
//#define INIT_LOCK(pMutex)   g_mutex_init(pMutex, NULL)
#define LOCK(pMutex)         g_mutex_lock(*pMutex)
#define UNLOCK(pMutex)       g_mutex_unlock(*pMutex)
#define DELETE_LOCK(pMutex)  g_mutex_free(*pMutex)
#endif


#ifndef PAGESIZE
#define PAGESIZE            4096
#endif

#define LCD_POSX_MAX        1280
#define LCD_POSY_MAX        800
#define LCD_WIDTH_MAX       1280
#define LCD_HEIGHT_MAX      800
#define LCD_STRIDE_MAX      1280

#define FF_FB_SWITCH_TIMER    500 //ms
#define BASE_TIMER      (0.50)   //from FF_FB_SWITCH_TIMER
#define FF_FB_LEVEL1_TIMER     (BASE_TIMER*2) //(0.4) //s
#define FF_FB_LEVEL2_TIMER     (BASE_TIMER*4) //(0.8)
#define FF_FB_LEVEL3_TIMER      (BASE_TIMER*8)//(1.6)
#define FF_FB_LEVEL4_TIMER      (BASE_TIMER*16)//(3.2)
#define FF_FB_LEVEL5_TIMER      (BASE_TIMER*32)//(6.4)

#define GET_STATE_DELAY  10 //define the get state wait timer
//#define MEM_DEBUG
//#define TWOINONE
// make sure gst can be initialized only once
static gboolean gst_inited = FALSE;


// ----------------------------------------------------------------------------
// helper functions to redirect gstreamer log to android's log system
// ----------------------------------------------------------------------------
#define GST_CONFIG_ENVIRONMENT_GROUP  "Environment"
#define NUL '\0'

//// get_gst_env_from_conf()
//// Read and export environment variables from /sdcard/gst.conf.
////
//static int get_gst_env_from_conf()
//{
//    gboolean res = FALSE;
//    GKeyFile* conf_file = NULL;
//    gchar** key_list = NULL;
//    gsize length = 0;
//    GError* error = NULL;
// 
//    // open and load GST_CONFIG_FILE
//    conf_file = g_key_file_new ();
//    if(conf_file == NULL)
//        return -1;
//
//    GST_DEBUG_Player("Load config file: "GST_CONFIG_FILE"\n");
//    res = g_key_file_load_from_file(
//        conf_file,
//        GST_CONFIG_FILE,
//        G_KEY_FILE_NONE,
//        &error);
//    if(res != TRUE)
//    {
//        if(error)
//            GST_DEBUG_Player ("Load config file error: %d (%s)\n", 
//                error->code, error->message);
//        else
//            GST_DEBUG_Player ("Load config file error.\n");
//        goto EXIT;
//    }
//
//    // enumerate all environment viarables
//    error = NULL;
//    key_list = g_key_file_get_keys(
//        conf_file,
//        GST_CONFIG_ENVIRONMENT_GROUP,
//        &length,
//        &error);
//    if(key_list == NULL)
//    {
//        if(error)
//            GST_DEBUG_Player ("Fail to get environment vars: %d (%s)\n",
//                error->code, error->message);
//        else
//            GST_DEBUG_Player ("Fail to get environment vars\n");
//        res = FALSE;
//        goto EXIT;
//    }
//
//    // export all environment viarables
//    for(gsize i=0; i<length; i++)
//    {
//        if(key_list[i])
//        {
//            gchar* key = key_list[i];
//            gchar* value = g_key_file_get_string(
//                conf_file,
//                GST_CONFIG_ENVIRONMENT_GROUP,
//                key,
//                NULL);
//            if(value)
//            {
//                setenv(key, value, 1);
//                GST_DEBUG_Player("setenv:  %s=%s\n", key, value);
//            }
//            else
//            {
//                unsetenv(key);
//                GST_DEBUG_Player("unsetsev:  %s\n", key);
//            }
//        }
//    }
//
//EXIT:
//    // release resource
//    if(conf_file)
//        g_key_file_free(conf_file);
//    if(key_list)
//        g_strfreev(key_list);
//
//    return (res == TRUE) ? 0 : -1;
//}

// android_gst_debug_log()
// Hook function to redirect gst log from stdout to android log system
//
//static void android_gst_debug_log (GstDebugCategory * category, GstDebugLevel level,
//    const gchar * file, const gchar * function, gint line,
//    GObject * object, GstDebugMessage * message, gpointer unused)
//{
//    if (level > gst_debug_category_get_threshold (category))
//    return;
//
//    const gchar *sfile = NULL;
//    // Remove the full path before file name. All android code build from top
//    // folder, like .../external/gst-plugin-android/player/GstPlayer.cpp, which
//    // make log too long. 
//#if 0    //tom 1214 added 
//    sfile  = GST_PLAYER_GET_SHORT_FILENAME(file);
//
//    // redirect gst log to android log
//    switch (level)
//    {
//    case GST_LEVEL_ERROR:
//        GST_DEBUG_Player ("[%d], %s   %s:%d:%s: %s\n", 
//            gettid(),
//            gst_debug_level_get_name (level), 
//            sfile, line, function, 
//            (char *) gst_debug_message_get (message));  
//        break;
//    case GST_LEVEL_WARNING:
//        GST_DEBUG_Player ("[%d], %s   %s:%d:%s: %s\n", 
//            gettid(),
//            gst_debug_level_get_name (level), 
//            sfile, line, function, 
//            (char *) gst_debug_message_get (message));  
//        break;
//    case GST_LEVEL_INFO:
//        GST_DEBUG_Player ("[%d], %s   %s:%d:%s: %s\n", 
//            gettid(),
//            gst_debug_level_get_name (level), 
//            sfile, line, function, 
//            (char *) gst_debug_message_get (message));  
//        break;
//    case GST_LEVEL_DEBUG:
//        GST_DEBUG_Player ("[%d], %s   %s:%d:%s: %s\n",
//            gettid(),
//            gst_debug_level_get_name (level), 
//            sfile, line, function, 
//            (char *) gst_debug_message_get (message));  
//        break;
//    case GST_LEVEL_LOG:
//        GST_DEBUG_Player ("[%d], %s   %s:%d:%s: %s\n", 
//            gettid(),
//            gst_debug_level_get_name (level), 
//            sfile, line, function, 
//            (char *) gst_debug_message_get (message));  
//        break;
//    default:
//        break;
//    }
//	#endif
//}

static gboolean init_gst()
{
    if (!gst_inited)
    {
        char * argv[2];
        char**argv2;
        int argc = 0;
        GST_DEBUG_Player ("Initialize gst context\n");

        if (!g_thread_supported ())
            g_thread_init (NULL);

        // initialize gst
        GST_DEBUG_Player ("Initializing gst\n");
        argv2 = argv;
        argv[0] = "GstPlayer";
        argv[1] = NULL;
        argc++;

        gst_init(&argc, &argv2);
        //gst_debug_remove_log_function(gst_debug_log_default );

        GST_DEBUG_Player ("Gst is Initialized.\n");
        gst_inited = TRUE;    
    }
    else
    {
        GST_DEBUG_Player ("Gst has been initialized\n");
    }
    return TRUE;
}

// ----------------------------------------------------------------------------
// GstPlayerPipeline
// ----------------------------------------------------------------------------
// Message Handler Function
GstBusSyncReply GstPlayerPipeline::bus_callback (GstBus *bus, GstMessage *msg,gpointer data)
{
   
 //tom added for diseal callback func
    GstPlayerPipeline* pGstPlayerPipeline = (GstPlayerPipeline*) data;
   // GstObject *msgsrc=GST_MESSAGE_SRC(msg);
   //printf("Get %s Message From %s\n",GST_MESSAGE_TYPE_NAME(msg), GST_OBJECT_NAME(msgsrc)); //tomchen 1230 del

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:

        pGstPlayerPipeline->handleEos(msg);           
        break;
    case GST_MESSAGE_ERROR:
        pGstPlayerPipeline->handleError(msg);
        break;
    case GST_MESSAGE_TAG:
       // GST_DEBUG_Player("Get %s Message From %s\n", GST_MESSAGE_TYPE_NAME(msg), GST_OBJECT_NAME(msgsrc)); //tomchen 1230 del
       // if(pGstPlayerPipeline->mMediaType == MediaType_Video)
        //    pGstPlayerPipeline->handleTag(msg);
        break;
    case GST_MESSAGE_BUFFERING:
        pGstPlayerPipeline->handleBuffering(msg);
        break;
    case GST_MESSAGE_STATE_CHANGED:
        pGstPlayerPipeline->handleStateChanged(msg); //maybe is bad call it
        break;
    case GST_MESSAGE_ASYNC_DONE:
        //pGstPlayerPipeline->handleAsyncDone(msg);
        break;
    case GST_MESSAGE_ELEMENT:
        //pGstPlayerPipeline->handleElement(msg);//open the get spectrum info function for lorrylv
        break;
    case GST_MESSAGE_APPLICATION:
        pGstPlayerPipeline->handleApplication(msg);
        break;       
    case GST_MESSAGE_SEGMENT_DONE:
        //pGstPlayerPipeline->handleSegmentDone(msg);
        break;
    case GST_MESSAGE_DURATION:
        //pGstPlayerPipeline->handleDuration(msg);
        break;
	case GST_MESSAGE_WARNING:
		pGstPlayerPipeline->handleWarning(msg);
    default:
        break;
    }
#if 0
#ifdef ANDROID	    
    UNLOCK(&pGstPlayerPipeline->mActionMutex);
#else
     g_mutex_unlock(pGstPlayerPipeline->mActionMutex);
#endif    

#endif
	return GST_BUS_PASS;


}

void GstPlayerPipeline::appsrc_need_data(GstAppSrc *src, guint length,
        gpointer user_data)
{
    GstPlayerPipeline* player_pipeline = 
        (GstPlayerPipeline*)user_data;
    GstBuffer *buffer = NULL;
    GstFlowReturn flow_ret;
    bool ret = false;
//    off_t offset = 0;

    // GST_DEBUG_Player ("player_pipeline=%p, Request length=%d, offset=%lu,
    // fd=%d", player_pipeline, length, (long unsigned
    // int)(player_pipeline->mOffset), player_pipeline->mFd);

    // check current offset is inside range
    if (player_pipeline->mOffset > player_pipeline->mLength)
    {
        GST_DEBUG_Player("Offset %lu is outside file %lu. Send EOS\n", 
                (unsigned long)(player_pipeline->mOffset), 
                (unsigned long)(player_pipeline->mLength));
        goto EXIT;
    }
    
    // read the amount of data, we are allowed to return less if we are EOS
    if (player_pipeline->mOffset + length > player_pipeline->mLength)
        length = player_pipeline->mLength - player_pipeline->mOffset;

    // create GstBuffer
    buffer = gst_buffer_new ();
    if(buffer == NULL)
    {
        GST_DEBUG_Player("Cannot create buffer! Send EOS\n");
        goto EXIT;
    }

    GST_BUFFER_DATA (buffer) = player_pipeline->mMemBase + player_pipeline->mOffset;
    GST_BUFFER_SIZE (buffer) = length;
    GST_BUFFER_OFFSET (buffer) = player_pipeline->mOffset;
    GST_BUFFER_OFFSET_END (buffer) =player_pipeline->mOffset + length;

    /*
    GST_DEBUG_Player("offset=%lu, length=%u, data: %02x %02x %02x %02x %02x %02x %02x %02x",
        (unsigned long)player_pipeline->mOffset,
        length,
        *(GST_BUFFER_DATA (buffer) + 0),
        *(GST_BUFFER_DATA (buffer) + 1),
        *(GST_BUFFER_DATA (buffer) + 2),
        *(GST_BUFFER_DATA (buffer) + 3),
        *(GST_BUFFER_DATA (buffer) + 4),
        *(GST_BUFFER_DATA (buffer) + 5),
        *(GST_BUFFER_DATA (buffer) + 6),
        *(GST_BUFFER_DATA (buffer) + 7));
    */

    // send buffer to appsrc
    flow_ret = gst_app_src_push_buffer(player_pipeline->mAppSource, buffer);
 //   if(GST_FLOW_IS_FATAL(flow_ret)) 
 //       GST_DEBUG_Player("Push error %d\n", flow_ret);

    player_pipeline->mOffset += length;
    ret = true;

EXIT:
    // gst_app_src_push_buffer() will steal the GstBuffer's reference, we need
    // not release it here.  
    // if (buffer) gst_buffer_unref (buffer);

    if (ret != true)
        gst_app_src_end_of_stream (src);
}

void GstPlayerPipeline::appsrc_enough_data(GstAppSrc *src, 
        gpointer user_data)
{
    GST_DEBUG_Player ("Shall not enter here\n");
}

gboolean GstPlayerPipeline::appsrc_seek_data(GstAppSrc *src, 
        guint64 offset, gpointer user_data)
{
    GstPlayerPipeline* player_pipeline = 
        (GstPlayerPipeline*)user_data;

    // GST_DEBUG_Player ("Enter, offset=%lu\n", (long unsigned int)offset);
    player_pipeline->mOffset = offset;
    return TRUE;
}

// this callback is called when playbin2 has constructed a source object to
// read from. Since we provided the appsrc:// uri to playbin2, this will be the
// appsrc that we must handle. We set up a signals to push data into appsrc. 
void GstPlayerPipeline::playbin2_found_source(GObject * object, GObject * orig, 
    GParamSpec * pspec, gpointer user_data)
{
    GstPlayerPipeline* player_pipeline = 
        (GstPlayerPipeline*)user_data;



    // get a handle to the appsrc
    if (player_pipeline->mAppSource)
    {
        g_object_unref(player_pipeline->mAppSource);
        player_pipeline->mAppSource = NULL;
    }
    g_object_get (orig, pspec->name, &(player_pipeline->mAppSource), NULL);
    GST_DEBUG_Player ("appsrc: %p\n", player_pipeline->mAppSource);

    // we can set the length in appsrc. This allows some elements to estimate
    // the total duration of the stream. It's a good idea to set the property
    // when you can but it's not required.  
    gst_app_src_set_size(player_pipeline->mAppSource, player_pipeline->mLength);

    // configure the appsrc to work in pull (random access) mode 
    gst_app_src_set_stream_type(player_pipeline->mAppSource, 
            GST_APP_STREAM_TYPE_RANDOM_ACCESS);
    

    // set appsrc callback 
    GstAppSrcCallbacks callback;
    callback.need_data = appsrc_need_data;
    callback.enough_data = appsrc_enough_data;
    callback.seek_data = appsrc_seek_data;
    gst_app_src_set_callbacks(player_pipeline->mAppSource, &callback, 
            player_pipeline, NULL);
}

void GstPlayerPipeline::taglist_foreach(const GstTagList *list, 
            const gchar *tag, gpointer user_data)
{
    // following code is copy from gst-plugins-bad/examples/gstplay/player.c
    gint i, count;

    count = gst_tag_list_get_tag_size (list, tag);

    for (i = 0; i < count; i++) {
        gchar *str;

        if (gst_tag_get_type (tag) == G_TYPE_STRING) 
        {
            if (!gst_tag_list_get_string_index (list, tag, i, &str))
            {
                GST_DEBUG_Player("Cannot get tag %s\n", tag);
            } 
        }
        else 
        {
            str = g_strdup_value_contents (
                    gst_tag_list_get_value_index (list, tag, i));
        }

        if (i == 0) 
        {
            GST_DEBUG_Player("%15s: %s\n", gst_tag_get_nick (tag), str);
        } 
        else 
        {
            GST_DEBUG_Player("               : %s\n", str);
        }
        g_free (str);
    }
}

// added for subtitle
 void GstPlayerPipeline::subtitle_signal_func(GObject * object, GObject * orig, 
    GParamSpec * pspec, gpointer user_data)
{

     GST_DEBUG_Player(" ***** subtitle_signal_func  be call*****\n");
}

GstPlayerPipeline::GstPlayerPipeline(GstPlayer* gstPlayer, int mediatype)
{
    GST_DEBUG_Player ("**** GstPlayerPipeline() Enter ****\n");
    int ret = 0;
    
    mGstPlayer = gstPlayer;

    // GstElement
    mPlayBin = NULL;
    mAudioSink = NULL;
    mVideoSink = NULL;
    mAppSource = NULL;

    mEqualizer = NULL;	
    mSpectrum = NULL;
    mConvert = NULL;
    mVol = NULL;
    audio_bin = NULL;

    // app source
    mFd = 0;
    mLength = 0;
    mOffset = 0;
    mMemBase = NULL;
    
    // mainloop
    mMainLoop = NULL;
    mBusMessageThread = NULL;

    // prepare
    mAsynchPreparePending = false;    

    // seek
    mSeeking = false;
    mSeekState = GST_STATE_VOID_PENDING;
    
    // others
    mIsLooping = false;
    m_speed = 0;//tom added 1231
    mPlayingState = mPlaying_No;
    mMediaType = mediatype;//
    mCompletedFlag = false;
    mPipe_init_state = 0;
    mHttpStreamFlag = 0;
#ifdef FF_FB_NEW_MODE    
     mFFFBLoopFlag = 0;// 1 -> run, 0 -> stop
     mFrameCount = 0;
     mPreFrameTime = 0;
#endif    
	mTrackFlag = 0;
    // subtitle
    mSubXpos = -1;
    mSubYpos = -1;
    mSubAlpha = 0;
    mSubDisplay = 0;    
    mSubWidth = 0;
    mSubHeight = 0;
    mSubFont = NULL;
    mSubFile = NULL;

    //video position
    mVideoPosX = 0;
    mVideoPosY=0;
    mVideoWidth = 0;
    mVideoHeigh = 0;
    mVideoStride = 0;
    
    // initialize gst framework
    init_gst();
	
#ifdef ANDROID	
    INIT_LOCK(&mActionMutex);
#else 
    mActionMutex = NULL;
    mActionMutex = g_mutex_new();
    
    //if(mActionMutex == NULL) GST_DEBUG_Player ("*** new mutex error ****\n");
#endif
    // initilize members
#ifdef ANDROID	    
    LOCK(&mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
#ifdef MEM_DEBUG
  gst_alloc_trace_set_flags_all (GST_ALLOC_TRACE_LIVE);
  if (!gst_alloc_trace_available ()) {
      g_print("Trace not available (recompile with trace enabled).\n");
  }
  else
      g_print("Trace is enabled\n");
#endif
    // create pipeline 
    ret = create_pipeline();
    if(ret==true)
        mPipe_init_state = 1;
     else
        mPipe_init_state = 0;
#ifdef MEM_DEBUG
  GST_DEBUG_Player ("**** after create_pipeline() ****\n");
  gst_alloc_trace_print_live ();
#endif
     
#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
    GST_DEBUG_Player ("**** GstPlayerPipeline() Leave ****\n");
    return;
}

GstPlayerPipeline::~GstPlayerPipeline()
{
    GST_DEBUG_Player ("Enter\n");

#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif

    delete_pipeline ();
#ifdef MEM_DEBUG
  GST_DEBUG_Player ("**** after delete_pipeline ****\n");
  gst_alloc_trace_print_live ();
#endif

    mGstPlayer = NULL;
    GST_DEBUG_Player (" **** free mutex ****\n");
 #ifdef ANDROID	    
    UNLOCK(&pGstPlayerPipeline->mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
    //DELETE_LOCK(&mActionMutex);
    g_mutex_free(mActionMutex);
    mActionMutex = NULL;
    
    GST_DEBUG_Player (" **** Leave ~GstPlayerPipeline ****\n");
}

gpointer
GstPlayerPipeline::do_loop (GstPlayerPipeline * ed)
{
  GST_DEBUG_Player ("enter main loop");
  g_main_loop_run (ed->mMainLoop);
  GST_DEBUG_Player ("exit main loop");

  return NULL;
}
gpointer
GstPlayerPipeline::fffb_loop (GstPlayerPipeline * ed)
{
  GST_DEBUG_Player ("enter fffb loop");
  while(ed->mFFFBTRUE)
  {
	usleep(450000);  
	if(ed->mFFFBLoopFlag ==1)
		FF_FB_Timer_callback(ed);
  }
  GST_DEBUG_Player ("exit fffb loop");
  return NULL;
}

// TODO: delete this API
// Player will delete playbin2 everytime, try no do that in the future
bool GstPlayerPipeline::create_pipeline ()
{
    GstBus *bus = NULL;

    // create playbin2
    mPlayBin = gst_element_factory_make ("playbin2", NULL);
    if(mPlayBin == NULL)
    {
        GST_DEBUG_Player("Failed to create playbin2\n");
        goto ERROR;
    }

    // add watch message
  /* create and start mainloop */
	GST_DEBUG_Player ("set up mainloop");
	mMainCtx = g_main_context_new ();
	mMainLoop = g_main_loop_new (mMainCtx, FALSE);
    if (mMainLoop == NULL) 
    {
        GST_DEBUG_Player ("Failed to create MainLoop.\n");
        goto ERROR;
    }	
	mBusMessageThread = g_thread_create ((GThreadFunc) do_loop, this, TRUE, NULL);
    if (mBusMessageThread == NULL)
    {
        GST_DEBUG_Player ("Failed to create Bus Message Thread.\n");
        goto ERROR;
    }
	mFFFBTRUE = 1;
	mFFFBThread = g_thread_create ((GThreadFunc) fffb_loop, this, TRUE, NULL);
    if (mFFFBThread == NULL)
    {
        GST_DEBUG_Player ("Failed to create FF FB Thread.\n");
        goto ERROR;
    }
	
  // register bus callback        
	bus = gst_pipeline_get_bus (GST_PIPELINE (mPlayBin));
  // add the pipeline bus to our custom mainloop
	mBusWatch = gst_bus_create_watch (bus);
	gst_object_unref (bus);
	g_source_set_callback (mBusWatch, (GSourceFunc) bus_callback, this, NULL);
	g_source_attach (mBusWatch, mMainCtx);

	
//    g_object_set (mPlayBin, "flags",0x00000067, NULL);
//   g_object_set (mPlayBin, "av-offset",500000000, NULL);
    // FIXME: after using fakesink, there is no unref() warning message, so
    // gstaudioflinger sink shall has some bugs
    //
    //
    // create audio & video sink elements for playbin2
    if(mMediaType == MediaType_Video)
    {
        system("reset_vpu.sh");
        
#ifdef SSL_GST_VPPINSINK
        g_object_set (mPlayBin, "flags",0x00000043, NULL);
#endif  //SSL_GST_VPPINSINK  
        
        mEqualizer	= gst_element_factory_make("equalizer-10bands", "equalizer");
	mVol 		= gst_element_factory_make("volume","vol");
	mAudioSink	= gst_element_factory_make("alsasink",NULL);
	g_object_set (G_OBJECT (mAudioSink), "sync", TRUE,NULL);
        g_object_set (mAudioSink, "buffer-time",((gint64)100000),NULL);
        
	audio_bin= gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(audio_bin),mEqualizer,/*mConvert,*/ /*mSpectrum,*/mVol, mAudioSink, NULL);
	gst_element_link_many(mEqualizer,/*mConvert,*//*mSpectrum,*/mVol,mAudioSink,NULL);
	pad1 = gst_element_get_static_pad(mEqualizer/*mSpectrum*/, "sink");
	gst_element_add_pad(audio_bin, gst_ghost_pad_new(NULL,pad1));
	g_object_set(G_OBJECT(mPlayBin), "audio-sink", audio_bin, NULL);
	//g_object_set (G_OBJECT (mSpectrum), "bands", spect_bands, "threshold", -80, "message", TRUE, "message-phase", TRUE, NULL);
	eq_set_equalizer(0);
        g_object_set (G_OBJECT(mAudioSink), "drift-tolerance", 100000, NULL);// 
        
       

    GST_DEBUG_Player ("**** create surfaceflingersink **** \n");
    mVideoSink = gst_element_factory_make("surfaceflingersink", NULL);//subtitlesink surfaceflingersink ximagesink
    if (mVideoSink == NULL)
    {
        GST_DEBUG_Player ("Failed to create surfaceflingersink\n");
        goto ERROR;
    }
     g_object_set (mPlayBin, "video-sink", mVideoSink, NULL);
#ifdef SSL_GST_VPPINSINK
//    GST_DEBUG_Player ("**** create sslsubtitle **** \n");
//    mTextSink = gst_element_factory_make("sslsubtitle", NULL);//subtitlesink surfaceflingersink ximagesink
//    if (mTextSink == NULL)
//    {
//        GST_DEBUG_Player ("Failed to create subtitlesink\n");
//        goto ERROR;
//    }
//     //g_object_set (G_OBJECT (mTextSink), "show-preroll-frame",FALSE , NULL);
//     g_object_set (mPlayBin, "text-sink", mTextSink, NULL);

#if 1
     //kahho:: 12-Oct-11, tuning
#ifndef SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS
#define SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS  (30)
#endif  //SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS
     g_object_set (G_OBJECT (mVideoSink), "max-lateness", SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS*1000000, NULL);  //kahho:: gstvideosink default is 20000000
#endif
#else   //SSL_GST_VPPINSINK
     g_object_set (G_OBJECT (mVideoSink), "async", TRUE, NULL);
#endif  //SSL_GST_VPPINSINK
    }
    else if(mMediaType == MediaType_Audio)
    {
        g_object_set (mPlayBin, "flags",0x00000002, NULL);
     // add spectrum && equalizer element for lorrylv 
        mEqualizer	= gst_element_factory_make("equalizer-10bands", "equalizer");
	//mConvert	= gst_element_factory_make("audioconvert", "convert");
	//mSpectrum	= gst_element_factory_make ("spectrum", "spectrum");
	mVol 		= gst_element_factory_make("volume","vol");
	mAudioSink	= gst_element_factory_make("alsasink",NULL);
	g_object_set (G_OBJECT (mAudioSink), "sync", TRUE,NULL);
        g_object_set (mAudioSink, "buffer-time",((gint64)100000),NULL);
	//g_object_set (G_OBJECT(mAudioSink), "buffer-time", 1000000, NULL);
       //g_object_set (G_OBJECT(mAudioSink), "latency-time", 100000, NULL);//"sync", false, NULL );
        
	audio_bin= gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(audio_bin),mEqualizer,/*mConvert,*/ /*mSpectrum,*/mVol, mAudioSink, NULL);
	gst_element_link_many(mEqualizer,/*mConvert,*//*mSpectrum,*/mVol,mAudioSink,NULL);
	pad1 = gst_element_get_static_pad(mEqualizer/*mSpectrum*/, "sink");
	gst_element_add_pad(audio_bin, gst_ghost_pad_new(NULL,pad1));
	g_object_set(G_OBJECT(mPlayBin), "audio-sink", audio_bin, NULL);
	//g_object_set (G_OBJECT (mSpectrum), "bands", spect_bands, "threshold", -80, "message", TRUE, "message-phase", TRUE, NULL);
	eq_set_equalizer(0);
        g_object_set (G_OBJECT(mAudioSink), "drift-tolerance", 100000, NULL);// 
       //GST_DEBUG_Player("gst element has mEqualizer....,mSpectrum , mAudioSink,\n");
	//g_object_set(G_OBJECT(mVol), "volume", 1, NULL);
    
    }
    else   if(mMediaType == MediaType_Photo)
    {
        GST_DEBUG_Player ("**** create photo  **** \n");
        mAudioSink = gst_element_factory_make("alsasink", NULL);//alsasink
        if (mAudioSink == NULL)
        {
            GST_DEBUG_Player ("Failed to create alsasink\n");
            goto ERROR;
        }
        g_object_set (G_OBJECT(mAudioSink), "buffer-time", 600000, NULL);
        g_object_set (G_OBJECT(mAudioSink), "latency-time", 40000, NULL);//"sync", false, NULL );
        g_object_set (G_OBJECT (mAudioSink), "sync", FALSE, NULL);
        g_object_set (mPlayBin, "audio-sink", mAudioSink, NULL);
    }
    else
    {
            GST_DEBUG_Player ("****mMediaType error, Failed to create pipline\n");
            goto ERROR;
    }

     //g_object_set (G_OBJECT (mVideoSink), "preroll-queue-len", 10, NULL);
     //g_object_set (mPlayBin, "max-size-time",2000000000, NULL);
    // g_object_set (G_OBJECT (mVideoSink), "qos", FALSE, NULL);
    /*    
    {
        int tag = 0;
        g_object_get (mPlayBin, "flags", &tag, NULL);
        GST_DEBUG_Player ("**** playbin2 tag is %d ****\n", tag);
        tag &= 0xfffd;//clear the audio tag
        g_object_set (mPlayBin, "flags", tag, NULL);
    }
    */
    gst_element_set_state (mPlayBin, GST_STATE_NULL);	
    GST_DEBUG_Player ("Pipeline is created successfully\n");
    return true;

ERROR:
    GST_DEBUG_Player ("Pipeline is created failed\n");
    // release resource
    return false;
}

void  GstPlayerPipeline::delete_pipeline ()
{
    // release pipeline & main loop
    GST_DEBUG_Player ("****Release  pepeline loop ***\n");
    if (mPlayBin) 
    {
        // send exit message to pipeline bus post an application specific 
        // message unlock here for bus_callback to process this message
#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
	if(mFFFBThread)
        {
            GST_DEBUG_Player ("LET fffb to exit\n");
			mFFFBTRUE = 0;
            /* Wait for main loop to quit*/
            GST_DEBUG_Player ("Wait for mFFFBThread loop to quit ...\n");
            g_thread_join (mFFFBThread);
            GST_DEBUG_Player("mFFFBThread joins successfully\n");
            mFFFBThread = NULL;
        } 
        else 
        {
            GST_DEBUG_Player("mFFFBThread is NULL, no need to quit\n");
        }

#if 0
	if(mBusMessageThread)
        {
            GST_DEBUG_Player ("Send QUIT_MAINLOOP message to bus\n");
            gst_element_post_message (
                GST_ELEMENT (mPlayBin),
                gst_message_new_application (
                    GST_OBJECT (mPlayBin),
                    gst_structure_new (MSG_QUIT_MAINLOOP,
                    "message", G_TYPE_STRING, "Deleting Pipeline", NULL)));

            /* Wait for main loop to quit*/
            GST_DEBUG_Player ("Wait for main loop to quit ...\n");
            g_thread_join (mBusMessageThread);
            GST_DEBUG_Player("BusMessageThread joins successfully\n");
            mBusMessageThread = NULL;
        } 
        else 
        {
            GST_DEBUG_Player("BusMessageThread is NULL, no need to quit\n");
        }
#endif
#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif

		GstBus *bus;
		bus = gst_pipeline_get_bus (GST_PIPELINE (mPlayBin));
		GST_DEBUG_Player ("flush bus messages");
		if (bus != NULL) {
			gst_bus_set_flushing (bus, TRUE);
			gst_object_unref (bus);
		}

        GST_DEBUG_Player ("One pipeline exist, now delete it .\n");
        gst_element_set_state (mPlayBin, GST_STATE_NULL); //tom 1224 for test
    }

    // release audio & video sink
/*#if 1    
	if (mAudioSink)
    {
        GST_DEBUG_Player ("****Release audio sink\n");
        gst_object_unref (mAudioSink);
        mAudioSink = NULL;
    }
#endif
    if (mVideoSink)
    {
        GST_DEBUG_Player ("****Release video sink\n");
        gst_object_unref (mVideoSink);
        mVideoSink = NULL;
    }
*/    
    if (mAppSource)
    {
        GST_DEBUG_Player ("****Release app source\n");
        gst_object_unref (mAppSource);
        mAppSource = NULL;
    }
    if(mPlayBin)
    {   
        GST_DEBUG_Player ("***Delete playbin2\n");
        gst_object_unref (mPlayBin);
        mPlayBin = NULL;
    }
    if (mMainLoop)
    {

	GST_DEBUG_Player ("***Delete mainloop\n");
    g_source_destroy (mBusWatch);
	g_source_unref(mBusWatch);
	mBusWatch = NULL;
    g_main_loop_quit (mMainLoop);
    g_thread_join (mBusMessageThread);
	mBusMessageThread = NULL;
	
	GST_DEBUG_Player ("***Delete mainloop2\n");
    g_main_loop_unref (mMainLoop);
	mMainLoop = NULL;
    g_main_context_unref (mMainCtx);
	mMainCtx = NULL;
	GST_DEBUG_Player ("***Delete mainloop3\n");
			
    } 
    if (mMemBase)
    {
        GST_DEBUG_Player ("***Unmap fd\n");
        munmap((void*)mMemBase, (size_t)mLength);
    }

    // app source
    mFd = 0;
    mLength = 0;
    mOffset = 0;
    mMemBase = NULL;
    // seek
    mSeeking = false;
    mSeekState = GST_STATE_VOID_PENDING;
    // prepare
    mAsynchPreparePending = false;
    // subtitle
    mSubXpos=-1;
    mSubYpos=-1;
    mSubAlpha=0;
    mSubDisplay=0;
    mSubWidth=0;
    mSubHeight=0;
    if(mSubFont)
    	g_free(mSubFont);
    mSubFont=NULL;
    if(mSubFile)
       g_free(mSubFile);
    mSubFile=NULL;

    GST_DEBUG_Player ("**** Leave delete pipeling ****\n");
}

bool GstPlayerPipeline::setDataSource(const char *url)
{
    GST_DEBUG_Player("Enter, url=%s\n",url);
    mHttpStreamFlag = 0;
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    if (mPlayBin == NULL)
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        return false;
    }
    // set uri to playbin2 
    gchar* full_url = NULL;
	if( strstr (url,".ts") || strstr (url,".TS")|| strstr (url,".VOB")|| strstr (url,".vob") 
		|| strstr (url,".DAT"))
	{
		mTrackFlag = 1;
	}
    if(url[0] == '/')
    {
        // url is an absolute path, add prefix "file://"
        int len = strlen(url) + 10;
        GST_DEBUG_Player ("*** file len is %d ****\n", len);
        full_url = (gchar*)g_malloc(len);//malloc tom 110105
        if(full_url == NULL)
            return false;
        g_strlcpy(full_url, "file://", len);
        g_strlcat(full_url, url, len);
    }
    else if (g_str_has_prefix(url, "file:///"))
    {
        full_url = g_strdup(url);
        if(full_url == NULL)
            return false;        
    }
	else if ((g_str_has_prefix(url, "http://"))|| (g_str_has_prefix(url, "rtsp://")))
	{
		mHttpStreamFlag = 1;
		full_url = g_strdup(url);
		if(full_url == NULL)
		    return false;
		g_object_set (G_OBJECT (mPlayBin), "buffer-duration", 60 * GST_SECOND,(gchar *) NULL);
		g_object_set (G_OBJECT (mPlayBin), "buffer-size", (gint) 1024 * 1024,(gchar *) NULL);
	}
    else
    {
        GST_DEBUG_Player("Invalide url.\n");
        return false;
    }
    GST_DEBUG_Player("playbin2 uri: %s\n", full_url);
    g_object_set (mPlayBin, "uri", full_url, NULL);
    
    if(full_url)
        g_free (full_url);// free tom 110105

#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
    return true;
}

bool GstPlayerPipeline::setDataSource(int fd, int64_t offset, int64_t length)
{
    // URI: appsrc://...  
    //
    // Example:
    // external/gst-plugins-base/tests/examples/app/appsrc-stream2.c
    // gst-plugins-base/tests/examples/app/appsrc-ra.c
    if (mPlayBin == NULL)
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        return false;
    }  
    // TODO: reset player here
    if (fd == 0)
    {
        GST_DEBUG_Player ("Invalid fd: %d\n", fd);
        return false;
    }
    struct stat stat_buf;
    if( fstat(fd, &stat_buf) != 0)
    {
        GST_DEBUG_Player ("Cannot get file size\n");
        return false;
    }
    mFd = fd;
    mLength = stat_buf.st_size; 
    mOffset = 0;
    
     if (mMemBase) // tom 110111 added 
    {
        GST_DEBUG_Player ("***Unmap fd\n");
        munmap((void*)mMemBase, (size_t)mLength);
        mMemBase = NULL;
    }
    // map the file into memory  
    mMemBase = (guint8*)mmap(0, (size_t)mLength, PROT_READ, MAP_PRIVATE, fd, 0);
    if(mMemBase == NULL)
    {
		GST_DEBUG_Player("*** mmap mem error and exit ****\n");
		return false;
    }
    GST_DEBUG_Player("playbin2 uri: appsrc://, fd: %d, length: %lu, mMemBase: %p\n", 
            mFd, (unsigned long int)mLength, mMemBase);

    // use appsrc in playbin2
    g_object_set (mPlayBin, "uri", "appsrc://", NULL);

    // get notification when the source is created so that we get a handle to
    // it and can configure it
    g_signal_connect (mPlayBin, "deep-notify::source", (GCallback)
            playbin2_found_source, this);

    return true;
}
#ifdef ANDROID 	
bool GstPlayerPipeline::setAudioSink(sp<MediaPlayerInterface::AudioSink> audiosink)
{
    LOCK (&mActionMutex);    
    if(audiosink == 0)
    {
        GST_DEBUG_Player("Error audio sink %p\n", audiosink.get());
        return false;
    }

    if (!mPlayBin) 
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        return false;
    }

    if(mAudioOut != 0)
    {
        mAudioOut.clear();
        mAudioOut = 0;
    }
    mAudioOut = audiosink;

    // set AudioSink
    GST_DEBUG_Player("MediaPlayerInterface::AudioSink: %p\n", mAudioOut.get());
    g_object_set (mAudioSink, "audiosink", mAudioOut.get(), NULL);

    UNLOCK (&mActionMutex);
    return true; 
}

bool GstPlayerPipeline::setVideoSurface(const sp<ISurface>& surface)
{
    LOCK (&mActionMutex);
    if(surface == 0)
    {
        GST_DEBUG_Player("Error ISurface %p\n", surface.get());
        return false;
    }

    if (!mPlayBin) 
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        return false;
    }  
    // set ISurface
    GST_DEBUG_Player("ISurface: %p\n", surface.get());
    g_object_set (mVideoSink, "surface", surface.get(), NULL);

    UNLOCK (&mActionMutex);

    return false;
}
#else
bool GstPlayerPipeline::setAudioSink(/* maybe need to add param*/ )
{


	return true;
}
bool GstPlayerPipeline::setVideoSurface(/* maybe need to add param*/)
{


	return true;
}
#endif

#ifdef FF_FB_NEW_MODE
gboolean  GstPlayerPipeline::FF_FB_Timer_callback (gpointer user_data)
{

   bool ret = false;
   int curtime=0,total_time=0,temp=0; //ms
   GstPlayerPipeline *player_pipe = (GstPlayerPipeline *)user_data;
   
   // struct timeval sstimer, ctimer;//sys start time ,current time
  // gettimeofday(&ctimer,NULL); 
  // GST_DEBUG_Player("*** FF_FB_Timer_callback,systime tv_sec:%d,tv_usec:%d ***\n",
  //     ctimer.tv_sec , ctimer.tv_usec);
   /*GST_DEBUG_Player("*** FF_FB_Timer_callback ***\n");*/
     
    if(!player_pipe)
    {
        GST_DEBUG_Player("*** player_pipe is null, and exit***\n");
        return false;
    }
    if(player_pipe->mCompletedFlag || (!player_pipe->mFFFBLoopFlag))
    {
             GST_DEBUG_Player("*** video ff fb exit ***\n");
             goto timer_exit;
    }        
    if(! player_pipe->getDuration(&total_time))
    {
             GST_DEBUG_Player("*** getDuration error and return ***\n");
             goto timer_exit;
    }

    GST_DEBUG_Player("*** ff speed is %f***\n", player_pipe->m_speed);
    
    //  do seek operation
    // flag (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT|GST_SEEK_FLAG_SKIP)
    curtime = 0;
    if(player_pipe->getCurrentPosition(&curtime))
    {
        GST_DEBUG_Player("*** get cur pos  is %d, total time is %d, mPreFrameTime  is %d***\n" ,curtime ,total_time,player_pipe->mPreFrameTime );// next timer is ms
        if(player_pipe->mPreFrameTime  >= (total_time -player_pipe->m_speed*1000) && (player_pipe->m_speed > 0 ))  
         {
            GST_DEBUG_Player("*** ff to end position ,and exit***\n");
            temp = total_time;
            ret =gst_element_seek (player_pipe->mPlayBin, 1.0, GST_FORMAT_TIME,  (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT), //|GST_SEEK_FLAG_KEY_UNIT
                   GST_SEEK_TYPE_SET, temp*GST_MSECOND, GST_SEEK_TYPE_NONE, (total_time*GST_MSECOND));
            player_pipe->mGstPlayer->sendEvent(MEDIA_FF_COMPLETE);
            goto timer_exit;; //goto FF_exit; pause the ff/fb
         }
        else if(player_pipe->mPreFrameTime  < (-player_pipe->m_speed*1000) && player_pipe->m_speed < 0 )
        {
            GST_DEBUG_Player("*** fb to start , exit***\n");
            temp = 0;
            ret =gst_element_seek (player_pipe->mPlayBin, 1.0, GST_FORMAT_TIME,  (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT), //|GST_SEEK_FLAG_KEY_UNIT
                   GST_SEEK_TYPE_SET, temp*GST_MSECOND, GST_SEEK_TYPE_NONE, (total_time*GST_MSECOND));
               player_pipe->mGstPlayer->sendEvent(MEDIA_FB_COMPLETE);
            goto timer_exit;; //goto FF_exit; pause the ff/fb
        }

        temp = player_pipe->mPreFrameTime + player_pipe->m_speed*1000;//  ms 
        player_pipe->mPreFrameTime  = temp;
        ret =gst_element_seek (player_pipe->mPlayBin, 1.0, GST_FORMAT_TIME,  (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT), //|GST_SEEK_FLAG_KEY_UNIT
                       GST_SEEK_TYPE_SET, temp*GST_MSECOND, GST_SEEK_TYPE_NONE, (total_time*GST_MSECOND));
        //ret = player_pipe->seekTo(temp);
        if(ret)
        {
            player_pipe->mFrameCount++;
            GST_DEBUG_Player("*** count  is %d***\n",player_pipe->mFrameCount);
        }
        else
        {
            GST_DEBUG_Player("*** FF or FB seekto error and exit***\n");
        }
  
   }
   else
   {
        GST_DEBUG_Player("*** get cur pos  error and exit ***\n");
   }
        
    return true;
 timer_exit:
     player_pipe->mFrameCount = 0;
     player_pipe->mFFFBLoopFlag = 0;
    return false;
}
#endif
bool GstPlayerPipeline::prepare()
{
    bool ret = false;
    int subtitle_stream;
    GstStateChangeReturn state_return;
       
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    if (!mPlayBin) 
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }  
        
    GST_DEBUG_Player("**** prepare Enter****\n"); 
    state_return = gst_element_set_state (mPlayBin, GST_STATE_PAUSED);
    GST_DEBUG_Player("state_return = %d\n", state_return);
    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        GST_DEBUG_Player ("Fail to set pipeline to PAUSED\n");
        goto EXIT;
    }
    else if (state_return == GST_STATE_CHANGE_ASYNC)
    {
        GstState state;

        // wait for state change complete
        GST_DEBUG_Player("Wait for pipeline's state change to PAUSE...\n");
        state_return = gst_element_get_state (mPlayBin, &state, NULL, GST_SECOND*(GET_STATE_DELAY + 5));//GST_CLOCK_TIME_NONE
        GST_DEBUG_Player("Pipeline's state change to PAUSE\n");
        if (state_return != GST_STATE_CHANGE_SUCCESS || state != GST_STATE_PAUSED ) 
        {
            GST_DEBUG_Player ("Fail to set pipeline to PAUSED\n");
            goto EXIT;
        }
    }

    // send prepare done message

        // get video size

    
//    g_object_get( mPlayBin, "n-text", &subtitle_stream, NULL);// tom 110226 for subtitle
//    GST_DEBUG_Player ("****subtitle_Status  %d****\n", subtitle_stream);
//    if(subtitle_stream>0)
//    {
//        GST_DEBUG_Player("Configure subtitle x %d y %d alpha %d display %d\n", mSubXpos, mSubYpos, mSubAlpha, mSubDisplay);
//        if(mSubXpos>0&&mSubXpos+mSubWidth<=MAIN_WIN_WIDTH)
//            g_object_set (mPlayBin, "fw-xpos", mSubXpos, NULL);
//        if(mSubYpos>0&&mSubYpos+mSubHeight<=MAIN_WIN_HEIGHT)
//            g_object_set (mPlayBin, "fw-ypos", mSubYpos, NULL);
//        if(mSubWidth<=MAX_SUBTITLE_WIDTH&&mSubWidth>0)
//            g_object_set (mPlayBin, "fw-width", mSubWidth, NULL);
//        if(mSubHeight<=MAX_SUBTITLE_HEIGHT&&mSubHeight>0)
//            g_object_set (mPlayBin, "fw-height", mSubHeight, NULL);
//        if(mSubAlpha>0&&mSubAlpha<=255)
//            g_object_set (mPlayBin, "fw-alpha", mSubAlpha, NULL);
//        g_object_set (mPlayBin, "fw-display", mSubDisplay, NULL);
//        if(mSubFont!=NULL)
//            g_object_set (mPlayBin, "subtitle-font-desc", mSubFont, NULL );
//    }
#if 0
{	
    GstFormat  fmt = GST_FORMAT_TIME;
    gint64  dur;
    GstFormat format = GST_FORMAT_TIME;
    int tt = 0;
    GST_DEBUG_Player("getDuration enter \n"); 
    if (!mPlayBin) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }
    mDruation = 0;	
    tt = gst_element_query_duration(mPlayBin, &format, &dur);
    if(  tt != TRUE || format != GST_FORMAT_TIME)
    {
        GST_DEBUG_Player ("Fail to get duration\n");
        mDruation = -1000;// return -100 to application
    }
    else
    {
        mDruation = (int)(dur / GST_MSECOND);
        if((mDruation) < 4 *1000)
        {
            GST_DEBUG_Player ("**** get duration is invalid, msec is %d ****\n", (mDruation));
            (mDruation) = -1000; 
        }
        else
        {
        }
    }
}	
usleep(500000);	
#endif
    if (mGstPlayer)
    {
//        int width = 0;
//        int height = 0;
//        if (getVideoSize(&width, &height))
//        {
//            GST_DEBUG_Player("Send MEDIA_SET_VIDEO_SIZE, width (%d), height (%d)\n", width, height);
//            mGstPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, width, height);
//        }
        mGstPlayer->sendEvent(MEDIA_PREPARED);
    }	
    ret = true;
EXIT:
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::prepareAsync()
{
    bool ret = false;
    GstStateChangeReturn state_return;
       
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif

    if (!mPlayBin) 
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }  
    
    GST_DEBUG_Player("Enter\n"); 
    state_return = gst_element_set_state (mPlayBin, GST_STATE_PAUSED);
    GST_DEBUG_Player("state_return = %d\n", state_return);
    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        GST_DEBUG_Player ("Fail to set pipeline to PAUSED\n");
        goto EXIT;
    }

    // wait for state change complete in message loop
    mAsynchPreparePending = true;
    GST_DEBUG_Player("Wait for pipeline's state change to PAUSE...\n");
    
    ret = true;
EXIT:
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}


bool GstPlayerPipeline::start()
{
    bool ret = false;
    GstStateChangeReturn state_return;
    int nvideo_tag =0, naudio_tag;
       
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    if (!mPlayBin) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }  
    mCompletedFlag = false;
#ifdef FF_FB_NEW_MODE        
    mFFFBLoopFlag = 0;
#endif
#ifdef MEM_DEBUG
  GST_DEBUG_Player ("**** befort start() ****\n");
  gst_alloc_trace_print_live ();
#endif
    GST_DEBUG_Player("**** start  Enter 111****\n");
#ifndef TWOINONE	
    if((mMediaType == MediaType_Video)&&(!mHttpStreamFlag))
    {
        g_object_get(G_OBJECT(mPlayBin), "n-video", &nvideo_tag, NULL);
        g_object_get(G_OBJECT(mPlayBin), "n-audio", &naudio_tag, NULL);
        //GST_DEBUG_Player("**** nvideo streams is %d ****\n", nvideo_tag); 
        //GST_DEBUG_Player("**** naudio streams is %d ****\n", naudio_tag); 
        if(nvideo_tag <= 0)
        {
            GST_DEBUG_Player("**** nvideo streams is %d, no video to play and exit  ****\n", nvideo_tag); 
            goto EXIT;
        }
    }
#endif    

#ifdef SSL_GST_PORT_VQ
    sslUpdateAVqueue(mPlayBin);
#endif  //SSL_GST_PORT_VQ

    state_return = gst_element_set_state (mPlayBin, GST_STATE_PLAYING);
    GST_DEBUG_Player("state_return = %d\n", state_return);

    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        GST_DEBUG_Player ("Fail to set pipeline to PLAYING\n");
        goto EXIT;
    }
    else if (state_return == GST_STATE_CHANGE_ASYNC)
    {
        GstState state;

        // wait for state change complete
        GST_DEBUG_Player("Wait for pipeline's state change to PLAYING...\n");
        state_return = gst_element_get_state (mPlayBin, &state, NULL, GST_SECOND*GET_STATE_DELAY);//GST_CLOCK_TIME_NONE
        GST_DEBUG_Player("Pipeline's state change to PLAYING\n");
        if (state_return != GST_STATE_CHANGE_SUCCESS || state != GST_STATE_PLAYING ) 
        {
            GST_DEBUG_Player ("Fail to set pipeline to PLAYING, state_return is %d\n", state_return);
            goto EXIT;
        }
    }    
    
    mPlayingState = mPlaying_is;
    ret = true;

EXIT:
	usleep(200000);
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::stop()
{
    bool ret = false;
    GstStateChangeReturn state_return;

    GST_DEBUG_Player("*** gstplayer stop Enter  ****\n"); 
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
#if 1
    if (!mPlayBin) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }  
#ifdef FF_FB_NEW_MODE        
    mFFFBLoopFlag = 0;
#endif

    state_return = gst_element_set_state (mPlayBin, GST_STATE_READY);
    GST_DEBUG_Player("**** set GST_STATE_READY state_return = %d ***\n", state_return);

    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        GST_DEBUG_Player ("Fail to set pipeline to PLAYING\n");
        goto EXIT;
    }
    else if (state_return == GST_STATE_CHANGE_ASYNC)
    {
        GstState state;

        // wait for state change complete
        GST_DEBUG_Player("Wait for pipeline's state change to READY...\n");
        state_return = gst_element_get_state (mPlayBin, &state, NULL, GST_SECOND*GET_STATE_DELAY);//GST_CLOCK_TIME_NONE
        GST_DEBUG_Player("Pipeline's state change to READY\n");
        if (state_return != GST_STATE_CHANGE_SUCCESS || state != GST_STATE_READY ) 
        {
            GST_DEBUG_Player ("Fail to set pipeline to READY\n");
            goto EXIT;
        }
    }    
#endif

    // seek
    if(mSeeking)
    {

         GST_DEBUG_Player("Stop playback while seeking. Send MEDIA_SEEK_COMPLETE immediately\n");
         if(mGstPlayer)
             mGstPlayer->sendEvent(MEDIA_SEEK_COMPLETE);
    }
    mSeeking = false;
    
    mSeekState = GST_STATE_VOID_PENDING;

    // prepare
    if(mAsynchPreparePending)
    {
        GST_DEBUG_Player("mAsynchPreparePending shall not be true!\n");
    }
    mAsynchPreparePending = false;

    ret = true;
    mPlayingState = mPlaying_stop;
#ifdef MEM_DEBUG
  GST_DEBUG_Player ("**** after stop() ****\n");
  gst_alloc_trace_print_live ();
#endif
EXIT:
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif

return ret;
}

bool  GstPlayerPipeline::pause()
{
    bool ret = false;
    int pause_fail_tag=0;
    GstStateChangeReturn state_return;
       
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    if (!mPlayBin) 
    {
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }  
    GST_DEBUG_Player("*** pause Enter ****\n"); 

#ifdef FF_FB_NEW_MODE        
    mFFFBLoopFlag = 0;
#endif
    /*if(mSeeking)
    {
        GST_DEBUG_Player("Pause in seeking, switch seek pending state to pause\n"); 
        mSeekState = GST_STATE_PAUSED;
    }*/

    state_return = gst_element_set_state (mPlayBin, GST_STATE_PAUSED);
    GST_DEBUG_Player("state_return = %d\n", state_return);
    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        GST_DEBUG_Player ("Fail to set pipeline to PAUSED\n");
        goto EXIT;
    }
    else if (state_return == GST_STATE_CHANGE_ASYNC)
    {
        GstState state;
       
        // wait for state change complete
        GST_DEBUG_Player("Wait for pipeline's state change to PAUSE...\n");
        state_return = gst_element_get_state (mPlayBin, &state, NULL, GST_SECOND*GET_STATE_DELAY);//GST_CLOCK_TIME_NONE
        GST_DEBUG_Player("Pipeline's state change to PAUSE\n");
        if (state_return != GST_STATE_CHANGE_SUCCESS || state != GST_STATE_PAUSED ) 
        {
            GST_DEBUG_Player ("Fail to set pipeline to PAUSE, state_return is %d\n", state_return);
            pause_fail_tag = 1;
            goto EXIT;
        }
    }
    ret = true;
    mPlayingState = mPlaying_pause;
EXIT:
	usleep(200000);
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
    if(pause_fail_tag)//ret == false
    {
        GST_DEBUG_Player ("**** pause time out  error ,reset it  ****\n");
        int curpos = 0;
        getCurrentPosition(&curpos);
        stop();
        prepare();
        seekTo(curpos);
    }
return ret;
}

bool GstPlayerPipeline::isPlaying()
{
    GstState state, pending;
    bool ret = false;
    if(mPlayingState ==mPlaying_is)
	{
    		//usleep(2000000);
    		//GST_DEBUG_Player("*** exit isPlaying() ***\n");
		return true;
	}
	else 
	{	
		//GST_DEBUG_Player("*** exit isPlaying() ***\n");
		return false;
	}
#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif    

    if (!mPlayBin) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }

    gst_element_get_state (mPlayBin, &state, &pending,  GST_SECOND*GET_STATE_DELAY);//GST_CLOCK_TIME_NONE
   
    //GST_DEBUG_Player("state: %d, pending: %d, seeking: %d", state, pending, mSeeking); 

    // we shall return true if, we are playing or we are seeking in playback
    if (state == GST_STATE_PLAYING || 
            ( mSeeking == true && mSeekState == GST_STATE_PLAYING))
        ret = true;

    //GST_DEBUG_Player("playing: %d", ret); 
    //if(ret)
    //    mPlayingState = mPlaying_is;
   
EXIT:    
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::getVideoSize(int *w, int *h)
{
    bool ret = false;
    GstPad* pad = NULL;
    GstStructure* struc = NULL;
//    GList *pads;

    if (!mPlayBin || !mVideoSink) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }
    
    pad = gst_element_get_static_pad(mVideoSink, "sink");
    if (pad == NULL)
    {
        GST_DEBUG_Player ("Cannot get sink pad from video sink\n");
        goto EXIT;
    }

    struc = gst_caps_get_structure(GST_PAD_CAPS(pad), 0);
    if (struc == NULL)
    {
        GST_DEBUG_Player ("Cannot get structure from video sink\n");
        goto EXIT;
    }        
    GST_DEBUG_Player("Sink caps: %s\n", gst_caps_to_string(GST_PAD_CAPS(pad)));

    if (gst_structure_get_int(struc, "width", w) != TRUE)
    {
        GST_DEBUG_Player ("Cannot get width from caps\n");
        *w = *h = 0;
        goto EXIT;
    }
    if (gst_structure_get_int(struc, "height", h) != TRUE)
    {
        GST_DEBUG_Player ("Cannot get width from caps\n");
        *w = *h = 0;
        goto EXIT;
    }
    GST_DEBUG_Player("width: %d, height: %d\n", *w, *h);
    
    ret = true; 


EXIT:    
    if(pad)
        gst_object_unref(pad);

    return ret;    
}


bool  GstPlayerPipeline::seekTo(int msec)
{
    GstState state, pending;
    bool ret = false;

    gint64 seek_pos = (gint64)msec * GST_MSECOND;
       
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
if (!mPlayBin) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }

    GST_DEBUG_Player("Enter, seek to: %d\n", msec);

    // get current stable state
    gst_element_get_state (mPlayBin, &state, &pending, GST_SECOND*GET_STATE_DELAY);//GST_CLOCK_TIME_NONE
    GST_DEBUG_Player("state: %d, pending: %d\n", state, pending); 

    if (gst_element_seek_simple(mPlayBin, GST_FORMAT_TIME, 
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP), seek_pos) != TRUE)
    {
        GST_DEBUG_Player ("Fail to seek to position %d\n", msec);
    }
    else
    {
        GST_DEBUG_Player ("Seek to %d\n", msec);
        mSeeking = true;
        mSeekState = state;
        ret = true;
        //mPlayingState = mPlaying_seek;
    }

EXIT:    
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::getCurrentPosition(int *msec)
{
    //GstFormat  fmt = GST_FORMAT_TIME;
    gint64  pos;
    bool ret = false;
    GstFormat format = GST_FORMAT_TIME;
    
   // GST_DEBUG_Player("enter"); 

 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    *msec = 0;
    if (!mPlayBin || msec == NULL) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }
    if (gst_element_query_position(mPlayBin, &format, &pos) != TRUE 
            || format != GST_FORMAT_TIME)
        GST_DEBUG_Player ("Fail to get current position\n");
    else
    {
        *msec = (int)(pos / GST_MSECOND);
     //   printf("Current position: %d\n", *msec);
        ret = true;
    }
#ifdef MEM_DEBUG	
//    GST_DEBUG_Player ("**** getCurrentPosition is %d ****\n",  *msec);
#endif

EXIT:    
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::getDuration(int *msec)
{  
//    GstFormat  fmt = GST_FORMAT_TIME;
    gint64  dur;
    bool ret = true;
    GstFormat format = GST_FORMAT_TIME;
    int tt = 0;
    //GST_DEBUG_Player("getDuration enter \n"); 
  
 #ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif

    *msec = 0;
    if (!mPlayBin || msec == NULL) 
    { 
        GST_DEBUG_Player ("Pipeline not initialized\n");
        goto EXIT;
    }
	// *msec = mDruation;
#if 1
     tt = gst_element_query_duration(mPlayBin, &format, &dur);
    //GST_DEBUG_Player ("**** tt is %d, format is %d****\n", tt, format);
    if(  tt != TRUE 
            || format != GST_FORMAT_TIME)
   {
        GST_DEBUG_Player ("Fail to get duration\n");
        (*msec) = -1000;// return -100 to application
    }
    else
    {
        *msec = (int)(dur / GST_MSECOND);
//        printf("Duration: %d\n", *msec);
        if((*msec) < 4 *1000)
        {
            GST_DEBUG_Player ("**** get duration is invalid, msec is %d ****\n", (*msec));
            (*msec) = -1000; 
            ret = false;
        }
        else
        {
            ret = true;
        }
    }
#endif

EXIT:    
    //GST_DEBUG_Player("getDuration exit \n");
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::reset()
{
    GST_DEBUG_Player ("Enter\n");
    
    // stop playback
    stop();
    return true;
}

bool GstPlayerPipeline::setLooping(int loop)
{
    GST_DEBUG_Player  ("setLooping (%s)\n", (loop!=0)?"TRUE":"FALSE");
#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
	mIsLooping = (loop != 0) ? true : false;
#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
    return true;
}

//tom1224 added 
bool GstPlayerPipeline::FastForward(int front_state)
{

#ifdef  FF_FB_NEW_MODE
//     bool ret = false;
    int curtime=0;
    // 1 pause the video
    if(front_state !=FF_normal && (!mFFFBLoopFlag))
    {
        pause();
    }
    // 2 set the ff switch time
    switch(front_state)
    {
        case FF_normal:
            start();
            mFFFBLoopFlag = 0;// switch 0.1s
            return true;
           
        case FF_Speed1:
            m_speed = FF_FB_LEVEL1_TIMER;// switch 0.2s
            break;
        case FF_Speed2:
            m_speed = FF_FB_LEVEL2_TIMER;// switch 0.4s
            break;
        case FF_Speed3:
            m_speed = FF_FB_LEVEL3_TIMER;// switch 0.8s
            break;
        case FF_Speed4:
            m_speed = FF_FB_LEVEL4_TIMER;// switch 1.6s
            break;
        case FF_Speed5:
            m_speed = FF_FB_LEVEL5_TIMER;// switch 3.2s
            break;            
        default :
            m_speed = FF_FB_LEVEL1_TIMER;
            break;
    }
    // 3 set FF timeout 
    mCompletedFlag = false;
    if(!mFFFBLoopFlag)
    {
        getCurrentPosition(&curtime);
        mPreFrameTime = curtime;
        GST_DEBUG_Player("*** added the timer for ff ***\n");
        mFFFBLoopFlag = 1;
        //g_timeout_add(FF_FB_SWITCH_TIMER, (GSourceFunc)FF_FB_Timer_callback, this);
        mFrameCount = 0;
    }
    return true;
        
/*
    bool ret = false;
    gint64  stop = 0,start=0;//m_frontend->sliderTiming->value();
    int curtime=0,total_time=0;
    int count=0;
    gint64 time_use=0,stream_use=0,temp=0;
    struct timeval sstimer, ctimer;//sys start time ,current time
    mCompletedFlag = false;
    // 1 pause the video         
    pause();
    // 2 set the ff switch time
    if(m_speed < 0)     m_speed = 0.1;
    switch(front_state)
    {
        case FF_normal:
            m_speed = 0.1;// switch 0.1s
            break;
        case FF_Speed1:
            m_speed = 0.2;// switch 0.2s
            break;
        case FF_Speed2:
            m_speed = 0.4;// switch 0.4s
            break;
        case FF_Speed3:
            m_speed = 0.8;// switch 0.8s
            break;
        case FF_Speed4:
            m_speed = 1.6;// switch 1.6s
            break;
        case FF_Speed5:
            m_speed = 3.2;// switch 3.2s
            break;            
        default :
            m_speed = 1.0;
            break;
    }
    // 3 init start, stop video and system timer
    g_timeout_add(100, FF_FB_Timer_callback, NULL);
    if(!getCurrentPosition(&curtime))
    {
             GST_DEBUG_Player("*** getCurrentPosition error and return ***\n");
             return false;
    }
    g_usleep (100 * 1000);//ms
    if(! getDuration(&total_time))
    {
             GST_DEBUG_Player("*** getDuration error and return ***\n");
             return false;
    }
    start = curtime;
    stop = total_time;
    gettimeofday(&sstimer,NULL);     
    GST_DEBUG_Player("*** ff speed is %f***\n", m_speed);
    GST_DEBUG_Player("***curtime is %ld,   stop is %ld ***\n", start, stop);
    
    // 4 do seek loop
    // flag (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT|GST_SEEK_FLAG_SKIP)
    while(start < stop)
    {
        temp = (start + m_speed*1000)*GST_MSECOND;//  ns 
        ret = gst_element_seek (mPlayBin, 1.0, GST_FORMAT_TIME,  (GstSeekFlags)(GST_SEEK_FLAG_FLUSH), 
                       GST_SEEK_TYPE_SET, temp, GST_SEEK_TYPE_NONE, (stop*GST_MSECOND));
        if(ret)
        {
            GST_DEBUG_Player("*** ff set is start***\n");
            while(1)
            {             
                if(mCompletedFlag)
                 {
                    GST_DEBUG_Player("*** get Eqos msg, exit ff ***\n");
                    goto FF_exit;
                 }
                curtime = 0;
                if(getCurrentPosition(&curtime))
                {
                    GST_DEBUG_Player("*** get cur pos  is %d, start  pos  is %d***\n",curtime, start);// next timer is ms
                    if(curtime >= (stop -500))
                     {
                        GST_DEBUG_Player("*** ff to end position and exit***\n");
                        goto FF_exit;
                     }
                    stream_use = curtime - start;
                    
                    gettimeofday(&ctimer,NULL); 
                    time_use = (ctimer.tv_sec - sstimer.tv_sec)*1000 + (ctimer.tv_usec - sstimer.tv_usec)/1000;// ms
                    GST_DEBUG_Player("*** sys  time_use  is %d***\n",time_use);
                    if(time_use >= FF_FB_SWITCH_TIMER)// use system < 200m will delay
                    {
                        //if(stream_use >= (m_speed*1000))
                        {
                            start = curtime;
                            sstimer.tv_sec = ctimer.tv_sec;
                            sstimer.tv_usec = ctimer.tv_usec ;
                            count++;
                            GST_DEBUG_Player("*** count  is %d***\n",count);
                            break;
                        }
                    }
                    else
                    {
                        temp = FF_FB_SWITCH_TIMER - time_use;
                        GST_DEBUG_Player("*** delay time is %d***\n",temp);
                        usleep (temp * 1000);;//make temp ms delay
                    }
               }
               else
                {
                    usleep (50 * 1000);//make 20 ms delay g_sleep
               }
            }
        }
        else
        {
            GST_DEBUG_Player("*** ff set is invalid ***\n");
            break;
        }
    }
    
 FF_exit:
      GST_DEBUG_Player("*** exit ff ***\n");
     
    return true;*/ 
#else
    bool ret = false;
    gint64 value = 0, stop = 0,start=0;//m_frontend->sliderTiming->value();
    int curtime=0,total_time=0;
    float time_use=0;
    
    if(!getCurrentPosition(&curtime))
    {
             GST_DEBUG_Player("*** getCurrentPosition error and return ***\n");
             return false;
    }
    if(! getDuration(&total_time))
    {
             GST_DEBUG_Player("*** getDuration error and return ***\n");
             return false;
    }
#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif
    if(m_speed < 0)     m_speed = 1.0;
    switch(front_state)
    {
        case FF_normal:
            m_speed = 1.0;
            break;
        case FF_Speed1:
            m_speed = 2.0;
            break;
        case FF_Speed2:
            m_speed = 4.0;
            break;
        case FF_Speed3:
            m_speed = 8.0;
            break;
        case FF_Speed4:
            m_speed = 16.0;
            break;
        case FF_Speed5:
            m_speed = 32.0;
            break;            
        default :
            m_speed = 1.0;
            break;
    }
     
    value = curtime*GST_MSECOND;
    stop = total_time*GST_MSECOND;
    GST_DEBUG_Player("*** ff speed is %f***\n", m_speed);
    GST_DEBUG_Player("***value is %ld,   stop is %ld ***\n", value, stop);

    if (mPlayingState!=mPlaying_is) 
    {
	   GST_DEBUG_Player("*** start to play ***\n");
          gst_element_set_state(mPlayBin, GST_STATE_PLAYING);           
     }
     /*ret = gst_element_seek (mPlayBin, m_speed, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,//GST_SEEK_FLAG_FLUSH
                       GST_SEEK_TYPE_SET, value,
                       GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);*/
    //GST_SEEK_FLAG_FLUSH is need to more cpu, GST_SEEK_FLAG_SKIP is high effectly
    ret = gst_element_seek (mPlayBin, m_speed, GST_FORMAT_TIME,  (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT|GST_SEEK_FLAG_SKIP), //GST_SEEK_FLAG_SKIP,GST_SEEK_FLAG_KEY_UNIT
                       GST_SEEK_TYPE_SET, value,
                       GST_SEEK_TYPE_NONE, stop);//GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE
    if(ret)
         GST_DEBUG_Player("*** ff set is start***\n");
    else
        GST_DEBUG_Player("*** ff set is invalid ***\n");

    mPlayingState = mPlaying_FF;
 

 
#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
 return ret;       

 #endif  

}

bool GstPlayerPipeline::FastBack(int back_state)
{

#ifdef  FF_FB_NEW_MODE
//     bool ret = false;
     int curtime=0;
    // 1 pause the video
    if(back_state !=FF_normal&& (!mFFFBLoopFlag))
    {
        pause();
    }
    // 2 set the ff switch time
    switch(back_state)
    {
        case FF_normal:
            start();
            mFFFBLoopFlag = 0;// switch 0.1s
            return true;
        case FF_Speed1:
            m_speed = -FF_FB_LEVEL1_TIMER;// switch 0.2s
            break;
        case FF_Speed2:
            m_speed = -FF_FB_LEVEL2_TIMER;// switch 0.4s
            break;
        case FF_Speed3:
            m_speed = -FF_FB_LEVEL3_TIMER;// switch 0.8s
            break;
        case FF_Speed4:
            m_speed = -FF_FB_LEVEL4_TIMER;// switch 1.6s
            break;
        case FF_Speed5:
            m_speed = -FF_FB_LEVEL5_TIMER;// switch 3.2s
            break;            
        default :
            m_speed = -FF_FB_LEVEL1_TIMER;
            break;
    }
    // 3 set FF timeout 
    mCompletedFlag = false;
    if(!mFFFBLoopFlag)
    {
        getCurrentPosition(&curtime);
        mPreFrameTime = curtime;
        GST_DEBUG_Player("*** added the timer for ff ***\n");
        mFFFBLoopFlag = 1;
        //g_timeout_add(FF_FB_SWITCH_TIMER, (GSourceFunc)FF_FB_Timer_callback, this);
        mFrameCount = 0;
    }
    return true;
#else    
    bool ret = false;
    gint64 value = 0;//m_frontend->sliderTiming->value();
    int curtime=0;

    if(!getCurrentPosition(&curtime))
    {
             GST_DEBUG_Player("*** getCurrentPosition error and return ***\n");
             return false;
    }

#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
    g_mutex_lock(mActionMutex);
#endif   

    if(m_speed > 0)     m_speed = -1.0;
    switch(back_state)
    {
        case FB_normal:
            m_speed = -1.0;
            break;
        case FB_Speed1:
            m_speed = -2.0;
            break;
        case FB_Speed2:
            m_speed = -4.0;
            break;
        case FB_Speed3:
            m_speed = -8.0;
            break;
        case FB_Speed4:
            m_speed = -16.0;
            break;
        case FB_Speed5:
            m_speed = -32.0;
            break;            
        default :
            m_speed = -1.0;
            break;
    }
     
    value = curtime*GST_MSECOND;
    GST_DEBUG_Player("*** fb speed is %f***\n", m_speed);
    ///added operation
    if (mPlayingState!=mPlaying_is) 
    {
	   GST_DEBUG_Player("*** start to play ***\n");
          gst_element_set_state(mPlayBin, GST_STATE_PLAYING);           
    }
    /*  ret =  gst_element_seek (mPlayBin, m_speed, GST_FORMAT_TIME, GST_SEEK_FLAG_KEY_UNIT, //GST_SEEK_FLAG_SKIP
                       GST_SEEK_TYPE_CUR, G_GINT64_CONSTANT (0) ,
                       GST_SEEK_TYPE_END, value);*/
    ret =  gst_element_seek (mPlayBin, m_speed, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT|GST_SEEK_FLAG_SKIP),  // GST_SEEK_FLAG_SKIP,GST_SEEK_FLAG_KEY_UNIT
                       GST_SEEK_TYPE_SET, G_GINT64_CONSTANT (0) ,
                       GST_SEEK_TYPE_SET, value);
    if(ret)
         GST_DEBUG_Player("*** fb set is start***\n");
    else
        GST_DEBUG_Player("*** fb set is invalid ***\n");

    mPlayingState = mPlaying_FB;
 #ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
#endif
}

bool GstPlayerPipeline::SetAudioPlayStyle(int audio_style)
{
    bool ret = false;
       
#ifdef ANDROID	    
    LOCK(mActionMutex);
#else
     g_mutex_lock(mActionMutex);
#endif


	///added operation
	///

#ifdef ANDROID	    
    UNLOCK(mActionMutex);
#else
     g_mutex_unlock(mActionMutex);
#endif
return ret;
}

bool GstPlayerPipeline::SetVideoInfo(int x, int y, int w, int h, int stride)
{
    bool ret = false;
    if((x<0 ||x>LCD_POSX_MAX)|| (y<0 ||y>LCD_POSY_MAX)
        || (w<0 ||w>LCD_WIDTH_MAX)||(h<0 ||h>LCD_HEIGHT_MAX)||(stride<0 ||stride>LCD_STRIDE_MAX))
    {
        GST_DEBUG_Player("*** SetVideoInfo param error ***\n");
        return ret;
    }
    mVideoPosX = x;
    mVideoPosY=y;
    mVideoWidth = w;
    mVideoHeigh = h;
    mVideoStride = stride;
    
    GST_DEBUG_Player("*** SetVideoInfo , x:%d, y:%d. w:%d,h:%d,stride:%d ***\n",
        x, y, w, h, stride);
    g_object_set (mPlayBin, "video-xpos", mVideoPosX, NULL);
    g_object_set (mPlayBin, "video-ypos", mVideoPosY, NULL);
    g_object_set (mPlayBin, "video-width", mVideoWidth, NULL);
    g_object_set (mPlayBin, "video-height", mVideoHeigh, NULL);
    g_object_set (mPlayBin, "screen-width", mVideoStride, NULL);

    return true;
}
int GstPlayerPipeline::getPipeInitState(void)
{
    return mPipe_init_state;
}

/***********   added some api for subtitle *********/
//added subtitle api
bool GstPlayerPipeline::subtitle_open()
{

    GST_DEBUG_Player ("****subtitle_open  ****\n");
    mSubDisplay=1;
    g_object_set (mPlayBin, "fw-display", mSubDisplay, NULL);	
    return true;
}
bool GstPlayerPipeline::subtitle_closed()
{

    GST_DEBUG_Player ("****subtitle_closed  ****\n");
    mSubDisplay=0;
    g_object_set (mPlayBin, "fw-display", mSubDisplay, NULL);		
    return true;
}
bool GstPlayerPipeline::subtitle_setPos(int posx, int posy)
{

    GST_DEBUG_Player ("****subtitle_setPos  ****\n");
    mSubXpos=posx;
    mSubYpos=posy;
    
    if(mSubXpos>=0&&mSubXpos+mSubWidth<=MAIN_WIN_WIDTH)
    g_object_set (mPlayBin, "fw-xpos", mSubXpos, NULL);

    if(mSubYpos>=0&&mSubYpos+mSubHeight<=MAIN_WIN_HEIGHT)
    g_object_set (mPlayBin, "fw-ypos", mSubYpos, NULL);	
    GST_DEBUG_Player("****invalid position,use the default position*****\n");
	
    return true;
}
bool GstPlayerPipeline::subtitle_setSize(int width, int height)
{

    GST_DEBUG_Player ("****subtitle_setSize  ****\n");
    mSubWidth=width;
    mSubHeight=height;
    if(mSubWidth<=MAX_SUBTITLE_WIDTH&&mSubWidth>0)
    g_object_set (mPlayBin, "fw-width", mSubWidth, NULL);

    if(mSubHeight<=MAX_SUBTITLE_HEIGHT&&mSubHeight>0)
    g_object_set (mPlayBin, "fw-height", mSubHeight, NULL);

    return true;
}
bool GstPlayerPipeline::subtitle_setAlpha(int alpha_type, int value)
{

    GST_DEBUG_Player ("****subtitle_setAlpha  ****\n");
    mSubAlpha=value; 
    if(mSubAlpha>=0&&mSubAlpha<=255)
	g_object_set (mPlayBin, "fw-alpha", mSubAlpha, NULL);	
    return true;
}
bool GstPlayerPipeline::subtitle_setFont( const gchar* font_desc )
{
    if(mSubFont)
		g_free(mSubFont);
    mSubFont=g_strdup(font_desc);
    if(mSubFont!=NULL)
	  g_object_set (mPlayBin, "subtitle-font-desc", mSubFont, NULL);	
    GST_DEBUG_Player ("****subtitle_setFont  is %s****\n", mSubFont);
    return true;
}
bool GstPlayerPipeline::subtitle_setFile( const gchar* sub_url )
{
    if(sub_url[0] == '/')
    {
        // url is an absolute path, add prefix "file://"
        int len = strlen(sub_url) + 10;
        mSubFile = (gchar*)g_malloc(len);//malloc tom 110105
        if(mSubFile == NULL)
            return false;
        g_strlcpy(mSubFile, "file://", len);
        g_strlcat(mSubFile, sub_url, len);
    }
    else if (g_str_has_prefix(sub_url, "file:///"))
    {
        mSubFile = g_strdup(sub_url);
        if(mSubFile == NULL)
            return false;        
    }
    else
    {
        GST_DEBUG_Player("Invalide sub_url.\n");
        return false;
    }
	
    g_object_set (mPlayBin, "suburi", mSubFile, NULL);
	
    GST_DEBUG_Player ("****subtitle_setFile is %s****\n", mSubFile);
	
    if(mSubFile)
    {
	g_free(mSubFile);
	mSubFile = NULL;
    }
    return true;
}
int GstPlayerPipeline::subtitle_Status(void )
{
    // 0 -> no subtitle; 1-> support subtitle
    int subtitle_stream;
    g_object_get( mPlayBin, "n-text", &subtitle_stream, NULL);
    GST_DEBUG_Player ("****subtitle_Status  %d****\n", subtitle_stream);
    if(subtitle_stream>0)
    return true;
    else
    return false;
}    
// for multi subtitle track
int GstPlayerPipeline::subtitle_get_track(void)
{
    // 0 -> no subtitle; 1 support subtitle, >1 multi subtitle
    int subtitle_stream;
    if(mTrackFlag)
	return 0;
    g_object_get( mPlayBin, "current-text", &subtitle_stream, NULL);
    GST_DEBUG_Player ("****current subtitle id  %d****\n", subtitle_stream);
    g_object_get( mPlayBin, "n-text", &subtitle_stream, NULL);
    GST_DEBUG_Player ("****subtitle stream num  %d****\n", subtitle_stream);
    return subtitle_stream;	
	
}
bool GstPlayerPipeline::subtitle_set_track(int track)
{
    int subtitle_stream;
    GST_DEBUG_Player ("****set subtitle stream id  %d****\n", track);
    g_object_get( mPlayBin, "n-text", &subtitle_stream, NULL);
    if(track > (subtitle_stream-1))
    {
   	GST_DEBUG_Player ("****track %d > subtitle stream num %d, error****\n",track, subtitle_stream);
	return false;	
   }
    g_object_set( mPlayBin, "current-text", track, NULL);
    return subtitle_stream;		
	
}
// for multi audio track
int GstPlayerPipeline::audio_get_track(void)
{
    // 0 -> no audio; 1 audio stream, >1 multi audio stream
    int audio_stream;
    if(mTrackFlag)
        return 1;
    g_object_get( mPlayBin, "current-audio", &audio_stream, NULL);
    GST_DEBUG_Player ("****current stream id  %d****\n", audio_stream);
    g_object_get( mPlayBin, "n-audio", &audio_stream, NULL);
    GST_DEBUG_Player ("****audio stream num  %d****\n", audio_stream);

    return audio_stream;	
}
bool GstPlayerPipeline::audio_set_track(int track)
{
    int audio_stream;
    GST_DEBUG_Player ("****set audio stream id  %d****\n", track);
    g_object_get( mPlayBin, "n-audio", &audio_stream, NULL);    
    if(track > (audio_stream -1))
    {
    	GST_DEBUG_Player ("****track %d > audio total stream  %d, error ****\n", track, audio_stream);
    	return false;
    }
    g_object_set( mPlayBin, "current-audio", track, NULL);
    return true;	
	
}

//calc vs value function for lorrylv
bool GstPlayerPipeline::get_vs_value(int64_t *value)
{
    int i = 0;
    int j,k;
    int64_t vs_value[40];
    int sum[10];

    int temp[10] = {1,2,3,3,4,4,5,5,6,7};
    memset(sum,0,sizeof(sum));
    memset(value,0,sizeof(value));
    for(; i < 40; i++)
    {
       vs_value[i] = abs((int)co_mag[i]); //Ture value
    }

    for(j=0,i=0;j<10;j++)
    {
        for(k=0;k<temp[j];k++,i++)
        {
            if (sum[j]<vs_value[i])
            sum[j] = vs_value[i];
        }
    }

    for(i=0;i<10;i++)
    {
	//	if (i%2==0)
	//		value[i] = (sum[i]-20)/9;
	//	else
	    value[i] = (int)(((sum[i]-20)/7)*1.8)-1;
		if (value[i]>14)
			value[i]=14;
		else if (value[i]<0)
			value[i]=0;
    }

	return 0;
}

bool GstPlayerPipeline::eq_set_equalizer(int eqvalue)
{	
	g_object_set(G_OBJECT(mEqualizer), "band0", band_state[eqvalue][0], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band1", band_state[eqvalue][1], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band2", band_state[eqvalue][2], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band3", band_state[eqvalue][3], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band4", band_state[eqvalue][4], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band5", band_state[eqvalue][5], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band6", band_state[eqvalue][6], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band7", band_state[eqvalue][7], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band8", band_state[eqvalue][8], NULL);
	g_object_set(G_OBJECT(mEqualizer), "band9", band_state[eqvalue][9], NULL);
	
	return TRUE;
}
   
void GstPlayerPipeline::handleEos(GstMessage* p_msg)
{
    GST_DEBUG_Player ("Recevied EOS.\n");

   /* if (mIsLooping) 
    {
        GST_DEBUG_Player ("Loop is set, start playback again\n");
        start();
    } 
    else 
    {
        GST_DEBUG_Player ("send MEDIA_PLAYBACK_COMPLETE event.\n");
        if(mGstPlayer)
            mGstPlayer->sendEvent(MEDIA_PLAYBACK_COMPLETE);
    }  
    */  
    mCompletedFlag = true;
   GST_DEBUG_Player ("send MEDIA_PLAYBACK_COMPLETE event.\n");
   if(mGstPlayer)
   {
        /*if(mPlayingState == mPlaying_FF)
        {
            mPlayingState = mPlaying_No;
            mGstPlayer->sendEvent(MEDIA_FF_COMPLETE);
        }
        else if(mPlayingState == mPlaying_FB)
        {
            mPlayingState = mPlaying_No;
            mGstPlayer->sendEvent(MEDIA_FB_COMPLETE);
        }
        else
        {
            mGstPlayer->sendEvent(MEDIA_PLAYBACK_COMPLETE);
        }*/
         if(mGstPlayer)
            mGstPlayer->sendEvent(MEDIA_PLAYBACK_COMPLETE);
    }

}

void GstPlayerPipeline::handleError(GstMessage* p_msg)
{
    gchar *debug=NULL;
    GError *err=NULL;

    gst_message_parse_error(p_msg, &err, &debug);
     g_print("%s error(%d): %s debug: %s\n", g_quark_to_string(err->domain), err->code, err->message, debug);

    if (mGstPlayer)
        mGstPlayer->sendEvent(MEDIA_ERROR, err->code); 

    g_free(debug);
    g_error_free(err);
}
void GstPlayerPipeline::handleWarning(GstMessage* p_msg)
{
    gchar *debug=NULL;
    GError *warning=NULL;
    gst_message_parse_warning(p_msg, &warning, &debug);
    //GST_DEBUG_Player("%s error(%d): %s debug: %s\n", g_quark_to_string(warning->domain), warning->code, warning->message, debug);
    g_free(debug);
    g_error_free(warning);
}

void GstPlayerPipeline::handleTag(GstMessage* p_msg)
{
    GstTagList* taglist=NULL;

  //  GST_DEBUG_Player ("Enter\n");
    gst_message_parse_tag(p_msg, &taglist);
  //  GST_DEBUG_Player ("****Got tags from element %s ***\n", GST_OBJECT_NAME (p_msg->src));
    // enumerate each tag
    gst_tag_list_foreach(taglist, taglist_foreach, this);
    
    gchar* codec_name;
    if(gst_tag_list_get_string( taglist, GST_TAG_VIDEO_CODEC, &codec_name))
    {
      GST_DEBUG_Player("**** video codec is %s\n", codec_name);
#if 0
      if(strstr(codec_name, "H.264")||strstr(codec_name, "AVC")||strstr(codec_name, "MPEG-4 part 10"))
      {
      //  GST_DEBUG_Player("disable qos\n");
	//g_object_set (G_OBJECT (mVideoSink), "qos", FALSE, NULL);
      }
      else
      {
             //mGstPlayer->sendEvent(MEDIA_ERROR);
      }
#endif
      g_free(codec_name);
    }
    gst_tag_list_free(taglist);
}

void GstPlayerPipeline::handleBuffering(GstMessage* p_msg)
{
    gint percent = 0;
    gst_message_parse_buffering (p_msg, &percent);

    GST_DEBUG_Player("Buffering: %d\n", percent); 
    if (mGstPlayer)
        mGstPlayer->sendEvent(MEDIA_BUFFERING_UPDATE, (unsigned int)percent); 
}

void GstPlayerPipeline::handleStateChanged(GstMessage* p_msg)
{
    GstState newstate = GST_STATE_VOID_PENDING;
    GstState oldstate = GST_STATE_VOID_PENDING;
    GstState pending  = GST_STATE_VOID_PENDING;
    GstElement *msgsrc = (GstElement *)GST_MESSAGE_SRC(p_msg);

    gst_message_parse_state_changed(p_msg, &oldstate, &newstate, &pending);

    //GST_DEBUG_Player("State changed message: %s, old-%d, new-%d, pending-%d\n", GST_ELEMENT_NAME(msgsrc),
    //     (int)oldstate, (int)newstate, (int)pending);  //tomchen 1230 del

    // prepareAsync
    if (mAsynchPreparePending == true && msgsrc == mPlayBin && 
            newstate == GST_STATE_PAUSED && 
            (pending == GST_STATE_VOID_PENDING || pending == GST_STATE_PLAYING) )
    {
//        int width = 0;
//        int height = 0;
        GST_DEBUG_Player("prepareAsynch() done, send MEDIA_PREPARED event\n");
        mAsynchPreparePending = false;
        if (mGstPlayer)
        {
//            // get video size
//            if(getVideoSize(&width, &height))
//            {
//                printf("Send MEDIA_SET_VIDEO_SIZE, width (%d), height (%d)\n", width, height);
//                mGstPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, width, height);
//            }            
            mGstPlayer->sendEvent(MEDIA_PREPARED);
        }
    }   

    // seekTo
//    printf("mSeeking:%d newstate:%d mSeekState:%d pending:%d GST_STATE_VOID_PENDING:%d\n",mSeeking,newstate,mSeekState,pending,GST_STATE_VOID_PENDING);
    if (mSeeking == true && msgsrc == mPlayBin && newstate == mSeekState &&
            (pending == GST_STATE_VOID_PENDING))
    {
//        printf("seekTo() done, send MEDIA_SEEK_COMPLETE event\n");
        mSeeking = false;
        mSeekState = GST_STATE_VOID_PENDING;
        if (mGstPlayer)
            mGstPlayer->sendEvent(MEDIA_SEEK_COMPLETE);
    }
}

void GstPlayerPipeline::handleAsyncDone(GstMessage* p_msg)
{
    GST_DEBUG_Player("Enter\n");
}

void GstPlayerPipeline::handleSegmentDone(GstMessage* p_msg)
{
    gint64 position = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_message_parse_segment_done(p_msg, &format, &position);
    GST_DEBUG_Player ("Position: %d\n", (int)(position / GST_MSECOND));
}

void GstPlayerPipeline::handleDuration(GstMessage* p_msg)
{
    gint64 duration = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_message_parse_duration(p_msg, &format, &duration);
    GST_DEBUG_Player ("**** Duration: %d ****\n", (int)(duration / GST_MSECOND));
}

void GstPlayerPipeline::handleElement(GstMessage* p_msg)
{
    const GstStructure* pStru = gst_message_get_structure(p_msg);
    const gchar* name = gst_structure_get_name(pStru);
//    GstElement *msgsrc = (GstElement *)GST_MESSAGE_SRC(p_msg);

    //GST_DEBUG_Player("message string: %s\n", name);
    if (strcmp(name, "application/x-contacting") == 0)
    {
        //onContacting();
    }
    else if (strcmp(name, "application/x-contacted") == 0)
    {
        //onContacted();
    }
    else if (strcmp(name, "application/x-link-attempting") == 0)
    {
        //onLinkAttempting(msgsrc);
    }
    else if (strcmp(name, "application/x-link-attempted") == 0)
    {
        //onLinkAttempted(msgsrc);
    }
    else if (strcmp(name, "application/x-download-completed") == 0)
    {
        // onDownloadCompleted();
    }                      
	else if (strcmp(name,"spectrum")== 0)
	{// get the spectrum value message
		const GValue *magnitudes;
		const GValue *phases;
		const GValue *mag, *phase;
		gdouble freq;
		guint i;
		magnitudes = gst_structure_get_value (pStru, "magnitude");
		phases = gst_structure_get_value (pStru, "phase");
		for (i = 0; i < spect_bands; ++i)
		{
			freq = (gdouble) ((AUDIOFREQ /1.5) * i + AUDIOFREQ / 4) / spect_bands;
			mag = gst_value_list_get_value (magnitudes, i);
			phase = gst_value_list_get_value (phases, i);
			if (mag != NULL && phase != NULL) 
			{
				co_mag[i]=100+g_value_get_float (mag);
				co_phase[i]=g_value_get_float (phase);
			//	g_print ("band %d (freq %g): magnitude %f dB phase %f\n", i, freq,
	         //     g_value_get_float (mag), g_value_get_float (phase));
		    }
		}
	}
}

void GstPlayerPipeline::handleApplication(GstMessage* p_msg)
{
    const GstStructure* pStru = gst_message_get_structure(p_msg);
    const gchar* name = gst_structure_get_name(pStru);
//    GstElement *msgsrc = (GstElement *)GST_MESSAGE_SRC(p_msg);

    GST_DEBUG_Player("message string: %s\n", name);
    if (strcmp(name,  MSG_QUIT_MAINLOOP) == 0)
    {
        GST_DEBUG_Player ("Received QUIT_MAINLOOP message.\n");
        g_main_loop_quit (mMainLoop);
    }
}

#ifdef SSL_GST_PORT_VQ
#ifndef SSL_GST_PORT_VQ_AQBASE
#define SSL_GST_PORT_VQ_AQBASE  (300)
#endif  //SSL_GST_PORT_VQ_AQBASE
#include "mapm.c"
static int sslUpdateAVqueue(GstElement* mPlayBin)
{
    volatile int *pSRAM = (volatile int *)mapm(SSL_PRIVATE_DATA, SSL_PRIVATE_DATA_SIZE);
    int vqsize = 3;   //default to 3
//    guint64 aqsize;

    GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): mapm(SSL_PRIVATE_DATA=0x%8.8x, SSL_PRIVATE_DATA_SIZE=%d)\n==========\n\n", SSL_PRIVATE_DATA, SSL_PRIVATE_DATA_SIZE);
    if(pSRAM == NULL)
    {
        GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): mapm() failed\n==========\n\n");
        return -1;
    }
    else
    {
        vqsize = pSRAM[0];
        unmapm((void *)pSRAM, SSL_PRIVATE_DATA_SIZE);
        if(vqsize < 1 || vqsize > 32)
        {
            GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): max_size_buffers in SRAM = %d is likely corrupted value, not changing AV queues\n==========\n\n", vqsize);
            return -1;
        }
    }
#if 0
    aqsize = (guint64)vqsize*33 + SSL_GST_PORT_VQ_AQBASE;
    GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): setting vqueue to %d buffers, aqueue to %d ms\n==========\n\n", vqsize, aqsize);
    g_object_set(G_OBJECT(mPlayBin), "vqueue-max-size-buffers", vqsize, NULL);
    g_object_set(G_OBJECT(mPlayBin), "aqueue-max-size-time", aqsize*1000000, NULL);
#else
//    printf("\n\n==========\nsslUpdateAVqueue(): setting vqueue to %d buffers\n==========\n\n", vqsize);
    g_object_set(G_OBJECT(mPlayBin), "vqueue-max-size-buffers", vqsize, NULL);
#endif
    return 0;
}
#endif  //SSL_GST_PORT_VQ

void GstPlayerPipeline::vol_set(double vol)
{
    if(mVol)
    {
        if(vol>10 || vol<0) return;
        
        g_object_set (mVol,"volume",(gdouble)vol,NULL);
    }
}

void GstPlayerPipeline::mute(bool f)
{
    if(mVol)g_object_set (mVol,"mute",(gboolean)f,NULL);
    
}