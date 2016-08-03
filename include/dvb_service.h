/** 
 * @file   dvb_service.h
 * @author michalm
 *
 * @date   20 grudzień 2011, 14:11
 */

#ifndef DVB_SERVICE_H
#define	DVB_SERVICE_H

/**
 * @brief Struktura opisująca pid audio-video
 */
typedef struct 
{
    uint8_t     service_pid_type;
    uint16_t    pid;    
            
}dvb_service_pid;


#endif	/* DVB_SERVICE_H */

