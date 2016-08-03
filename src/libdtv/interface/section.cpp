

#include <stdio.h>
#include <stdlib.h>

#include <libucsi/mpeg/descriptor.h>
#include <libucsi/mpeg/section.h>
#include <libucsi/dvb/descriptor.h>
#include <libucsi/dvb/section.h>
#include <libucsi/atsc/descriptor.h>
#include <libucsi/atsc/section.h>
#include <libucsi/transport_packet.h>
#include <libucsi/section_buf.h>
#include <libucsi/dvb/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>

#include "section.h"
pthread_mutex_t section_mutex;


void init_section_mutex(void)
{
    pthread_mutex_init (&section_mutex,NULL);
}

void destroy_section_mutex(void)
{
    pthread_mutex_unlock(&section_mutex);
    pthread_mutex_destroy(&section_mutex);
}

#ifdef PC_VERSION
/**
 * 
 * @param pid
 * @param timeout
 * @param susr
 * @param _dvrfd
 * @return 
 */
uint8_t find_section(int16_t pid,uint32_t timeout,section_usr_p susr,FILE *_dvrfd)
{
	time_t starttime = time(NULL);

        
	while((time(NULL) - starttime) < (int32_t)timeout) {
            
		if ((susr->sz = fread(susr->databuf,1,188,_dvrfd)) < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "data overflow!\n");
				continue;
			} else if (errno == EAGAIN) {
				usleep(100);
				continue;
			} else {
				perror("read error");
				exit(1);
			}
		}
		for(susr->i=0; susr->i < susr->sz; susr->i+=TRANSPORT_PACKET_LENGTH) {
			// parse the transport packet
			susr->tspkt = transport_packet_init(susr->databuf + susr->i);
			if (susr->tspkt == NULL) {
				fprintf(stderr, "XXXX Bad sync byte\n");
				continue;
			}
			//pid = transport_packet_pid(tspkt);

                        if(pid!=transport_packet_pid(susr->tspkt)) continue;

			// extract all TS packet values even though we don't need them (to check for
			// library segfaults etc)
			if (transport_packet_values_extract(susr->tspkt, &susr->tsvals,(transport_value)0xffff) < 0) {
				fprintf(stderr, "XXXX Bad packet received (pid:%04x)\n", pid);
				continue;
			}

			// check continuity
			if (transport_packet_continuity_check(susr->tspkt,susr->tsvals.flags & transport_adaptation_flag_discontinuity,&susr->continuities)) {
				fprintf(stderr, "XXXX Continuity error (pid:%04x)\n", pid);
				susr->continuities = 0;
                                
				if (susr->section_bufs != NULL) {
					section_buf_reset(susr->section_bufs);
				}
				continue;
			}

			// allocate section buf if we don't have one already
			if (susr->section_bufs == NULL) {
				susr->section_bufs = (struct section_buf*)
					malloc(sizeof(struct section_buf) + DVB_MAX_SECTION_BYTES);
				if (susr->section_bufs == NULL) {
					fprintf(stderr, "Failed to allocate section buf (pid:%04x)\n", pid);
					exit(1);
				}
				section_buf_init(susr->section_bufs, DVB_MAX_SECTION_BYTES);
			}

			// process the payload data as a section
			while(susr->tsvals.payload_length) {
				susr->used = section_buf_add_transport_payload(susr->section_bufs,susr->tsvals.payload,susr->tsvals.payload_length,susr->tspkt->payload_unit_start_indicator,(int*)&susr->section_status);
				susr->tspkt->payload_unit_start_indicator = 0;
				susr->tsvals.payload_length -= susr->used;
				susr->tsvals.payload += susr->used;

				if (susr->section_status == 1) {

                                        return 0;
                                        
				} else if (susr->section_status < 0) {
					// some kind of error - just discard
					fprintf(stderr, "XXXX bad section %04x %i\n",pid, susr->section_status);
					section_buf_reset(susr->section_bufs);
				}
			}
		}
	}
        
        return 2;
}


#else

/**
 * @brief Funkcja pobiera całą sekcje danych dla DVB
 * @param pid - szukany pid
 * @param susr - wskaźnik na dane wykorzystywane przez tą funkcję
 * @param my_stream - wskaźnik na dane wykorzystywane przez tsi_fill_buffer
 * @return 0 - OK, 3 - timeout
 */
