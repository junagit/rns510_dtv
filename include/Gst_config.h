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

/* Define the version */
 #include <string.h>
 #include <stdlib.h>
 
//#define GST_PLAYER_TEMP_LOG //tom 1214 added 
#ifdef GST_PLAYER_TEMP_LOG
#define GST_DEBUG_Player(fmt, args...)    g_print("gstplayer %s:%d: %s "fmt, __FILE__, __LINE__, __FUNCTION__ , ##args)   
#else
#define GST_DEBUG_Player(fmt, args...)
#endif

#define MAIN_WIN_WIDTH 720
#define MAIN_WIN_HEIGHT 576
#define MAX_SUBTITLE_WIDTH 500
#define MAX_SUBTITLE_HEIGHT 100


#define GST_PLUGIN_VERSION "0.10.0.1"

/* Define the release version */
#define GST_PLUGIN_VERSION_RELEASE "0.20090307.163631"

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if valgrind should be used */
/* #undef HAVE_VALGRIND */

/* Name of package */
#define PACKAGE "gst-plugin-android"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "N/A"

/* Define to the full name of this package. */
#define PACKAGE_NAME "gst-plugin-android"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "gst-plugin-android-0.10.0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "gst-plugin-android"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.10.0.1"

/* directory where plugins are located */
#define PLUGINDIR  " "

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.10.0.1"



