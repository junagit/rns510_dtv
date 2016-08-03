/** 
 * @file   gst_core.cpp
 * @author Michał Motyl
 *
 * @date   27 styczeń 2012, 08:07
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapm.h"
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappbuffer.h>

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <struct_define.h>
#include <glib-2.0/glib/gthread.h>
#include "gst_core.h"

#ifdef TSI_LIVE
#include "dvbtcontrol.h"
#include "interface.h"
#endif

//GstClockTime baseTime;


#define SSL_PRIVATE_DATA_SIZE               (sizeof(int)*8)

#define SSL_PRIVATE_DATA                    (-SSL_PRIVATE_DATA_SIZE)
#define MSG_QUIT_MAINLOOP   "application/x-quit-mainloop"
//#define SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS  (180)
volatile int err_cnt = 0;

gst_dvb::gst_dvb()
{
    dvb_data_src = NULL;
    ts_demuxer = NULL;

    h264parser = NULL;
    mpeg_video_parser = NULL;
    h264decoder = NULL;
    mpeg2_decoder = NULL;
    video_sink = NULL;

    mp3decoder = NULL;
    ac3decoder = NULL;
    audio_sink = NULL;
    video_queue = NULL;
    audio_queue = NULL;
    queue = NULL;
    audio_volume = NULL;
    pipeline = NULL;
    loop = NULL;
    bus = NULL;
    ts_file = NULL;
    mBusMessageThread = NULL;
    mBusWatch = NULL;
    mMainCtx = NULL;	
    audio_linked = false; 
    mute_tmp = false;
    
    pthread_mutex_init (&restart_flag_lock,NULL);
}
    
gst_dvb::~gst_dvb()
{
    GST_Close();
    
    dvb_data_src = NULL;
    ts_demuxer = NULL;
    ts_file = NULL;
    h264parser = NULL;
    mpeg_video_parser = NULL;
    h264decoder = NULL;
    mpeg2_decoder = NULL;
    video_sink = NULL;
    mp3decoder = NULL;
    ac3decoder = NULL;
    audio_sink = NULL;
    video_queue = NULL;
    audio_queue = NULL;
    queue = NULL;
    audio_volume = NULL;
    pipeline = NULL;
    loop = NULL;
    bus = NULL;
    mBusMessageThread = NULL;
    mBusWatch = NULL;
    mMainCtx = NULL;	
    audio_linked = false; 
}



bool gst_dvb::GST_Play(dvb_service_pid *video_pid, dvb_service_pid *audio_pid, uint16_t service_id,dvb_service *s)
{
    GstState state;
    GST_CORE_DEBUG(" [ ENTER ]\n");
    system("reset_vpu.sh");
    err_cnt = 0;
    pthread_mutex_unlock (&restart_flag_lock);
    need_update = false;
    audio_checked = false;         
    video_checked = false;
    service_tmp = s;

    if(video_pid==NULL || audio_pid==NULL) {
        GST_CORE_ERROR_DEBUG("Error\n");
        return false;
    }

    pipeline = gst_pipeline_new ("dvb_player");
    
    if(pipeline==NULL) return false;
    
    mMainCtx = g_main_context_new ();
    
    if(mMainCtx==NULL)
    {
        gst_object_unref (pipeline);
        pipeline = NULL;
        GST_CORE_ERROR_DEBUG("Error\n");
        return false;
    }
    
    loop = g_main_loop_new (mMainCtx, FALSE);
    
    
    if (loop == NULL) 
    {
        gst_object_unref (pipeline);
        pipeline = NULL;
        
        g_main_context_unref(mMainCtx);
        mMainCtx = NULL;
        
        GST_CORE_ERROR_DEBUG("Error\n");
        return false;
    }	

    
    mBusMessageThread = g_thread_create ((GThreadFunc)do_loop, this, TRUE, NULL);
    if (mBusMessageThread == NULL)
    {
        gst_object_unref (pipeline);
        pipeline = NULL;
        
        g_main_context_unref(mMainCtx);
        mMainCtx = NULL;
        
        g_main_loop_unref(loop);
        loop = NULL;
        GST_CORE_ERROR_DEBUG("Failed to create Bus Message Thread.\n");
        return false;
    }
    
        /* we add a message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    mBusWatch = gst_bus_create_watch (bus);

    //gst_object_unref (bus);
    g_source_set_callback (mBusWatch, (GSourceFunc)bus_callback, this, NULL);
    g_source_attach (mBusWatch, mMainCtx);

    dvb_data_src = gst_element_factory_make("plugin","tsi data source");
    if(dvb_data_src==NULL) { GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    
//    ts_tee = gst_element_factory_make("tee","tsi tee");
//    if(ts_tee==NULL) { GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    
    ts_demuxer = gst_element_factory_make("mpegtsdemux","ts demuxer");
    if(ts_demuxer ==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    
    g_signal_connect (ts_demuxer, "pad-added", G_CALLBACK (gst_dvb::on_pad_added),this);

    g_object_set(ts_demuxer,"program-number",(gint)service_id,"check-crc",TRUE,NULL);
    //g_signal_connect(G_OBJECT(ts_demuxer), "notify::pmt-info", (GCallback) demuxer_notify_pmt_info,this);
//    g_signal_connect(G_OBJECT(ts_demuxer), "notify::pat-info", (GCallback)demuxer_notify_pat_info, NULL);
    
    audio_volume = gst_element_factory_make("volume","volume");
    if(audio_volume ==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    
    if(video_pid->service_pid_type==STREAM_TYPE_VIDEO_H264){
       
       h264decoder = gst_element_factory_make("ffdec_vpu_h264","h264decoder");
       if(h264decoder == NULL){GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    }
    
    if(video_pid->service_pid_type==STREAM_TYPE_VIDEO_MPEG2){
    
        mpeg2_decoder = gst_element_factory_make("ffdec_vpu_mpeg2video",NULL);
        if(mpeg2_decoder ==NULL){GST_CORE_ERROR_DEBUG("Error\n"); return false;}
        
        mpeg_video_parser = gst_element_factory_make("mpegvideoparse",NULL);
        if(mpeg_video_parser ==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}
    }
    
    if(audio_pid->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || audio_pid->service_pid_type==STREAM_TYPE_AUDIO_MPEG2)
    {
        mp3decoder = gst_element_factory_make("mad","mp3decoder");
        if(mp3decoder ==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}
        
        mp3flag = true;
    }
    
    if(audio_pid->service_pid_type==STREAM_TYPE_AUDIO_AC3)
    {    
        ac3decoder = gst_element_factory_make("ffdec_ac3",NULL);
        if(ac3decoder==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}

        mp3flag = false;
    }
    
    if(audio_pid->service_pid_type==STREAM_TYPE_AUDIO_EAC3)
    {    
        ac3decoder = gst_element_factory_make("ffdec_eac3",NULL);
        if(ac3decoder==NULL) {GST_CORE_ERROR_DEBUG("Error\n"); return false;}

        mp3flag = false;
    }
    
    video_sink = gst_element_factory_make("surfaceflingersink","video sink");
    //video_sink = gst_element_factory_make("fakesink",NULL);
    if(video_sink==NULL) {GST_CORE_ERROR_DEBUG("Error\n");return false;}
    
    audio_sink = gst_element_factory_make("alsasink","audio sink");
    //audio_sink = gst_element_factory_make("fakesink",NULL);
    if(audio_sink ==NULL) {GST_CORE_ERROR_DEBUG("Error\n");return false;}

    video_queue = gst_element_factory_make("queue","video queue");
    if(video_queue==NULL){GST_CORE_ERROR_DEBUG("Error\n");return false;}
    
    audio_queue = gst_element_factory_make("queue","audio queue");
    if(audio_queue==NULL){GST_CORE_ERROR_DEBUG("Error\n"); return false;}


    queue = gst_element_factory_make("queue","queue2");
    if(queue==NULL){GST_CORE_ERROR_DEBUG("Error\n"); return false;}
   
    
    gst_bin_add_many (GST_BIN (pipeline),dvb_data_src,/*ts_tee,*/ts_demuxer,video_sink,audio_sink,audio_volume,video_queue,audio_queue,queue,/*input_queue,*/NULL);
    
    gst_element_link (dvb_data_src,ts_demuxer);
    //gst_element_link (dvb_data_src,input_queue);