uint8_t find_section_live(int16_t pid,section_usr_p susr,stream_t *my_stream,int tout)
{
    int32_t timeout = tout*1000;
    //time_t starttime;
    int time_wait = 0;
    struct timeval start_time,end_time, find_time;
    
    	//starttime = time(NULL);
        gettimeofday(&start_time,NULL);// start to count time.
        gettimeofday(&find_time,NULL);// start to count time.
        
	while(1)
        {
            gettimeofday(&end_time,NULL);
	    time_wait = (int)((end_time.tv_sec - start_time.tv_sec )*1000+(end_time.tv_usec-start_time.tv_usec)/1000);
                
                if(time_wait >= timeout)
                {
                    break;
                }
            
            pthread_testcancel();
            
		susr->sz = tsi_fill_buffer(my_stream);

                if(susr->sz<=0) continue;
                
                
		for(susr->i=0; susr->i < susr->sz; susr->i+=TRANSPORT_PACKET_LENGTH) {
			// parse the transport packet
                    //pthread_testcancel();
                    
			susr->tspkt = transport_packet_init(susr->databuf + susr->i);
			if (susr->tspkt == NULL) {
				//printf("XXXX Bad sync byte\n");
				continue;
			}

                        if(pid!=transport_packet_pid(susr->tspkt)) continue;

			// extract all TS packet values even though we don't need them (to check for
			// library segfaults etc)
			if (transport_packet_values_extract(susr->tspkt, &susr->tsvals,(transport_value)0xffff) < 0) {
				printf("XXXX Bad packet received (pid:%04x)\n", pid);
				continue;
			}

			// check continuity
			if (transport_packet_continuity_check(susr->tspkt,susr->tsvals.flags & transport_adaptation_flag_discontinuity,&susr->continuities)) {
				printf("XXXX Continuity error (pid:%04x)\n", pid);
				susr->continuities = 0;
                                
				if (susr->section_bufs != NULL) {
					section_buf_reset(susr->section_bufs);
				}
				continue;
			}

			// allocate section buf if we don't have one already
			if (susr->section_bufs == NULL) {
				susr->section_bufs = (struct section_buf*) malloc((sizeof(struct section_buf) + DVB_MAX_SECTION_BYTES));
				if (susr->section_bufs == NULL) {
					printf("### Failed to allocate section buf (pid:%04x)  data len:%d\n", pid,(sizeof(struct section_buf) + DVB_MAX_SECTION_BYTES));
                                    return 3;
				}
				section_buf_init(susr->section_bufs, DVB_MAX_SECTION_BYTES);
			}

			// process the payload data as a section
			while(susr->tsvals.payload_length) {
				susr->used = section_buf_add_transport_payload(susr->section_bufs,susr->tsvals.payload,susr->tsvals.payload_length,susr->tspkt->payload_unit_start_indicator,(int*)&susr->section_status);
				susr->tspkt->payload_unit_start_indicator = 0;
				susr->tsvals.payload_length -= susr->used;
				susr->tsvals.payload += susr->used;

				if (susr->section_status == 1) {
                                      //  printf("find_section_live ok????? len:%d\n",susr->section_bufs->len);
                                        return 0;
                                        
				} else if (susr->section_status < 0) {
					// some kind of error - just discard
					printf("XXXX bad section %04x %i\n",pid, susr->section_status);
					section_buf_reset(susr->section_bufs);
				}
                                
			}
                        
		}
	}
        
        return 3;
}

uint8_t find_section_live2(int16_t pid,section_usr_p susr,stream_t *my_stream)
{
        susr->sz = tsi_fill_buffer(my_stream);

        if(susr->sz<=0)
        {
           // printf("XXXX susr->sz<=0\n");
            return 1;
        }
               
        for(susr->i=0; susr->i < susr->sz; susr->i+=TRANSPORT_PACKET_LENGTH) {
                    
                susr->tspkt = transport_packet_init(susr->databuf + susr->i);
                if (susr->tspkt == NULL) {
                 //       printf("XXXX Bad sync byte\n");
                        return 1;
                }

                if(pid!=transport_packet_pid(susr->tspkt)) continue;

                // extract all TS packet values even though we don't need them (to check for
                // library segfaults etc)
                if (transport_packet_values_extract(susr->tspkt, &susr->tsvals,(transport_value)0xffff) < 0) {
                        printf("XXXX Bad packet received (pid:%04x)\n", pid);
                        return 1;
                }

                // check continuity
                if (transport_packet_continuity_check(susr->tspkt,susr->tsvals.flags & transport_adaptation_flag_discontinuity,&susr->continuities)) {
                        printf("XXXX Continuity error (pid:%04x)\n", pid);
                        susr->continuities = 0;

                        if (susr->section_bufs != NULL) {
                                section_buf_reset(susr->section_bufs);
                        }
                        return 1;
                }

                // allocate section buf if we don't have one already
                if (susr->section_bufs == NULL) {
                        susr->section_bufs = (struct section_buf*) malloc((sizeof(struct section_buf) + DVB_MAX_SECTION_BYTES));
                        if (susr->section_bufs == NULL) {
                                printf("Failed to allocate section buf (pid:%04x)  data len:%d\n", pid,(sizeof(struct section_buf) + DVB_MAX_SECTION_BYTES));
                            return 1;
                        }
                        section_buf_init(susr->section_bufs, DVB_MAX_SECTION_BYTES);
                }

                // process the payload data as a section
                while(susr->tsvals.payload_length) {
                        susr->used = section_buf_add_transport_payload(susr->section_bufs,susr->tsvals.payload,susr->tsvals.payload_length,susr->tspkt->payload_unit_start_indicator,(int*)&susr->section_status);
                        susr->tspkt->payload_unit_start_indicator = 0;
                        susr->tsvals.payload_length -= susr->used;
                        susr->tsvals.payload += susr->used;

                        if (susr->section_status == 1) {
                           //     printf("find_section_live ok????? len:%d\n",susr->section_bufs->len);
                                return 0;

                        } else if (susr->section_status < 0) {
                                // some kind of error - just discard
                                printf("XXXX bad section %04x %i\n",pid, susr->section_status);
                                section_buf_reset(susr->section_bufs);
                        }

                }
                        
		
	}
        
        return 1;
}

#endif