/*
File Name : dvbtcontrol.h
Description : Control Code for SMS1010,including SMS1001 turner and SMS1002 demodulator
Reference : 1) Siano SMS1000 Host Control Library
                  2) <<Programming The SMS10xx APIs>>

*/
#ifndef _Turner_DVB
#define _Turner_DVB
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FREQ 858000
#define MIN_FREQ 474000
#define checkf(f)		( (f>=MinFreq) && (f<=MaxFreq))	//define the frequency range
#define BAND_WIDTH 8000


#define HW_NO_ERROR 			0
#define HW_INIT_LIB_ERROR	-1
#define HW_INIT_DEVICE_ERROR 	-2
#define HW_FREQ_ERROR 			-3
#define HW_WAITTIMEOUT_ERROR	-4
#define HW_TUNE_ERROR	-5
#define HW_GET_STATISTICS_REQ_ERROR 	-6
#define HW_GET_SIGNALSTRENGTH_REQ_ERROR 	-7
#define HW_LOADFW_ERROR  -8
#define HW_ADDPIDFILTER_ERROR -9
#define HW_REMOVEPIDFILTER_ERROR -10
#define HW_DEINIT_ERROR -11
#define HW_RETRIEVEPIDFILTER_ERROR -12

#ifndef BOOL
#define BOOL int
#endif
//Initiate the device sms1010
int HW_Initialized(void);
//Deinitiate the device
void HW_Close(void);
//request the demodulator to tune to a specific frequency
/*
bandwidth: 8000 8M
			7000 7M
			6000 6M
			5000 5M
			1500 1.5M
 */
/*
 *	@return: OK <1   Fail <0 
 * */
int HW_Lock(uint32_t freqency,uint32_t bandwidth);
/*
 HW_AddPidFilter
params:
	@PID  					: the pid will be both assigned to TSI and Siano 
	@packet_number_interrupt : 
 */
int HW_AddPidFilter(uint32_t PID);
int HW_RemovePidFilter(uint32_t PID);
int HW_Set_Manual (uint8_t mode, uint8_t rate, uint16_t guardinterval);

//read the sms1010's status
//int  HW_ReadStatus(void* pStatus);
int HW_ReadStatus(uint32_t *pSignalStrength,uint32_t *ber);
//get the signal strength
int HW_ReadSignalStrength(uint32_t *pSignalStrength);
//to get the demodulator being locked or not
BOOL HW_IsDemodLocked(void);
//
int HW_RetrievePidFilterList(uint32_t MaxPidNum,uint32_t* pActNum,uint32_t* pPidList);

//get the status value
int ReadSignalSet(uint8_t * mode, uint8_t * rate, /*INTERLEAVE_LENGTH* interleaverLength, */uint16_t * guardInterval);

//get the signal strength
int ReadSignalStrength(void);
//return the the value of SNR
uint32_t ReadSignalQuality(void);
//remove all the pid
int HW_RemoveAllPid();

int HW_Detect();

/*
 	print the statistics info
 */
int HW_StatusInfo(void* pStatus);
/*
 	dvb_tsi_retrieve_data
params:
		mem_buf  : buffer where data be filled
read_packet_num : packet number that will get

return :
		the exactly length of data that have been filled 
	
 */

#ifdef __cplusplus
}
#endif

#endif