//    gst_element_link (input_queue,ts_tee);
//    gst_element_link (ts_tee,ts_demuxer);
    
    
    if(video_pid->service_pid_type==STREAM_TYPE_VIDEO_H264){
    
        gst_bin_add_many (GST_BIN (pipeline),/*,h264parser,*/h264decoder,NULL);   
        gst_element_link (video_queue,h264decoder);
        gst_element_link (h264decoder,queue);
        gst_element_link (queue,video_sink);
    }
    else
    {
        if(video_pid->service_pid_type==STREAM_TYPE_VIDEO_MPEG2)
        {
            gst_bin_add_many (GST_BIN (pipeline),mpeg_video_parser,mpeg2_decoder,NULL);

            gst_element_link (video_queue,mpeg_video_parser);
            gst_element_link (mpeg_video_parser,mpeg2_decoder);
            gst_element_link (mpeg2_decoder,video_sink);
        }
    }
    
    if(audio_pid->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || audio_pid->service_pid_type==STREAM_TYPE_AUDIO_MPEG2)
    {
        GST_CORE_DEBUG(" #### STREAM_TYPE_AUDIO MP3 ####\n");
        
        gst_bin_add_many (GST_BIN (pipeline),mp3decoder,NULL);
        gst_element_link (mp3decoder,audio_queue);
        gst_element_link (audio_queue,audio_volume);
        gst_element_link (audio_volume,audio_sink);
    }
    else
    {
        if(audio_pid->service_pid_type==STREAM_TYPE_AUDIO_AC3 || audio_pid->service_pid_type==STREAM_TYPE_AUDIO_EAC3)
        {
            GST_CORE_DEBUG(" #### STREAM_TYPE_AUDIO_AC3 ####\n");
            gst_bin_add_many (GST_BIN (pipeline),ac3decoder/*,audioresample,*/,NULL);
            gst_element_link (ac3decoder,audio_queue);
            gst_element_link (audio_queue,audio_volume);
            gst_element_link (audio_volume,audio_sink);
        }
    }
   
    //po ustawieniu qos na false brak jest eventów qos wysylanych do dekodera h264
    g_object_set (video_sink, "sync",TRUE,"qos",TRUE,"max-lateness",(150*GST_MSECOND),NULL);
    
    //g_object_set (dvb_data_src, "service_pointer",s,NULL);
   
    g_object_set (G_OBJECT(audio_sink),"provide-clock",FALSE, NULL);
    
    //slave-method 0 powoduje większe użycie procesora przy niektórych kanałach np.ESKA TV
   //g_object_set (G_OBJECT (audio_sink), "sync", TRUE,"qos",TRUE,"slave-method",0,NULL);
    
    g_object_set (G_OBJECT (audio_sink), "sync",TRUE,"qos",TRUE,"render-delay",(50*GST_MSECOND),NULL);


    g_object_set (G_OBJECT (video_queue),"max-size-buffers",((guint)0),"max-size-time",((guint64)0),"silent",TRUE,"max-size-bytes",((guint64)0),NULL);
    g_object_set (G_OBJECT (audio_queue),"max-size-buffers",((guint)0),"max-size-time",((guint64)0),"silent",TRUE,"max-size-bytes",((guint64)0),NULL);
    g_object_set (G_OBJECT (queue),"max-size-buffers",((guint)1),"silent",TRUE,"max-size-bytes",((guint64)0),NULL);
    

    //g_object_set (G_OBJECT (input_queue),"max-size-buffers",((guint)1),"max-size-time",((guint64)0),"silent",/*TRUE,"max-size-bytes",((guint64)0),*/NULL);
    mute(true);
       
        state_return = gst_element_set_state (pipeline, GST_STATE_PLAYING);

        if (state_return == GST_STATE_CHANGE_FAILURE) 
        {
            GST_CORE_ERROR_DEBUG("Blad zmiany stanu pototku na GST_STATE_PLAYING\n");
            return false;
        }
        else if (state_return == GST_STATE_CHANGE_ASYNC)
        {
            GST_CORE_DEBUG("Wait for pipeline's state change to PLAYING...\n");
            state_return = gst_element_get_state (pipeline, &state, NULL,(GST_SECOND*10));
            GST_CORE_DEBUG("Pipeline's state change to PLAYING\n");
            if (state_return != GST_STATE_CHANGE_SUCCESS || state != GST_STATE_PLAYING ) 
            {
                GST_CORE_ERROR_DEBUG("Fail to set pipeline to PLAYING, state_return is %d\n", state_return);
                return false;
            }
        }
    
    GST_CORE_DEBUG(" [ EXIT ]\n");
    return true;
}


