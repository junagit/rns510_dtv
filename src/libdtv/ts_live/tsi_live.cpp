/** 
 * @file   tsi_live.cpp
 * @author Michał Motyl
 *
 * @date   29 sierpień 2011, 09:18
 */

#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>


#ifdef TSI_LIVE
#include "tsi.h"
#include "gpio_ctrl.h"
#include "dvbtcontrol.h"
#include "interface.h"
#endif

