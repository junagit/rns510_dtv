/* 
 * File:   uart_io.h
 * Author: michalm
 *
 * Created on 1 marzec 2010, 12:09
 */

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string>



using namespace std;

#ifndef _UART_IO_H
#define	_UART_IO_H


#define BAUDRATE B19200

#define _POSIX_SOURCE 1 /* POSIX compliant source */


#define UART_THREAD_SLEEP_TIME  50000

void signal_handler_IO (int status);   /* definition of signal handler */
void *uart_thrd( void *ptr );
void uart_exit();
int uart_init(string dev,void *user_data);
void uart_init_thread();

void remote_controll(void *v);




#endif	/* _UART_IO_H */

