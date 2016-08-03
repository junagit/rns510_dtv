#ifndef _TYPE_H_
#define _TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdint.h>

typedef void (*rc_ctrl_p) (char,void*);
typedef void (*timeout_p)(int);


#ifdef __cplusplus
}
#endif

#endif

