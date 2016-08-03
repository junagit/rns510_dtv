#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdint.h>


FILE *ts_file = NULL;


uint8_t open_ts_file(const char *file)
{
    if(ts_file!=NULL) return 1;


    ts_file = fopen(file,"r");

    if(ts_file==NULL) {
      //  ts_file = 0;
        return 2;
    }

    return 0;
}


void close_ts_file(void)
{
    if(ts_file!=NULL){
        fclose(ts_file);
        ts_file = NULL;
    }
    
}


