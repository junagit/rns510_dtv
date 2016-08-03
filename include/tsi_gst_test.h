/** 
 * @file   tsi_gst_test.h
 * @author michalm
 *
 * @date   16 kwiecień 2012, 12:34
 */

#ifndef TSI_GST_TEST_H
#define	TSI_GST_TEST_H

#define MSG_QUIT_MAINLOOP   "application/x-quit-mainloop"

class tsi_gst_test {
public:
    tsi_gst_test();
    virtual ~tsi_gst_test();
    bool start_test(void);
    void exit_test(void);
private:
    GstElement          *src,*sink;        
    GstElement          *pipeline;
    GMainLoop           *loop;
    GstStateChangeReturn state_return;
    GThread             *mBusMessageThread;

    GSource             *mBusWatch;
    GMainContext        *mMainCtx;	
    static gpointer     do_loop (tsi_gst_test* p);
};

#endif	/* TSI_GST_TEST_H */

