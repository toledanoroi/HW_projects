/* 
Authors		:	Roi Toledano	203700505
				Yarden Avraham	302249925
Project		:	Exercise 3 - Robotic Laundry Machine
Uses		:	
Description :	
*/
#include "roomate.h"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/
typedef struct {
	int bin_full;
	roommate_info *roommates;
	timing_info *time_thread;
	bool error_in_alloc;
	bool error_id_null;
	bool error_closing_handle;
	bool error_waiting;
	bool error_thread_creation;
	FILE *fp_debug;
	FILE *fp_report;
	char *report_file;
	char *log_file;
	semaphore semaphore_laundry_bin_full;
	semaphore semaphore_laundry_bin_empty;
	mutex mutex_laundry_bin;
	mutex mutex_time;
	int num_of_roommates;
} machine_info;

typedef enum {
	MACHINE_THREAD__CODE_SUCCESS,
	MACHINE_THREAD__CODE_FAILED,
	MACHINE_THREAD__CODE_ERROR = -1

} ROOMMATE_THREAD__return_code;

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Mutexes and Semaphores*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*functions declaration*/
HANDLE CreateThreadSimpleMachine(LPTHREAD_START_ROUTINE machine_start_routine, machine_info *machine_parameters, DWORD *machine_thread_id, FILE *fp_debug, char *file_name);
DWORD WINAPI LaundryMachineThread(LPVOID lpParam);

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/