#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdint.h>
#include <gst/gst.h>


volatile int read_cnt = 0;
volatile bool jjj = false;

#include "dma_mem_src.h"
#include "tsi.h"
#include "interface.h"
#include "dvbtcontrol.h"
#include <sys/time.h>
#include <sys/mman.h>
#include <gst/base/gstbasesrc.h>
#include "dvb_service.h"
#include "struct_define.h"
#include "crc32.h"
GST_DEBUG_CATEGORY_STATIC (gst_plugin_template_debug);
#define GST_CAT_DEFAULT gst_plugin_template_debug

enum
{
  PROP_0,
  PROP_CURR_SERVICE
};


volatile int send_pmt = 10;
#ifdef DIRECT_DMA
#define TSI_READ_LEN (21*188)
#else
#define TSI_READ_LEN (60*188)
#endif

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)


//#define SHOW_DATA_RATE 1

//static bool measure = false;
//static uint32_t data_read = 0;

#ifdef SHOW_DATA_RATE
struct timeval start,end;
volatile int mcnt = 10; 
#endif

bool flag = false;
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS("video/mpegts, " "mpegversion = (int) 2," "systemstream = (boolean) true")
    );

GST_BOILERPLATE (GstPluginTemplate, gst_plugin_template, GstPushSrc, GST_TYPE_PUSH_SRC);

static void gst_plugin_template_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec);
static void gst_plugin_template_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec);
//static GstStateChangeReturn gst_dvbsrc_change_state (GstElement * element, GstStateChange transition);

static void gst_file_src_finalize (GObject * object);
static gboolean gst_file_src_start (GstBaseSrc * basesrc);
static gboolean gst_file_src_stop (GstBaseSrc * basesrc);
static GstFlowReturn gst_file_src_create(GstPushSrc * element, GstBuffer ** buffer);

#ifdef DIRECT_DMA
static uint32_t demuxer_fill_buffer_packet_process(int32_t buffer_len_s32,GstPluginTemplate * data,void *data_ptr);
#else
static uint32_t demuxer_fill_buffer_packet_process(int32_t buffer_len_s32,GstPluginTemplate * data);
#endif


static void gst_plugin_template_base_init (gpointer gclass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_set_details_simple(element_class,"Plugin","Source","Data source from DMA memory","Michal Motyl");
  gst_element_class_add_pad_template (element_class,gst_static_pad_template_get (&src_factory));
}

static gboolean gst_dvbsrc_unlock (GstBaseSrc * bsrc)
{
  GstPluginTemplate *src = GST_PLUGIN_TEMPLATE (bsrc);
  src->need_unlock = TRUE;
  return TRUE;
}

static gboolean gst_dvbsrc_unlock_stop (GstBaseSrc * bsrc)
{
  GstPluginTemplate *src = GST_PLUGIN_TEMPLATE (bsrc);

  src->need_unlock = FALSE;
  return TRUE;
}

static gboolean gst_dvbsrc_is_seekable (GstBaseSrc * bsrc)
{
  return FALSE;
}

static gboolean gst_dvbsrc_get_size (GstBaseSrc * src, guint64 * size)
{
  return FALSE;
}

#ifdef DIRECT_DMA
static void *malloc_aligned(size_t alignment, size_t bytes)
{
    // we need to allocate enough storage for the requested bytes, some 
    // book-keeping (to store the location returned by malloc) and some extra
    // padding to allow us to find an aligned byte.  im not entirely sure if 
    // 2 * alignment is enough here, its just a guess.
    const size_t total_size = bytes + (2 * alignment) + sizeof(size_t);

    // use malloc to allocate the memory.
    char *data = (char*)malloc(sizeof(char) * total_size);

    if (data)
    {
        // store the original start of the malloc'd data.
        const void * const data_start = data;

        // dedicate enough space to the book-keeping.
        data += sizeof(size_t);

        // find a memory location with correct alignment.  the alignment minus 
        // the remainder of this mod operation is how many bytes forward we need 
        // to move to find an aligned byte.
        const size_t offset = alignment - (((size_t)data) % alignment);

        // set data to the aligned memory.
        data += offset;

        // write the book-keeping.
        size_t *book_keeping = (size_t*)(data - sizeof(size_t));
        *book_keeping = (size_t)data_start;
    }

    return data;
}

