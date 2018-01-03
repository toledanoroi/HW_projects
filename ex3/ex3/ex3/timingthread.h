/*File Name :		timingthread.h
Authors:			Roi Toledano	203700505
					Yarden Avraham	302249925
Project Name:		Exercise 3 - Robotic Laundry Machine
Using:				roomate.h, machine.h , machine.c,roomate.c, timingthread.h, motherfile.c, motherfile.h
Description:		timingthread.c header; includes, structs and function declarations  */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "motherfile.h"

#define START_TIME 0
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/
typedef struct {

	DWORD program_time;
	DWORD time;
	bool time_flag;
	bool error_id_null;
	bool error_closing_handle;
	bool error_waiting;
	bool error_thread_creation;
	bool error_alloc;
	FILE *fp_debug;
	FILE *fp_report;
	char *report_file;
	char *log_file;
	mutex mutex_time;

	mutex mutex_debug_file;
	mutex mutex_report_file;

} timing_info;

typedef enum {
	TIMING_THREAD__CODE_SUCCESS,
	TIMING_THREAD__CODE_FAILED,
	TIMING_THREAD__CODE_ERROR = -1

} TIMING_THREAD__return_code;

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Mutexes and Semaphores*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
// Functions declarations
DWORD WINAPI TimeCounterThread(LPVOID lpParam);
HANDLE CreateTimingThreadSimple(LPTHREAD_START_ROUTINE timing_start_routine, timing_info *timing_parameters, DWORD *timing_thread_id, FILE *fp_debug, char *file_name);

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
