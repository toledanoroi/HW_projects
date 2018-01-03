/*File Name :		roomate.h
Authors:			Roi Toledano	203700505
					Yarden Avraham	302249925
Project Name:		Exercise 3 - Robotic Laundry Machine
Using:				machine.h , machine.c
Description:		roomate.c header; includes, structs and function declarations  */
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Includes and Defines*/
#include "timingthread.h"



/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Structs defenitions*/
typedef struct {

	int index;
	DWORD time_for_clothes_change;
	DWORD number_of_clothes;
	DWORD max_in_bin;
	timing_info *time_thread;
	int number_of_cloth_in_laundry;
	int number_of_cloth_in_closet;
	bool error_id_null;
	bool error_closing_handle;
	bool error_waiting;
	bool error_thread_creation;
	bool error_alloc;
	FILE *fp_debug;
	FILE *fp_report;
	char *report_file;
	char *log_file;
	semaphore semahore_closet_full;
	semaphore semahore_closet_empty;
	semaphore semaphore_laundry_bin_full;
	semaphore semaphore_laundry_bin_empty;
	mutex mutex_closet;
	mutex mutex_laundry_bin;

	mutex mutex_debug_file;
	mutex mutex_report_file;

} roommate_info;

typedef enum {
	ROOMMATE_THREAD__CODE_SUCCESS,
	ROOMMATE_THREAD__CODE_FAILED,
	ROOMMATE_THREAD__CODE_ERROR = -1

} ROOMMATE_THREAD__return_code;

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Mutexes and Semaphores*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
// Functions declarations
DWORD WINAPI RoommateThread(LPVOID lpParam);
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE roommate_start_routine, roommate_info *roommate_parameters, DWORD *roommate_thread_id, FILE *fp_debug, char *file_name);
int TakeCleanClothesFromCloset(roommate_info *roommate_thread_params);
int SendDirtyClothesToLaundry(roommate_info *roommate_thread_params);

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************END OF FILE**********************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------------------*/