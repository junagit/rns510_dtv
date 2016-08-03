/** 
 * @file   gst_core.h
 * @author michalm
 *
 * @date   30 styczeń 2012, 09:32
 */

#ifndef GST_CORE_H
#define	GST_CORE_H

#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappbuffer.h>

#include "struct_define.h"


#define GST_CORE_DEBUG_T 0
#define GST_CORE_ERROR_DEBUG_T 1
#if GST_CORE_DEBUG_T
        #define GST_CORE_DEBUG(fmt, args...)   printf("[GST_CORE][%s]: "fmt, __FUNCTION__, ##args)                /*info function */
#else
        #define GST_CORE_DEBUG(fmt, args...)
#endif

#if GST_CORE_ERROR_DEBUG_T
        #define GST_CORE_ERROR_DEBUG(fmt, args...)   printf("[GST_CORE ERROR][%s:%d]: "fmt, __FUNCTION__,__LINE__, ##args)                /*info function */
#else
        #define GST_CORE_ERROR_DEBUG(fmt, args...)
#endif


/**
 * @brief Klasa z funkcjami twrzącymi potok Gstreamer'a i sterującymi odtwarzaniem TV
 */
class gst_dvb
{
public:
     gst_dvb();
    ~gst_dvb();
    bool GST_Play(dvb_service_pid *video_pid, dvb_service_pid *audio_pid, uint16_t service_id,dvb_service *s);
    void GST_Close(void);
    uint8_t Stop_Play(void);//!< Funkcja zatrymująca odtwarzanie TV
    bool play_test();
    void mute(bool f);
    void volume_set(double vol);
    void record_start(char *file);
    void record_stop();
    void GetRenderedFrames(gint *rendered);
    bool need_upadate_tv_list()
    {
        return need_update;
    }
    
    bool need_restart()
    {
        pthread_mutex_lock (&restart_flag_lock);
      bool  temp = restart_flag;
      restart_flag = false;
      pthread_mutex_unlock (&restart_flag_lock);
      return temp;
    };
private:
    //static gboolean bus_call (GstBus *bus,GstMessage *msg,gpointer data);
    static GstBusSyncReply bus_callback (GstBus *bus, GstMessage *msg, gpointer data);
    static void on_pad_added (GstElement *element,GstPad *pad,gpointer data);
    //static void user_function (GstElement *typefind,guint probability,GstCaps *caps,gpointer data);
    static void demuxer_notify_pmt_info (GObject *obj, GParamSpec *pspec, gpointer user_data);
    static void demuxer_notify_pat_info (GObject *obj, GParamSpec *pspec, gpointer user_data);
    //static void dump_descriptors (GValueArray *descriptors);
    //static void dump_languages (GValueArray *languages);
    static gpointer do_loop (gst_dvb* p);

    GstElement          *dvb_data_src;
    GstElement          *ts_demuxer;

    GstElement          *mpeg_video_parser;

    GstElement          *h264parser;
    GstElement          *h264decoder;
    GstElement          *mpeg2_decoder;


    GstElement          *mpeg_audio_parser;
    GstElement          *mp3decoder;
    GstElement          *ac3decoder;
    
    GstElement          *video_sink;
    GstElement          *audio_sink;
    GstElement          *video_queue;
    GstElement          *audio_queue;
    GstElement          *queue;
        
    GstElement          *audio_volume;
    GstElement          *ts_tee;
    GstElement          *pipeline;
    
    GstElement          *deinterlacer;

    GstElement          *ts_file;
    GMainLoop           *loop;
    GstStateChangeReturn state_return;
    GstBus              *bus;
    GThread             *mBusMessageThread;
    
    
    GstElement          *input_queue;

    volatile bool                restart_flag;
    volatile bool                mute_tmp;
    volatile bool                mp3flag;
    volatile bool       need_update;
    GSource             *mBusWatch;
    GMainContext        *mMainCtx;	
    volatile bool                init; 
    volatile bool       audio_checked;         
    volatile bool       video_checked;
    uint16_t            curr_pmt;
    volatile bool       audio_linked;
    dvb_service         *service_tmp;
    void sslUpdateAVqueue();
    pthread_mutex_t     restart_flag_lock;
};

#endif	/* GST_CORE_H */

