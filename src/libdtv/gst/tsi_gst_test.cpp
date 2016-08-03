/** 
 * @file   tsi_gst_test.cpp
 * @author michalm
 *
 * @date   16 kwiecień 2012, 12:34
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

#include "tsi_gst_test.h"

tsi_gst_test::tsi_gst_test()
{
    
    
}



tsi_gst_test::~tsi_gst_test() 
{
    
    
    
}



bool tsi_gst_test::start_test(void)
{
    if (!g_thread_supported ()) g_thread_init (NULL);
    
    gst_init(NULL,NULL);
    
    pipeline = gst_pipeline_new ("dvb_player");

    mMainCtx = g_main_context_new ();
    loop = g_main_loop_new (mMainCtx, FALSE);
    
    if (loop == NULL) 
    {
        printf("Error\n");
        return false;
    }	

    

    mBusMessageThread = g_thread_create ((GThreadFunc)do_loop, this, TRUE, NULL);
    if (mBusMessageThread == NULL)
    {
        printf("Failed to create Bus Message Thread.\n");
        return false;

 
    src = gst_element_factory_make("plugin",NULL);
    if(src==NULL) { printf("Error\n"); return false;}
    
    sink = gst_element_factory_make("fakesink",NULL);
    if(sink ==NULL) {printf("Error\n"); return false;}
    
    gst_bin_add_many (GST_BIN (pipeline),src,sink,NULL);
    gst_element_link (src,sink);
    
    state_return = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if (state_return == GST_STATE_CHANGE_FAILURE) 
    {
        printf("Blad zmiany stanu pototku na GST_STATE_PLAYING\n");
        return false;
    }
    
}
}

void tsi_gst_test::exit_test(void)
{
    gst_element_send_event(pipeline,gst_event_new_eos());
    
    if(pipeline!=NULL)
    {
        //chodzi bez tego kodu??
#if 1
    	if(mBusMessageThread)
        {
            printf("Send QUIT_MAINLOOP message to bus\n");
            gst_element_post_message (GST_ELEMENT (pipeline),gst_message_new_application (GST_OBJECT (pipeline),gst_structure_new (MSG_QUIT_MAINLOOP,"message", G_TYPE_STRING, "Deleting Pipeline", NULL)));

            /* Wait for main loop to quit*/
            printf("Wait for main loop to quit ...\n");
            g_main_loop_quit(loop);
            g_thread_join (mBusMessageThread);
            printf("BusMessageThread joins successfully\n");
            mBusMessageThread = NULL;
        } 
        else 
        {
           printf("BusMessageThread is NULL, no need to quit\n");
        }
#endif 
      
        GstBus *bus;
            bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
            
            if (bus != NULL) {
                    gst_bus_set_flushing (bus, TRUE);
                    gst_object_unref (bus);
            }

        gst_element_set_state (pipeline, GST_STATE_NULL); //tom 1224 for test
        gst_object_unref (pipeline);
        pipeline = NULL;
    }
    
}


gpointer tsi_gst_test::do_loop (tsi_gst_test * p)
{
  g_main_loop_run (p->loop);
  return NULL;
}