static void free_aligned(void *raw_data)
{
    
//    printf("@@@@ free_aligned @@@@\n");
    if (raw_data)
    {
        char *data = (char*)raw_data;

        // we have to assume this memory was allocated with malloc_aligned.  
        // this means the sizeof(size_t) bytes before data are the book-keeping 
        // which points to the location we need to pass to free.
        data -= sizeof(size_t);

        // set data to the location stored in book-keeping.
        data = (char*)(*((size_t*)data));

        // free the memory.
        free(data);
    }
}
#endif

/* initialize the plugin's class */
static void gst_plugin_template_class_init (GstPluginTemplateClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbasesrc_class;
  GstPushSrcClass *gstpushsrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *) klass;
  gstpushsrc_class = (GstPushSrcClass *) klass;
  gobject_class->finalize = gst_file_src_finalize;
  gobject_class->set_property = gst_plugin_template_set_property;
  gobject_class->get_property = gst_plugin_template_get_property;

 // g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_int ("width", "window width","Output window width", 0,sizeof(int),0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  gstbasesrc_class->start = GST_DEBUG_FUNCPTR (gst_file_src_start);
  gstbasesrc_class->stop = GST_DEBUG_FUNCPTR (gst_file_src_stop);
  gstbasesrc_class->unlock = GST_DEBUG_FUNCPTR (gst_dvbsrc_unlock);
  gstbasesrc_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_dvbsrc_unlock_stop);
  gstbasesrc_class->is_seekable = GST_DEBUG_FUNCPTR (gst_dvbsrc_is_seekable);
  gstbasesrc_class->get_size = GST_DEBUG_FUNCPTR (gst_dvbsrc_get_size);
  gstpushsrc_class->create = gst_file_src_create;
  
//   g_object_class_install_property (gobject_class, PROP_CURR_SERVICE,
//        g_param_spec_string ("surface", "Surface",
//        "The pointer of ISurface interface", NULL, G_PARAM_READWRITE));
   
    g_object_class_install_property (gobject_class, PROP_CURR_SERVICE,g_param_spec_pointer ("service_pointer", "service_pointer","Whether to be very verbose or not",(GParamFlags)G_PARAM_READWRITE));
}

dvb_service_pid *get_video_pid_from_list2(dvb_service *sp)
{
    uint16_t pids_cnt,i;
    dvb_service_pid *tmp;
    
    if(sp==NULL) return NULL;
    
    pids_cnt = sp->getServicePidList()->get_list_count();
    
    //printf("get_video_pid_from_list: pids_cnt = %d\n",pids_cnt);
    
    if(!pids_cnt) return NULL;
    
    for(i = 0;i<pids_cnt;i++)
    {
        tmp = sp->getServicePidList()->get_from_list(i);
        
        if(tmp)
        {
            if(tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_VIDEO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_VIDEO_H264)
            {
                return tmp;
            }
        }
    }
    
    return NULL;
}

