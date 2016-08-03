/* 
 * File:   utils.h
 * Author: michalm
 *
 * Created on 1 marzec 2010, 08:34
 */

#ifndef _UTILS_H
#define	_UTILS_H


#define  SET_BIT(x,pos)         x |= (1<<pos)
#define  CLR_BIT(x,pos)         x &=~(1<<pos)
#define  TOGGLE_BIT(x,pos)      x ^=(1<<pos)
#define  TEST_BIT(x,pos)        (x & (1<<pos))



#endif	/* _UTILS_H */

