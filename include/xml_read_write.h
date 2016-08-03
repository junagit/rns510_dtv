/* 
// FIle name: cmmb_xml_handle.h
// Head file for channel information write and read
// Author: Jimye (jimye@solomon-systech.om)
// Copyright 2007(c)Solomon Systech Limited
*/
#ifndef _XML_READ_WRITE_H
#define _XML_READ_WRITE_H
#include <unistd.h>
#include "struct_define.h"

typedef struct dvb_string_conv
{	
  uint8_t type;	
  size_t  (*func) ( char *dst, size_t *dstlen,
   const uint8_t* src, size_t srclen );
	
} dvb_string_conv_t;

char * generate_xml_text(unsigned char * buf,unsigned int len);
//DTV_CHANNEL_INFORMATION * read_channel_info_from_xml(char * filename);
void dvbchar_convert_to_utf8(char *to, unsigned char *from, int length);
int dvbchar_init();
void dvbchar_exit();
//static inline size_t dvb_convert(int conv,const uint8_t *src, size_t srclen,char *dst, size_t *dstlen);
int dvb_get_string(char *dst, size_t dstlen, const uint8_t *src, size_t srclen, char *dvb_default_charset, dvb_string_conv_t *conv);
void descriptorcpy(char **dest, const unsigned char *src, size_t len);
#endif