void gst_dvb::GST_Close(void)
{
    if(loop==NULL) return;
   
    GST_CORE_DEBUG(" [ ENTER ]\n");
    g_main_loop_quit (loop);   
    if(pipeline!=NULL)
    {
    	if(mBusMessageThread)
        {
            gst_element_post_message (GST_ELEMENT (pipeline),gst_message_new_application (GST_OBJECT (pipeline),gst_structure_new (MSG_QUIT_MAINLOOP,"message", G_TYPE_STRING, "Deleting Pipeline", NULL)));

            GST_CORE_DEBUG("Wait for main loop to quit ...\n");
            g_thread_join ((GThread*)mBusMessageThread);
            GST_CORE_DEBUG("BusMessageThread joins successfully\n");
            mBusMessageThread = NULL;
        } 
        else 
        {
           GST_CORE_DEBUG("BusMessageThread is NULL, no need to quit\n");
        }
      
        bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
            
        if (bus != NULL) 
        {
            gst_bus_set_flushing (bus, TRUE);
            gst_object_unref (bus);
            bus = NULL;
        }


        GstStateChangeReturn res = gst_element_set_state (pipeline, GST_STATE_READY); 

        res = gst_element_set_state (pipeline, GST_STATE_NULL); 

        gst_object_unref (pipeline);
        pipeline = NULL;

        if(loop)
        {
            g_source_destroy (mBusWatch);
            g_source_unref(mBusWatch);
            mBusWatch = NULL;
            g_main_loop_unref (loop);
            loop = NULL;
            g_main_context_unref (mMainCtx);
            mMainCtx = NULL;
        }
        
    }
    
    dvb_data_src = NULL;
    ts_demuxer = NULL;
    ts_file = NULL;
    h264parser = NULL;
    mpeg_video_parser = NULL;
    h264decoder = NULL;
    mpeg2_decoder = NULL;
    video_sink = NULL;
    mp3decoder = NULL;
    ac3decoder = NULL;
    audio_sink = NULL;
    video_queue = NULL;
    audio_queue = NULL;
    queue = NULL;
    audio_volume = NULL;
    pipeline = NULL;
    loop = NULL;
    bus = NULL;
    mBusMessageThread = NULL;
    mBusWatch = NULL;
    mMainCtx = NULL;	
    audio_linked = false; 
    ts_file = NULL;
    init = false;
    audio_linked = false;
    
    pthread_mutex_unlock (&restart_flag_lock);
    GST_CORE_DEBUG(" [ EXIT ]\n");
    return;
}