bool make_new_pmt(dvb_service *service,unsigned char *dst)
{
    unsigned char * buf;
    dvb_service_pid *tmp;
    
    if(service==NULL) return false;
    
    if(dst==NULL) return false;
    
    if(service->getServicePidList()->get_list_count()==0) return false;
    
    printf("make_new_pmt: service name:%s\n",service->getServiceName());

	if(dst!=NULL){
            buf = &dst[TS_PACKET_SIZE];
        }
        
	buf[0] = 0x47;
	buf[1] = 0x40|((service->getPMTpid()>>8)&0x1f);//high 5 bits
	buf[2] = service->getPMTpid() & 0xff;//low 8 bit
	buf[3] = 0x10;
    	//packet data
	buf[4] = 0x00;
	buf[5] = 0x02;//table id
	
	buf[6] = 0xb0;//section length H 4 bits
	//buf[7] = 0x17;//section length L 8 bits
	
	buf[8] = (service->getServiceID()>>8) & 0xff;//H program id
	buf[9] = service->getServiceID() & 0xff;       //L program id
	buf[10]= 0xc5;//version number = 2
   	buf[11]= 0x00; //section number
   	buf[12]= 0x00; //last section number
   	buf[13]= 0xe0 | ((service->getPCRpid()>>8) & 0xff); //PRC H
   	buf[14]= service->getPCRpid() & 0xff;  // PRC L
   	buf[15]= 0xf0;  //program information H
   	buf[16]= 0x00;	//program information L
	int index = 17;	
	
        tmp = get_video_pid_from_list2(service);
        
	if(tmp != NULL)//for none video es
	{
		buf[index++]= tmp->service_pid_type;//video type
		buf[index++]= 0xe0|((tmp->pid>>8)&0x1f);//video pid H 5 bits
		buf[index++]= tmp->pid & 0xff; //video pid L 8 bits
		buf[index++]= 0xf0; //ES infor Length H 4 bits
		buf[index++]= 0x00; //ES infor Length L 8 bits
	}
	unsigned int kk = 0,jj=index;
	//printf("a_number:%d\n",a_number);
	for(kk=0;kk<service->getServicePidList()->get_list_count();kk++)
	{
            tmp = service->getServicePidList()->get_from_list(kk);
            
            if(tmp==NULL) continue;
            
            if(tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG1 || tmp->service_pid_type==STREAM_TYPE_AUDIO_MPEG2 || tmp->service_pid_type==STREAM_TYPE_AUDIO_AAC || tmp->service_pid_type==STREAM_TYPE_AUDIO_AC3 || tmp->service_pid_type==STREAM_TYPE_AUDIO_EAC3)
            {
                
                buf[jj++]=tmp->service_pid_type;//audio type
                buf[jj++]=0xe0|((tmp->pid>>8)&0x1f);//audio pid H 5 bits
                buf[jj++]=tmp->pid & 0xff; //audio pid L 8 bits
                buf[jj++]=0xf0;//ES infor Length H 4 bits
                buf[jj++]=0x00;//ES infor Length L 8 bits
                
            }
	}
	buf[7] = jj + 4 - 8;
	uint32_t crc = 0;
        index = jj;	
	crc = _dvb_crc32(&buf[5],index-5);
	  
	buf[index] = (crc>>24)&0xff;
	buf[index+1] = (crc>>16)&0xff;
	buf[index+2] = (crc>>8)&0xff;
	buf[index+3] = crc & 0xff;
	int i;
	for(i=index+4;i<TS_PACKET_SIZE;i++)
	{
	    buf[i]=0xff;
	}

        printf("## make_new_pmt OK!!!1\n");
	return true;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_plugin_template_init (GstPluginTemplate * filter,GstPluginTemplateClass * gclass)
{
  filter->silent = FALSE;
  filter->map_base = NULL;
  filter->target = tsi_get_alloc_dma_phys_addr();
  filter->virt_addr = NULL;
  filter->need_unlock = TRUE;
  filter->current_service = NULL;
  send_pmt = 10;
  
  
  gst_base_src_set_live (GST_BASE_SRC (filter), FALSE);
  gst_base_src_set_do_timestamp(GST_BASE_SRC (filter), TRUE);
  gst_base_src_set_format(GST_BASE_SRC (filter),GST_FORMAT_DEFAULT);
  gst_base_src_set_blocksize(GST_BASE_SRC (filter),(TSI_READ_LEN));
}


static void
gst_plugin_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstPluginTemplate *filter = GST_PLUGIN_TEMPLATE (object);

  switch (prop_id) {
    case PROP_CURR_SERVICE:
        printf("###### PROP_CURR_SERVICE #######\n");
       filter->current_service =  (dvb_service*)g_value_get_pointer(value);
      break;
    default:
    //  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}



static void
gst_plugin_template_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  //GstPluginTemplate *filter = GST_PLUGIN_TEMPLATE (object);

  switch (prop_id) {

      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}



/*
static GstStateChangeReturn gst_dvbsrc_change_state (GstElement * element, GstStateChange transition)
{
  GstPluginTemplate *src;
  GstStateChangeReturn ret;

  src = GST_PLUGIN_TEMPLATE (element);
  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
            //printf("@@@@@@@@@@@ GST_STATE_CHANGE_NULL_TO_READY\n");
            break;
      
      case GST_STATE_CHANGE_READY_TO_NULL:
            //printf("@@@@@@@@@@@ GST_STATE_CHANGE_READY_TO_NULL\n");
            break;
          
     case GST_STATE_CHANGE_PAUSED_TO_READY:
            //printf("@@@@@@@@@@@ GST_STATE_CHANGE_PAUSED_TO_READY\n");
            break;
          
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
            //printf("@@@@@@@@@@@ GST_STATE_CHANGE_PAUSED_TO_PLAYING\n");
            break;

    case GST_STATE_CHANGE_READY_TO_PAUSED:
            //printf("@@@@@@@@@@@ GST_STATE_CHANGE_READY_TO_PAUSED\n");
            break;
          
    default:
      break;
  }

  return ret;
}
*/


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean plugin_init (GstPlugin * plugin)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template plugin' with your description
   */
    
  GST_DEBUG_CATEGORY_INIT (gst_plugin_template_debug, "plugin",0, "Template plugin");

  return gst_element_register (plugin, "plugin", GST_RANK_NONE,GST_TYPE_PLUGIN_TEMPLATE);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstplugin"
#endif


/*
static gboolean register_elements (GstPlugin *plugin)
{
  return gst_element_register (plugin, "plugin",GST_RANK_NONE, G_TYPE_FUNDAMENTAL(22233));
}
*/


/* gstreamer looks for this structure to register plugins
 *
 * exchange the string 'Template plugin' with your plugin description
 */
GST_PLUGIN_DEFINE_STATIC (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "plugin",
    "Template plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)

        
static void gst_file_src_finalize (GObject * object)
{
  GstPluginTemplate *src;
  src = GST_PLUGIN_TEMPLATE (object);
  
    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize (object);
  
}


static gboolean gst_file_src_start (GstBaseSrc * basesrc)
{
 GstPluginTemplate *src = GST_PLUGIN_TEMPLATE (basesrc);

    if((src->fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
    {
        #ifdef SHOW_DATA_RATE
        mcnt = 5;
        #endif
        printf("/dev/mem could not be opened.\n");
        perror("open");
        return FALSE;
    } 

    src->map_base = (uint32_t*)mmap(0, (MAP_SIZE*16), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_NONBLOCK, src->fd, src->target & ~MAP_MASK);

    if(src->map_base == (uint32_t*) -1) {
        printf("Memory map failed.\n");
        perror("mmap");
        return FALSE;
    }

    src->virt_addr = (src->map_base + (src->target & MAP_MASK));
    src->need_unlock = TRUE;

    return TRUE;

}

/* unmap and close the file */
static gboolean gst_file_src_stop (GstBaseSrc * basesrc)
{
    GstPluginTemplate *src = GST_PLUGIN_TEMPLATE (basesrc);

  munmap((void*)src->map_base,(MAP_SIZE*16));
  /* close the file */
  close (src->fd);

  
  src->fd = -1;
  src->need_unlock = TRUE;
  return TRUE;
}

/*
static gboolean gst_file_src_is_seekable (GstBaseSrc * basesrc)
{
  return 0;
}



static gboolean gst_file_src_get_size (GstBaseSrc * basesrc, guint64 * size)
{
  *size = 0;
  return 0;
}
*/


static GstFlowReturn gst_file_src_create(GstPushSrc * element, GstBuffer ** buffer)
{
GstBuffer *buf = NULL;
uint32_t data_read_s32 = 0;
GstPluginTemplate *src = GST_PLUGIN_TEMPLATE (element);
//GstBaseSrc *bsrc;
//unsigned char tmp_pmt[2*188];
//GstClockTime time,new_base_time;
//static GstClockTime last_time;
#ifdef DIRECT_DMA
char *temp_ptr;

temp_ptr = (char*)malloc_aligned(4096,TSI_READ_LEN);
#endif

#ifdef SHOW_DATA_RATE
long int usecs;
unsigned long int secs;
double rate;

read_cnt++;

    if(measure==false)
    {
        measure = true;
        data_read = 0;
        gettimeofday(&start,NULL);
        secs = 0;
        usecs = 0;
        mcnt = 5;
    }
    else
    {
        
        gettimeofday(&end,NULL);
        
        secs = end.tv_sec-start.tv_sec;
        usecs = end.tv_usec-start.tv_usec;
        
        if(usecs<0) {
                usecs += 1000000;
                secs -= 1;
        }

        if(mcnt==0)
        {
            printf("secs:%d usecs:%d data_read:%d\n",secs,usecs,data_read);
            measure=false;
            rate = (double)data_read/((double)secs + (double)usecs/1000000.0);
            printf("Data rate: %5.3g MiB/s\n", (rate/(1024.0*1024.0)));
        }
    }
#endif 
    
#ifdef DIRECT_DMA
    data_read_s32 = demuxer_fill_buffer_packet_process(TSI_READ_LEN,src,temp_ptr);
#else
    
//    GstClock *clock;
//
//
//    clock = gst_element_get_clock (GST_ELEMENT (element));
//
//    if (clock) {
//      last_time = new_base_time = time = gst_clock_get_time (clock);
////      printf("time1:%" GST_TIME_FORMAT,GST_TIME_ARGS (time));
////      printf("\n");
//      time -= gst_element_get_base_time (GST_ELEMENT (element));
//      gst_object_unref (clock);
//    }
    data_read_s32 = demuxer_fill_buffer_packet_process(TSI_READ_LEN,src);
#endif


#ifdef SHOW_DATA_RATE
    if(measure==true)
    {
        data_read+=data_read_s32;
        mcnt--;
    }
#endif
    
#ifdef DIRECT_DMA
    if(data_read_s32==0) 
    {
        free_aligned(temp_ptr);
        data_read_s32 = 188;
        buf = gst_buffer_try_new_and_alloc(data_read_s32);
        GST_BUFFER_SIZE (buf) = data_read_s32;
        GST_BUFFER_OFFSET (buf) = GST_BUFFER_OFFSET_NONE;
        GST_BUFFER_TIMESTAMP(buf) = GST_CLOCK_TIME_NONE;
        GST_BUFFER_FLAG_SET(buf,GST_BUFFER_FLAG_DISCONT);
        *buffer = buf;
    }
    else
    {
        buf = gst_buffer_new ();
        GST_BUFFER_SIZE (buf) = data_read_s32;
        GST_BUFFER_MALLOCDATA (buf) = (guint8*)temp_ptr;
        GST_BUFFER_DATA (buf) = GST_BUFFER_MALLOCDATA (buf);
        buf->free_func = free_aligned;
        GST_BUFFER_OFFSET (buf) = GST_BUFFER_OFFSET_NONE;
        GST_BUFFER_TIMESTAMP(buf) = GST_CLOCK_TIME_NONE;
        *buffer = buf;
    }
#else
    
    if(data_read_s32==0) 
    {
        data_read_s32 = 188;
        buf = gst_buffer_try_new_and_alloc(data_read_s32);
        GST_BUFFER_SIZE (buf) = data_read_s32;
        GST_BUFFER_OFFSET (buf) = GST_BUFFER_OFFSET_NONE;
        GST_BUFFER_TIMESTAMP(buf) = GST_CLOCK_TIME_NONE;
      //  GST_BUFFER_TIMESTAMP(buf) = time;
      //  GST_BUFFER_FLAG_SET(buf,GST_BUFFER_FLAG_DISCONT);
        *buffer = buf;

    }
    else
    {
        buf = gst_buffer_try_new_and_alloc(data_read_s32); 
        ssl_memcpy(GST_BUFFER_DATA(buf),((guint8*)src->virt_addr),data_read_s32); 
        GST_BUFFER_SIZE (buf) = data_read_s32;
        GST_BUFFER_OFFSET (buf) = GST_BUFFER_OFFSET_NONE;
       // GST_BUFFER_TIMESTAMP(buf) = GST_CLOCK_TIME_NONE;
    //    GST_BUFFER_TIMESTAMP(buf) = time;
        *buffer = buf;
    }
#endif
    return GST_FLOW_OK;
}

#ifdef DIRECT_DMA

static uint32_t demuxer_fill_buffer_packet_process(int32_t buffer_len_s32,GstPluginTemplate * data,void *data_ptr)
{
	uint32_t read_len_s32, actual_read_len_s32,total_read_len_s32;
	char *p_read_buf_s8,*pread_buf_phy_u8;
        stream_t  st_packet_process;
        
	actual_read_len_s32 = 0;
	total_read_len_s32 = 0;
	//p_read_buf_s8 = p_buffer_s8;
	read_len_s32 = buffer_len_s32;

        pread_buf_phy_u8 = (char *)data_ptr;
	p_read_buf_s8 = pread_buf_phy_u8;

	st_packet_process.buff_idx = TSI_MEDIA_DATA_BUFFINDEX;
	st_packet_process.time_out = 500; //500 ms
             
        while(0<read_len_s32)
        {
            st_packet_process.buffer = (unsigned char*)p_read_buf_s8;
            st_packet_process.read_len = read_len_s32;

            /* use dma blocking call */
            actual_read_len_s32 = tsi_fill_buffer_dma(&st_packet_process);

            total_read_len_s32 += actual_read_len_s32;
            p_read_buf_s8 += actual_read_len_s32;
            read_len_s32 -= actual_read_len_s32;   
            
            if(0 == actual_read_len_s32) break;
        }
	/* return total read length */
	return (total_read_len_s32);
}

#else
static uint32_t demuxer_fill_buffer_packet_process(int32_t buffer_len_s32,GstPluginTemplate * data)
{
	uint32_t read_len_s32, actual_read_len_s32,total_read_len_s32;
	char *p_read_buf_s8,*pread_buf_phy_u8;
        stream_t  st_packet_process;
        
	actual_read_len_s32 = 0;
	total_read_len_s32 = 0;
	//p_read_buf_s8 = p_buffer_s8;
	read_len_s32 = buffer_len_s32;

	pread_buf_phy_u8 = (char *)data->target;
	p_read_buf_s8 = pread_buf_phy_u8;

	st_packet_process.buff_idx = TSI_MEDIA_DATA_BUFFINDEX;
	st_packet_process.time_out = 250; //500 ms
             
        while(0<read_len_s32)
        {
            st_packet_process.buffer = (unsigned char*)p_read_buf_s8;
            st_packet_process.read_len = read_len_s32;

            /* use dma blocking call */
            actual_read_len_s32 = tsi_fill_buffer_dma(&st_packet_process);

            total_read_len_s32 += actual_read_len_s32;
            p_read_buf_s8 += actual_read_len_s32;
            read_len_s32 -= actual_read_len_s32;   
            
            if(0 == actual_read_len_s32) break;
        }
	/* return total read length */
	return (total_read_len_s32);
}

#endif

