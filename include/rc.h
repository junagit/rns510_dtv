/*
 * File:   rc.h
 * Author: michalm
 *
 * Created on 8 marzec 2010, 08:15
 */

#ifndef _RC_H
#define	_RC_H

#define PILOT_NVS_02

#ifdef PILOT_NVS_02
enum rc_codes_RWT
{
Power=0x57,	//Klawisz pilota Power
_MUTE=0x5B,	//Klawisz pilota Mute

P1=0x1D,	//Klawisz pilota 1
P2=0x1E,	//Klawisz pilota 2
P3=0x06,	//Klawisz pilota 3
P4=0x7D,	//Klawisz pilota 4
P5=0x33,	//Klawisz pilota 5
P6=0x1B,	//Klawisz pilota 6
P7=0x4B,	//Klawisz pilota 7
P8=0x3D,	//Klawisz pilota 8
P9=0x2E,	//Klawisz pilota 9
P0=0x3B,	//Klawisz pilota 0
_NUMBER=0x6E,	//Klawisz pilota _/_
_RECORD=0x5D,	//Klawisz pilota Rec

MENU=0x23,	//Klawisz pilota Menu
 _EXIT_ =0x0E,	//Klawisz pilota Exit

OK=0x8B,	//Klawisz pilota OK
LEFT=0x2B,	//Klawisz pilota w Lewo  ' < '
RIGHT=0x6B,	//Klawisz pilota w Prawo ' > '
UP=0x43,	//Klawisz pilota w G�re ' /\ '
DOWN=0x03,	//Klawisz pilota w D��  ' \/ '

EPG=0x4A,	//Klawisz pilota EPG
FAST_SCAN=0xB7,	//Klawisz pilota FAST_SCAN

_F1=0x4E,	//Klawisz pilota F1
_F2=0x76,	//Klawisz pilota F2
_F3=0x3C,	//Klawisz pilota F3
_F4=0x62,	//Klawisz pilota F4

_DISPLAY=0x7E,	//Klawisz pilota DIS
_MINUS=0x3E,	//Klawisz pilota '-'
_PLUS=0x5E,	//Klawisz pilota '+'
_FORMAT=0x7C,	//Klawisz pilota FORMAT

INFO=0x0A,	//Klawisz pilota Info
_EQ=0x6D,	//Klawisz pilota EQ
_TV_R=0x72,	//Klawisz pilota TV_Radio
        
    REAR_ON = 0x51,   //Camera rear
    REAR_OFF = 0x53
};
#endif

#ifdef PILOT_RC5_1
enum rc_codes_RWT
{
    P1 = 0x01, //Pilot_NEC_1_NAVIHEVEN
    P2 = 0x02, //Pilot_NEC_1_NAVIHEVEN
    P3 = 0x03, //Pilot_NEC_1_NAVIHEVEN
    P4 = 0x04, //Pilot_NEC_1_NAVIHEVEN
    P5 = 0x05, //Pilot_NEC_1_NAVIHEVEN
    P6 = 0x06, //Pilot_NEC_1_NAVIHEVEN
    P7 = 0x07, //Pilot_NEC_1_NAVIHEVEN
    P8 = 0x08, //Pilot_NEC_1_NAVIHEVEN
    P9 = 0x09, //Pilot_NEC_1_NAVIHEVEN
    P0 = 0x00, //Pilot_NEC_1_NAVIHEVEN

   _MUTE=0x0D,
    EPG = 0x16, //Pilot_NEC_1_NAVIHEVEN
    MENU = 0x30, //Pilot_NEC_1_NAVIHEVEN
    INFO = 0x15, //Pilot_NEC_1_NAVIHEVEN
    _EXIT_ = 0x22, //Pilot_NEC_1_NAVIHEVEN

    OK = 0x3B, //Pilot_NEC_1_NAVIHEVEN
    LEFT = 0x11, //Pilot_NEC_1_NAVIHEVEN
    RIGHT = 0x10, //Pilot_NEC_1_NAVIHEVEN
    UP = 0x20, //Pilot_NEC_1_NAVIHEVEN
    DOWN = 0x21, //Pilot_NEC_1_NAVIHEVEN

    VOL_UP = 0x2B,//Pilot_NEC_1_NAVIHEVEN
    VOL_DOWN = 0x2C,//Pilot_NEC_1_NAVIHEVEN
    CH_UP = 0x3F, //Pilot_NEC_1_NAVIHEVEN
    CH_DOWN = 0x24, //Pilot_NEC_1_NAVIHEVEN

    P_RED = 0x37,//Pilot_NEC_1_NAVIHEVEN
    P_GREEN = 0x36,//Pilot_NEC_1_NAVIHEVEN
    P_YELLOW = 0x32,//Pilot_NEC_1_NAVIHEVEN
    P_BLUE = 0x34,//Pilot_NEC_1_NAVIHEVEN

    P_TXT = 0x23,
    P_SUT_T = 0x47,
    P_LANG = 0x3e,
    P_AUDIO = 0x07,
    REAR_ON = 0x51,   //Camera rear
    REAR_OFF = 0x53,

            _AUDIO=0x0E,
            ASPECT=0x24

};
#endif


#endif	/* _RC_H */