GstBusSyncReply gst_dvb::bus_callback (GstBus *bus, GstMessage *msg, gpointer data)
{
  gst_dvb *p = (gst_dvb *) data;
  
 // printf("message %s\n",GST_MESSAGE_TYPE_NAME(msg));
  
  switch (GST_MESSAGE_TYPE (msg)) 
  {

      case GST_MESSAGE_ASYNC_DONE:
          GST_CORE_DEBUG("message GST_MESSAGE_ASYNC_DONE\n");
//          p->sslUpdateAVqueue();
          break;
          
//      case GST_MESSAGE_WARNING:
//      {
//          GError *gerror;
//        gchar *debug;
//        gchar *name = gst_object_get_path_string (GST_MESSAGE_SRC (msg));
//
//        /* dump graph on warning */
//
//        gst_message_parse_warning (msg, &gerror, &debug);
//        printf("WARNING: from element %s: %s\n", name, gerror->message);
//      //  if (debug) {
//          printf("Additional debug info:\n%s\n", debug);
//    //    }
//        g_error_free (gerror);
//        g_free (debug);
//        g_free (name);
//      }
//          break;
          
      case GST_MESSAGE_NEW_CLOCK:
      {
        GstClock *clock;

        gst_message_parse_new_clock (msg, &clock);
        GST_CORE_DEBUG("New clock: %s\n", (clock ? GST_OBJECT_NAME (clock) : "NULL"));
        break;
      }
             
//       case GST_MESSAGE_INFO:{
//        GError *gerror;
//        gchar *debug;
//        gchar *name = gst_object_get_path_string (GST_MESSAGE_SRC (msg));
//
//        gst_message_parse_info (msg, &gerror, &debug);
//        if (debug) {
//          printf("INFO:\n%s\n",debug);
//        }
//        g_error_free (gerror);
//        g_free (debug);
//        g_free (name);
//        break;
//      }
          
    case GST_MESSAGE_EOS:
      p->audio_linked = 0;
      p->init = false;
      g_main_loop_quit (p->loop);   
      break;
      
      case  GST_MESSAGE_QOS:
      {
          /* These variables are documented at:
       * http://cgit.freedesktop.org/gstreamer/gstreamer/tree/docs/design/part-qos.txt#n53*/
        gboolean live;
        GstClockTime running_time, stream_time, timestamp, duration;
        GstClock *clk;
        GstClockTime current_time;
        GstClockTime base_time;
        GstClockTime start_time;
        guint64 processed, dropped;
        GstFormat format;

        gst_message_parse_qos (msg, &live, &running_time, &stream_time,&timestamp, &duration);
        //gst_message_parse_qos_stats (msg, &format, &processed, &dropped);
        
        if(p->pipeline)
        {
            clk = gst_element_get_clock(p->pipeline);

            if(clk)
            {
                current_time = gst_clock_get_time (clk);
                base_time = gst_element_get_base_time(p->pipeline);
                start_time = gst_element_get_start_time(p->pipeline);
                gst_object_unref (clk);

    //         printf("current_time:%" GST_TIME_FORMAT,GST_TIME_ARGS (current_time));
    //         printf("\n");
    //         printf("base_time:%" GST_TIME_FORMAT,GST_TIME_ARGS (base_time));
    //         printf("\n");
    //         printf("start_time:%" GST_TIME_FORMAT,GST_TIME_ARGS (start_time));
    //         printf("\n");
    //         printf("stream_time:%" GST_TIME_FORMAT,GST_TIME_ARGS (stream_time));
    //         printf("\n");

//                printf("processed %" G_GUINT64_FORMAT,processed);
//                printf("\n");
//                printf("dropped %" G_GUINT64_FORMAT,dropped);
//                printf("\n");
  
             if((base_time+stream_time)<(current_time-(2*GST_SECOND)))
             {
                // err_cnt++;
                // if(err_cnt>50)
                // {
                 //   err_cnt = 0;
//                    printf("                                   \n");
//                    printf("                                   \n");
//                    printf("####### error ##### %" GST_TIME_FORMAT,GST_TIME_ARGS ((base_time+stream_time)));    
//                    printf("                                   \n");
//                    printf("                                   \n");
//                    printf("s33333:%" GST_TIME_FORMAT,GST_TIME_ARGS ((current_time-(5*GST_SECOND))));
//                    printf("\n");

                    pthread_mutex_lock (&p->restart_flag_lock);
                    p->restart_flag = true;
                    pthread_mutex_unlock (&p->restart_flag_lock);
                // }
             }
             else
             {
                  err_cnt = 0;
             }

            }
        }
      
      
      
//      gint64 jitter;
//      gdouble proportion;
//      gint quality;
//      GstFormat format;
//      guint64 processed, dropped;

      /* The QoS message API is documented here:
       * http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstMessage.html */
      
    //  gst_message_parse_qos_values (msg, &jitter, &proportion, &quality);
     // gst_message_parse_qos_stats (msg, &format, &processed, &dropped);

//      printf ("QOS %s: live %s, running time %" GST_TIME_FORMAT ", stream time %"
//          GST_TIME_FORMAT "," " timestamp %" GST_TIME_FORMAT ", duration %"
//          GST_TIME_FORMAT "\n" "QOS: jitter %" G_GINT64_FORMAT
//          ", proportion %f, quality %d\n" "QOS: format %s, processed %"
//          G_GUINT64_FORMAT ", dropped %" G_GUINT64_FORMAT "\n",GST_MESSAGE_SRC_NAME(msg),
//          live ? "true" : "false", GST_TIME_ARGS (running_time),
//          GST_TIME_ARGS (stream_time), GST_TIME_ARGS (timestamp),
//          GST_TIME_ARGS (duration), jitter, proportion, quality,
//          format == GST_FORMAT_BUFFERS ? "frames" : "samples", processed,
//          dropped);

//          gdouble prop;
//          gint quality;
//          gint64 jitter;
//          
//          gst_message_parse_qos_values(msg,&jitter,&prop,&quality);
//          printf("QOS:jitter %lu %f %d\n",jitter,prop,quality);
      }
          break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      GST_CORE_ERROR_DEBUG("Error: %s\n", error->message);
      g_error_free (error);

//      g_main_loop_quit (p->loop);
      break;
    }
          
    default:
//        printf("default:%d\n",GST_MESSAGE_TYPE (msg));
      break;
  }
  
  return GST_BUS_PASS;
}


//void gst_dvb::user_function (GstElement *typefind,guint probability,GstCaps *caps,gpointer data)
//{
//  gchar *type;
//
//  type = gst_caps_to_string (caps);
//  g_print ("Media type %s found, probability %d%%\n", type, probability);
//  g_free (type);
//}



void gst_dvb::on_pad_added (GstElement *element,GstPad *pad,gpointer data)
{
  GstPad *sinkpad;
  gst_dvb *class_p = (gst_dvb *) data;
  GstPad *pad_tmp = pad;
  gchar *pad_name = NULL;
  
pad_name = gst_pad_get_name(pad_tmp);
//  printf("######### on_pad_added:%s \n",pad_name);
  
  
  if(!strncmp("video",pad_name,5))
  {
      GST_CORE_DEBUG("Linkink pad %s\n",pad_name);

      sinkpad = gst_element_get_static_pad (class_p->video_queue,"sink");
      
      if(sinkpad==NULL)
      {
          GST_CORE_ERROR_DEBUG("sinkpad == NULL!!!\n");
          if(pad_name!=NULL) g_free(pad_name);
          return;
      }

      gst_pad_link (pad_tmp, sinkpad);
      gst_object_unref (sinkpad);
      if(pad_name!=NULL) g_free(pad_name);
      return;
  }
  
  if(!strncmp("audio",pad_name,5))
  {   
      //trezba uwazac zeby nie łączyc podwójnie padów gdy strumień ma więcej strumieni audio niż jeden
      if(!class_p->audio_linked)
      {
          GST_CORE_DEBUG("Linkink pad %s\n",pad_name);
          
        class_p->audio_linked = true;
        //to musi chyba tak być polacznie demuxer-audio->dekoder mp3-> audio queue
        if(class_p->mp3flag)sinkpad = gst_element_get_static_pad (class_p->mp3decoder,"sink");
        else
            sinkpad = gst_element_get_static_pad (class_p->ac3decoder,"sink");
        
        if(sinkpad==NULL)
        {
            GST_CORE_ERROR_DEBUG("sinkpad == NULL!!!\n");
            if(pad_name!=NULL) g_free(pad_name);
            return;
        }

        gst_pad_link (pad_tmp, sinkpad);
        gst_object_unref (sinkpad);
        if(pad_name!=NULL) g_free(pad_name);
        return;
      }
  }
  
  if(pad_name!=NULL) g_free(pad_name);
  
}


gpointer gst_dvb::do_loop (gst_dvb * p)
{
    
//    GstClock *clock;
//  clock=gst_pipeline_get_clock ((GstPipeline*)p->pipeline);
//  baseTime=gst_clock_get_time(clock);
  g_main_loop_run (p->loop);
  
  return NULL;
}

/**
 * @brief       Funkcja zatrzymujaca odtwarzanie TV
 * @return      0 - Stop OK,1 - błąd
 */

uint8_t gst_dvb::Stop_Play(void)
{
    GST_CORE_DEBUG(" [ ENTER ]\n");
    
//    gst_element_send_event (this->pipeline, gst_event_new_eos()); 
//    if(!gst_pad_push_event(gst_element_get_pad(dvb_data_src, "src"), gst_event_new_flush_start())) 
//    {
//        GST_CORE_DEBUG("gst_event_new_flush_start error!!!\n");
//    }

    GST_Close();
    GST_CORE_DEBUG(" [ EXIT ]\n");
    return 0;
}


void gst_dvb::mute(bool f)
{
    
    //GST_CORE_DEBUG(" %d\n",f);
    g_object_set (audio_volume,"mute",(gboolean)f,NULL);
    mute_tmp = f;
};

void gst_dvb::volume_set(double vol)
{
    if(vol>10 || vol<0) return;

    g_object_set (audio_volume,"volume",(gdouble)vol,NULL);
    
    if(mute_tmp==true) mute(false);
   
};
    
    
void gst_dvb::record_start(char *file)
{
//   ts_file = gst_element_factory_make("filesink","tsi_file_sink");
//    if(ts_file==NULL) { GST_CORE_ERROR_DEBUG("Error\n"); return;}
//    
//    g_object_set (G_OBJECT (ts_file), "location",file,"append",TRUE,"sync",TRUE,NULL);
//    gst_element_set_state (ts_file, GST_STATE_PLAYING);
//    gst_bin_add_many (GST_BIN (pipeline),ts_file,NULL);
//    gst_element_link(ts_tee,ts_file);
}


void gst_dvb::record_stop()
{
//    gst_element_set_state (ts_file, GST_STATE_NULL);
//    gst_element_unlink(ts_tee,ts_file);
//    gst_bin_remove(GST_BIN (pipeline),ts_file);
    
}

void gst_dvb::sslUpdateAVqueue()
{
    volatile int *pSRAM = (volatile int *)mapm(SSL_PRIVATE_DATA, SSL_PRIVATE_DATA_SIZE);
    int vqsize = 3;   //default to 3
//    guint64 aqsize;

//    GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): mapm(SSL_PRIVATE_DATA=0x%8.8x, SSL_PRIVATE_DATA_SIZE=%d)\n==========\n\n", SSL_PRIVATE_DATA, SSL_PRIVATE_DATA_SIZE);
    if(pSRAM == NULL)
    {
        printf("\n\n==========\nsslUpdateAVqueue(): mapm() failed\n==========\n\n");
        return;
    }
    else
    {
        vqsize = pSRAM[0];
        unmapm((void *)pSRAM, SSL_PRIVATE_DATA_SIZE);
        if(vqsize < 1 || vqsize > 32)
        {
            printf("\n\n==========\nsslUpdateAVqueue(): max_size_buffers in SRAM = %d is likely corrupted value, not changing AV queues\n==========\n\n", vqsize);
            return;
        }
    }
#if 0
    aqsize = (guint64)vqsize*33 + SSL_GST_PORT_VQ_AQBASE;
    GST_DEBUG_Player("\n\n==========\nsslUpdateAVqueue(): setting vqueue to %d buffers, aqueue to %d ms\n==========\n\n", vqsize, aqsize);
    g_object_set(G_OBJECT(mPlayBin), "vqueue-max-size-buffers", vqsize, NULL);
    g_object_set(G_OBJECT(mPlayBin), "aqueue-max-size-time", aqsize*1000000, NULL);
#else
//    printf("\n\n==========\nsslUpdateAVqueue(): setting vqueue to %d buffers\n==========\n\n", vqsize);
  //  g_object_set(G_OBJECT(mPlayBin), "vqueue-max-size-buffers", vqsize, NULL);
  g_object_set (G_OBJECT (queue),"max-size-buffers",((guint)vqsize),"silent",TRUE,"max-size-bytes",((guint64)0),NULL);
#endif
    return ;
}

void gst_dvb::demuxer_notify_pmt_info (GObject *obj, GParamSpec *pspec, gpointer user_data)
{
  GObject *pmtinfo = NULL, *streaminfo = NULL;
  GValueArray *streaminfos = NULL;
  //GValueArray *descriptors = NULL;
  //GValueArray *languages = NULL;
  dvb_srvice_pid_list *pid_list = NULL;
  dvb_service *service = NULL ;
  dvb_service_pid *tmp;
  uint16_t pids_cnt;
  guint i;
  GValue * value = NULL;
  guint program, version, pcr_pid, es_pid, es_type;
  
  gst_dvb *p = (gst_dvb *)user_data;
  
  if(p==NULL) return;
  
  service = p->service_tmp;
  
  if(service==NULL) return;
 
  g_object_get (obj, "pmt-info", &pmtinfo, NULL);
  
  if(pmtinfo==NULL) return;
  
  g_object_get (pmtinfo, "program-number", &program, NULL);
  g_object_get (pmtinfo, "version-number", &version, NULL);
  g_object_get (pmtinfo, "pcr-pid", &pcr_pid, NULL);
  g_object_get (pmtinfo, "stream-info", &streaminfos, NULL);
  //g_object_get (pmtinfo, "descriptors", &descriptors, NULL);
  
  //g_print ("PMT: program: %04x version: %d pcr: %04x streams: %d " "descriptors: %d\n",(guint16)program, version, (guint16)pcr_pid, streaminfos->n_values,descriptors->n_values);
  
  //dump_descriptors (descriptors);
  if(streaminfos==NULL) return;
  
  for (i = 0 ; i < streaminfos->n_values; i++) 
  {
    value = g_value_array_get_nth (streaminfos, i);
    streaminfo = (GObject*) g_value_get_object (value);
    
    if(streaminfo==NULL) continue;
    
    g_object_get (streaminfo, "pid", &es_pid, NULL);
    g_object_get (streaminfo, "stream-type", &es_type, NULL);
    //g_object_get (streaminfo, "languages", &languages, NULL);
    //g_object_get (streaminfo, "descriptors", &descriptors, NULL);
  
//    g_print ("pid: %04x type: %x languages: %d descriptors: %d\n",(guint16)es_pid, (guint8) es_type, languages->n_values,
//        descriptors->n_values);
    
    if(es_type==STREAM_TYPE_VIDEO_MPEG1 || es_type==STREAM_TYPE_VIDEO_MPEG2 || es_type==STREAM_TYPE_VIDEO_H264)
    {
        if(p->video_checked==false)
        {
        GST_CORE_DEBUG("SPRAWDZAM ZGODNOSC PID-u WIDEO...\n");
        if(service!=NULL)
        {
            pid_list = service->getServicePidList();
            
            if(pid_list!=NULL)
            {
                pids_cnt = pid_list->get_list_count();
                
                for(int j = 0;j<pids_cnt;j++)
                {
                    tmp = pid_list->get_from_list(j);

                    if(tmp!=NULL)
                    {
                        if(tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_VIDEO_H264)
                        {
                            if(es_pid!=tmp->pid) 
                            {
                                printf("WYKRYTO NIEZGODNOSC PID-u VIDEO!!! es_pid:%d tmp->pid:%d\n",es_pid,tmp->pid);
                                tmp->pid = es_pid;
                                p->need_update = true;
                            }
                            else
                            {
                                GST_CORE_DEBUG("PID WIDEO OK!!!\n");
                                p->need_update = false;
                            }
                            
                            p->video_checked=true;
                            break;
                        }
                    }
                }
            }
            
        }
        }
    }
    
 
    if(es_type==STREAM_TYPE_AUDIO_MPEG1 || es_type==STREAM_TYPE_AUDIO_MPEG2)
    {   
        if(p->audio_checked==false)
        {
        GST_CORE_DEBUG("SPRAWDZAM ZGODNOŚĆ PID-u AUDIO...\n");
        if(service!=NULL)
        {
            pid_list = service->getServicePidList();
            
            if(pid_list!=NULL)
            {
                pids_cnt = pid_list->get_list_count();
                
                for(int j = 0;j<pids_cnt;j++)
                {
                    tmp = pid_list->get_from_list(j);

                    if(tmp)
                    {
                        if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2)
                        {
                            if(es_pid!=tmp->pid) 
                            {
                                printf("WYKRYTO NIEZGODNOSC PID-u AUDIO!!! es_pid:%d %x tmp->pid:%d %x\n",es_pid,es_pid,tmp->pid,tmp->pid);
                                tmp->pid = es_pid;
                                p->need_update = true;
                                
                            }
                            else
                            {
                                GST_CORE_DEBUG("PID AUDIO OK!!!\n");
                                p->need_update = false;
                            }
                            
                            p->audio_checked = true;
                            break;
                        }
                    }
                }
            }
            
        }
        
        }
    }
   
        
 
//    dump_languages (languages);
   // dump_descriptors (descriptors);
  }
}

void gst_dvb::demuxer_notify_pat_info (GObject *obj, GParamSpec *pspec, gpointer user_data)
{
  GValueArray *patinfo = NULL;
  GValue * value = NULL;
  GObject *entry = NULL;
  guint program, pid;
  guint i;

  g_object_get (obj, "pat-info", &patinfo, NULL);
  
  g_print ("PAT: entries: %d\n", patinfo->n_values);  
  for (i = 0; i < patinfo->n_values; i++) {
    value = g_value_array_get_nth (patinfo, i);
    entry = (GObject*) g_value_get_object (value);
    g_object_get (entry, "program-number", &program, NULL);
    g_object_get (entry, "pid", &pid, NULL);
    g_print ("    program: %04x pid: %04x\n", program, pid);
  }
  
}


void gst_dvb::GetRenderedFrames(gint *rendered)
{

    if(video_sink)
    {
        g_object_get (video_sink, "rendered-frames", rendered, NULL);
    }


    return;
}

//void gst_dvb::dump_descriptors (GValueArray *descriptors)
//{
//  GValue * value;
//  guint i;
//  for (i = 0 ; i < descriptors->n_values; i++) {
//    GString *string;
//    value = g_value_array_get_nth (descriptors, i);
//    string = (GString *) g_value_get_boxed (value);
//    
//    if (string->len > 2) {
//      g_print ("descriptor # %d tag %02x len %d\n",
//          i + 1, (guint8)string->str[0], (guint8)string->str[1]);
//      gst_util_dump_mem ((guint8*)string->str + 2, string->len - 2);    
//    }
//  }  
//  g_print ("\n");
//}

//void gst_dvb::dump_languages (GValueArray *languages)
//{
//  GValue * value;
//  guint i;
//  if (!languages->n_values)
//    return;
// 
//  g_print ("languages: ");
//  for (i = 0 ; i < languages->n_values; i++) {
//    const gchar *string;
//    value = g_value_array_get_nth (languages, i);
//    string = g_value_get_string (value);    
//    g_print ("%s,", string);
//  }  
//  g_print ("\n");
//}